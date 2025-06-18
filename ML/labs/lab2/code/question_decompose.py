from transformers import BertModel, BertTokenizer
import torch
import numpy as np
from sklearn.metrics.pairwise import cosine_similarity
from openai import OpenAI
import json
import re
import string
import collections
import pickle
from tqdm import tqdm
import os
from typing import List, Dict, Tuple

# --- Configuration ---
API_KEY = "sk-f1310a3207154eeba98d3985ae854e66"  # 替换为您的DeepSeek API密钥
BERT_MODEL_NAME = "bert-base-uncased"  # 使用BERT进行嵌入
CHUNK_SIZE = 256  # 文档分块大小（字符数）
TOP_K = 10 # 检索最相关的K个文档块
VECTOR_STORE_PATH = "bert_vectors.pkl"  # 向量存储文件路径
CONTEXT_MAP_PATH = "chunk_to_context.pkl"   # 新增：chunk到context的映射文件路径
BERT_MODEL_NAME = "bert-base-uncased"
os.environ["ONEDNN_OPTS"] = '0'  # 禁用TensorFlow的OneDNN优化以提高性能

# Initialize DeepSeek client
llm = OpenAI(api_key=API_KEY, base_url="https://api.deepseek.com/v1")

# Initialize BERT model for embeddings
tokenizer = BertTokenizer.from_pretrained(BERT_MODEL_NAME)
bert_model = BertModel.from_pretrained(BERT_MODEL_NAME)
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
bert_model = bert_model.to(device)


def decompose_question(question: str, max_subquestions=3) -> List[str]:
    """Decompose complex questions into sub-questions using DeepSeek API"""
    decomposition_prompt = f"""
    Please decompose the following complex question into {max_subquestions} independent sub-questions 
    that can be answered individually. Return in this exact format without additional explanations:
    
    Original question: {question}
    Sub-question 1: [First sub-question]
    Sub-question 2: [Second sub-question]
    ...
    """
    
    try:
        response = llm.chat.completions.create(
            model="deepseek-chat",
            messages=[
                {"role": "system", "content": "You are an expert question decomposition assistant skilled at breaking down complex questions into independent sub-questions."},
                {"role": "user", "content": decomposition_prompt}
            ],
            temperature=0.3,
            max_tokens=200
        )
        
        # Parse the returned sub-questions
        generated_text = response.choices[0].message.content
        subquestions = []
        for line in generated_text.split('\n'):
            if line.strip().startswith("Sub-question") and ':' in line:
                subq = line.split(':', 1)[1].strip()
                if not subq.endswith('?'):
                    subq += '?'
                subquestions.append(subq)
        
        return subquestions[:max_subquestions] if subquestions else [question]
    
    except Exception as e:
        print(f"Question decomposition failed: {e}")
        return [question]  # Fallback to original question

def multi_hop_retrieve_and_answer(question: str, embeddings: np.ndarray, 
                                chunks: List[str],                               max_hops=2) -> str:
    """
    完整的多跳问答流程：
    1. 分解问题得到子问题
    2. 为每个子问题检索相关上下文
    3. 回答每个子问题
    4. 综合子答案回答原问题
    """
    # 1. 问题分解（只执行一次）
    subquestions = decompose_question(question)
    if len(subquestions) == 1:  # 如果无法分解，直接回答
        contexts = retrieve_relevant_chunks(question, embeddings, chunks)
        return query_model(build_prompt(question, contexts[0]))
    
    # 2. 为每个子问题检索上下文
    subquestion_contexts = {}
    for subq in subquestions:
        contexts = retrieve_relevant_chunks(subq, embeddings, chunks)
        if contexts:
            subquestion_contexts[subq] = contexts[0]  # 取最相关的上下文
    
    # 3. 回答每个子问题
    sub_answers = []
    for subq, ctx in subquestion_contexts.items():
        answer = query_model(build_prompt(subq, ctx))
        sub_answers.append(f"Q: {subq}\nA: {answer}")
    
    # 4. 综合回答原问题
    if not sub_answers:
        return "I couldn't find enough information to answer this question."
    
    answers_str = '\n'.join(sub_answers)
    synthesis_prompt = f"""
    Sub-questions and their answers:
    {answers_str}
    
    Original question: {question}
    Based on the above information, provide a direct and concise final answer.
    """ +   ("Instruction: Based *only* on the provided context, answer the question concisely. "
            "For Yes/No questions, answer only 'Yes' or 'No'. "
            "For other questions, provide only the exact answer phrase (e.g., a name, date, number, or short phrase), "
            "without forming a complete sentence.")
    print(f"Synthesis prompt: {synthesis_prompt}")
    return query_model([{"role": "user", "content": synthesis_prompt}])

def is_relevant(question: str, context: str) -> bool:
    """判断context是否与问题相关"""
    inputs = tokenizer(question, context, return_tensors="pt", truncation=True).to(device)
    with torch.no_grad():
        outputs = bert_model(**inputs)
    similarity = outputs.last_hidden_state[:, 0, :].mean().item()
    return similarity > 0.7  # 阈值可调整

def build_prompt(question: str, context: str) -> List[dict]:
    """构建对话提示"""
    return [
        {"role": "system", "content": "You are a factual question answering assistant."},
        {"role": "user", "content": f"Context: {context}\nQuestion: {question}\nAnswer concisely:"}
    ]

# --- Document Processing Functions ---
def split_into_chunks(text: str, chunk_size: int = CHUNK_SIZE) -> List[str]:
    """Split a document into chunks of approximately equal size."""
    words = text.split()
    chunks = []
    current_chunk = []
    current_length = 0
    
    for word in words:
        if current_length + len(word) + 1 > chunk_size and current_chunk:
            chunks.append(" ".join(current_chunk))
            current_chunk = []
            current_length = 0
        current_chunk.append(word)
        current_length += len(word) + 1
    
    if current_chunk:
        chunks.append(" ".join(current_chunk))
    
    return chunks

def get_bert_embedding(text: str) -> np.ndarray:
    """Get BERT embedding for a piece of text."""
    inputs = tokenizer(text, return_tensors="pt", padding=True, truncation=True, max_length=512).to(device)
    with torch.no_grad():
        outputs = bert_model(**inputs)
    # Use mean pooling of the last hidden states
    embeddings = outputs.last_hidden_state.mean(dim=1).squeeze().cpu().numpy()
    return embeddings

def save_vector_store(embeddings: np.ndarray, chunks: list, chunk_to_context: dict, 
                     vector_path: str = VECTOR_STORE_PATH
                     ):
    """保存向量存储和chunk到context的映射到文件"""
    with open(vector_path, "wb") as f:
        pickle.dump({"embeddings": embeddings, "chunks": chunks}, f)

def load_vector_store(vector_path: str = VECTOR_STORE_PATH, 
                     ) -> tuple:
    
    with open(vector_path, "rb") as f:
        vector_data = pickle.load(f)

    
    return vector_data["embeddings"], vector_data["chunks"]

def build_or_load_vector_store(contexts: list, force_rebuild: bool = False) -> tuple:
    """构建或加载向量存储和映射"""
    if not force_rebuild:
        embeddings, chunks = load_vector_store()
        if embeddings is not None:
            return embeddings, chunks
    
    # 需要重新构建
    embeddings = []
    chunks = []
    
    for ctx_id, context in enumerate(tqdm(contexts)):
        context_chunks = split_into_chunks(context)
        for chunk in context_chunks:
            embedding = get_bert_embedding(chunk)
            embeddings.append(embedding)
            chunks.append(chunk)

    
    embeddings = np.array(embeddings)
    save_vector_store(embeddings, chunks)
    return embeddings, chunks

def retrieve_relevant_chunks(question: str, embeddings: np.ndarray, chunks: List[str], top_k: int = TOP_K) -> List[str]:
    """Retrieve top-k most relevant chunks for a question."""
    question_embedding = get_bert_embedding(question).reshape(1, -1)
    similarities = cosine_similarity(question_embedding, embeddings)[0]
    top_indices = np.argsort(similarities)[-top_k:][::-1]
    print(f"Top {top_k} indices: {top_indices}")  
    return [chunks[i] for i in top_indices]

# --- Data Loading ---
def load_dataset(file_path="hotpotqa_longbench.json", num_samples=None):
    """Loads data from a JSON file."""
    with open(file_path, "r", encoding="utf-8") as f:  # 明确指定utf-8编码
        data = json.load(f)
    if num_samples is not None:
        return data[:num_samples]
    return data

# --- Model Interaction ---
def query_model(messages: List[Dict], max_tokens: int = 64) -> str:
    """Queries the DeepSeek model with the provided messages."""
    try:
        response = llm.chat.completions.create(
            model="deepseek-chat",
            messages=messages,
            max_tokens=max_tokens,
            temperature=0.3,  # Lower temperature for more factual answers
        )
        return response.choices[0].message.content
    except Exception as e:
        print(f"Error querying model: {e}")
        return None

# --- Evaluation Functions ---
def normalize_answer(s: str) -> str:
    """Lower text and remove punctuation, articles and extra whitespace."""
    def remove_articles(text):
        return re.sub(r'\b(a|an|the)\b', ' ', text)
    def white_space_fix(text):
        return ' '.join(text.split())
    def remove_punc(text):
        exclude = set(string.punctuation)
        return ''.join(ch for ch in text if ch not in exclude)
    def lower(text):
        return text.lower()
    return white_space_fix(remove_articles(remove_punc(lower(s))))

def compute_em(a_gold: str, a_pred: str) -> int:
    """Computes Exact Match score."""
    return int(normalize_answer(a_gold) == normalize_answer(a_pred))

def compute_f1(a_gold: str, a_pred: str) -> float:
    """Computes F1 score."""
    gold_toks = normalize_answer(a_gold).split()
    pred_toks = normalize_answer(a_pred).split()

    if not gold_toks or not pred_toks:
        return int(gold_toks == pred_toks)

    common = collections.Counter(gold_toks) & collections.Counter(pred_toks)
    num_same = sum(common.values())

    if num_same == 0:
        return 0

    precision = 1.0 * num_same / len(pred_toks)
    recall = 1.0 * num_same / len(gold_toks)
    f1 = (2 * precision * recall) / (precision + recall)
    return f1

# --- Score Calculation and Printing ---
def calculate_and_print_scores(results: List[Dict]) -> Tuple[float, float]:
    """Calculates EM/F1 scores for results, prints them, and returns averages."""
    em_scores = []
    f1_scores = []
    
    print("=" * 80)
    print("Individual Results:")
    print("=" * 80)

    for i, result in enumerate(results):
        gold = result['golden_answer']
        pred = result['predicted_answer']
        
        em = compute_em(gold, pred)
        f1 = compute_f1(gold, pred)
        
        result['em'] = em
        result['f1'] = f1
        em_scores.append(em)
        f1_scores.append(f1)

        print(f"--- Sample {i+1} (ID: {result['id']}) ---")
        print(f"Question: {result['question']}")
        print(f"Predicted Answer: {pred}")
        print(f"Golden Answer: {gold}")
        print(f"EM: {em}")
        print(f"F1: {f1:.4f}")
        print("-" * 40)

    avg_em = np.mean(em_scores) if em_scores else 0
    avg_f1 = np.mean(f1_scores) if f1_scores else 0

    print("=" * 80)
    print("Overall Scores:")
    print("=" * 80)
    print(f"Average EM: {avg_em:.4f} ({avg_em * 100:.2f}%)")
    print(f"Average F1: {avg_f1:.4f} ({avg_f1 * 100:.2f}%)")
    print("=" * 80)

    return avg_em, avg_f1

# --- Save Results Function ---
def save_results_to_json(results: List[Dict], output_path: str):
    """Saves the prediction results to a JSON file."""
    output_data = [{'id': r['id'], 'pred_answer': r['predicted_answer'],'real_answer': r['golden_answer']} for r in results]

    print(f"Saving results to {output_path}...")
    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(output_data, f, ensure_ascii=False, indent=4)
    print("Results saved successfully.")

# --- Main Execution Logic ---
def run_evaluation(num_samples_to_run: int = 200):
    """Loads data, queries model, evaluates results, and saves predictions."""
    print(f"Loading dataset (first {num_samples_to_run} samples)...")
    dataset = load_dataset(num_samples=num_samples_to_run)
    print(f"Loaded {len(dataset)} samples.")
    
    # Extract contexts from dataset
    contexts = [item['context'] for item in dataset]
    
    # Build vector store
    embeddings, chunks = build_or_load_vector_store(contexts)
    
    results = []
    print("Querying model for each sample...")
    for i, item in enumerate(tqdm(dataset)):
        print(f"Processing sample {i+1}/{len(dataset)} (ID: {item['id']})...")
        
        predicted_answer = multi_hop_retrieve_and_answer(
            item['question'],
            embeddings,
            chunks
            
        )
        
        if predicted_answer is not None:
            results.append({
                'id': item['id'],
                'question': item['question'],
                'predicted_answer': predicted_answer,
                'golden_answer': item['answer']
            })
        else:
            print(f"Skipping sample {i+1} due to model query error.")

    print("Calculating and printing scores...")
    avg_em, avg_f1 = calculate_and_print_scores(results)
    print(f"Final Average EM: {avg_em:.4f}, Final Average F1: {avg_f1:.4f}")

    # --- Save Results ---
    output_dir = "results"
    os.makedirs(output_dir, exist_ok=True)
    output_path = os.path.join(output_dir, "outputs.json")
    save_results_to_json(results, output_path)
    
    print("Evaluation complete!")

if __name__ == "__main__":

    run_evaluation(num_samples_to_run=10)