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
TOP_K = 3 # 检索最相关的K个文档块
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
                     vector_path: str = VECTOR_STORE_PATH, 
                     context_map_path: str = CONTEXT_MAP_PATH):
    """保存向量存储和chunk到context的映射到文件"""
    with open(vector_path, "wb") as f:
        pickle.dump({"embeddings": embeddings, "chunks": chunks}, f)
    with open(context_map_path, "wb") as f:
        pickle.dump(chunk_to_context, f)

def load_vector_store(vector_path: str = VECTOR_STORE_PATH, 
                     context_map_path: str = CONTEXT_MAP_PATH) -> tuple:
    """从文件加载向量存储和chunk到context的映射"""
    if not os.path.exists(vector_path) or not os.path.exists(context_map_path):
        return None, None, None
    
    with open(vector_path, "rb") as f:
        vector_data = pickle.load(f)
    with open(context_map_path, "rb") as f:
        chunk_to_context = pickle.load(f)
    
    return vector_data["embeddings"], vector_data["chunks"], chunk_to_context

def build_or_load_vector_store(contexts: list, force_rebuild: bool = False) -> tuple:
    """构建或加载向量存储和映射"""
    if not force_rebuild:
        embeddings, chunks, chunk_to_context = load_vector_store()
        if embeddings is not None:
            print(f"Loaded vector store with {len(chunks)} chunks and {len(chunk_to_context)} mappings.")
            return embeddings, chunks, chunk_to_context
    
    # 需要重新构建
    embeddings = []
    chunks = []
    chunk_to_context = {}
    
    for ctx_id, context in enumerate(tqdm(contexts)):
        context_chunks = split_into_chunks(context)
        for chunk in context_chunks:
            embedding = get_bert_embedding(chunk)
            embeddings.append(embedding)
            chunks.append(chunk)
            chunk_to_context[chunk] = context  # 使用chunk文本作为key
    
    embeddings = np.array(embeddings)
    save_vector_store(embeddings, chunks, chunk_to_context)
    return embeddings, chunks, chunk_to_context

def retrieve_relevant_chunks(question: str, embeddings: np.ndarray, chunks: List[str],chunk_to_context:dict, top_k: int = TOP_K) -> List[str]:
    """Retrieve top-k most relevant chunks for a question."""
    question_embedding = get_bert_embedding(question).reshape(1, -1)
    similarities = cosine_similarity(question_embedding, embeddings)[0]
    top_indices = np.argsort(similarities)[-top_k:][::-1]
    print(f"Top {top_k} indices: {top_indices}")
     # 获取top-k chunks对应的contexts
    retrieved_contexts = []
    seen_contexts = set()  # 避免重复context
    for i in top_indices:
        chunk = chunks[i]
        context = chunk_to_context.get(chunk)
        if context and context not in seen_contexts:
            retrieved_contexts.append(context)
            seen_contexts.add(context)
            if len(retrieved_contexts) >= top_k:
                break
    print(f"Retrieved {len(retrieved_contexts)} relevant contexts")
    
    return retrieved_contexts

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
    embeddings, chunks, chunk_to_context = build_or_load_vector_store(contexts)
    
    results = []
    print("Querying model for each sample...")
    for i, item in enumerate(tqdm(dataset)):
        print(f"Processing sample {i+1}/{len(dataset)} (ID: {item['id']})...")
        
        # Retrieve relevant chunks using BERT
        relevant_chunks = retrieve_relevant_chunks(item['question'], embeddings, chunks,chunk_to_context)
        context = "\n\n".join(relevant_chunks)

        # Format query for DeepSeek
        query_to_model = (
            f"Context: {context}\n\n"
            f"Question: {item['question']}\n\n"
            "Instruction: Based *only* on the provided context, answer the question concisely. "
            "For Yes/No questions, answer only 'Yes' or 'No'. "
            "For other questions, provide only the exact answer phrase (e.g., a name, date, number, or short phrase), "
            "without forming a complete sentence."
        )
        
        messages = [
            {"role": "system", "content": "You are a helpful assistant in answering multi-hop questions."},
            {"role": "user", "content": query_to_model}
        ]
        
        predicted_answer = query_model(messages)
        
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

    run_evaluation(num_samples_to_run=200)