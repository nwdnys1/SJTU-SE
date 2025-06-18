from openai import OpenAI
from datasets import load_from_disk
import json
import argparse
import base64
from tqdm import tqdm
import os

client = OpenAI(
    base_url="http://47.242.151.133:24576/v1/",
    api_key="ml2025",
)


def load_data(path):
    ds = load_from_disk(path)
    return ds


def preprocess_image(example):
    # 保持原始图像处理不变
    image = example["image"]
    return image


def generate_answer(example):
    image = preprocess_image(example)

    # 图像处理部分保持不变
    tmp_dir = "./tmp"
    os.makedirs(tmp_dir, exist_ok=True)
    tmp_file = os.path.join(tmp_dir, "tmp_image.png")
    image.save(tmp_file, format="PNG")

    with open(tmp_file, "rb") as f:
        encoded_image = base64.b64encode(f.read())
    encoded_image_text = encoded_image.decode("utf-8")
    base64_image = f"data:image;base64,{encoded_image_text}"

    # ==================== PROMPT优化部分 ====================
    # 优化策略：
    # 1. 添加任务说明和格式要求
    # 2. 提供具体示例（few-shot）
    # 3. 强调文档理解的特殊性
    # ==================== 优化后的PROMPT ====================
    prompt = f"""
    You are an expert in Document Visual Question Answering (DocVQA). 
    Your task is to answer questions based on the given document image.
    
    Important Guidelines:
    - Focus on textual content in the document
    - Pay attention to document structure (tables, forms, etc.)
    - For numerical answers, provide the exact value as shown
    - For date questions, use the format in the document
    
    Question: {example['question']}
    
    Examples:
    Q: What is the invoice number?
    A: INV-2023-0042
    
    Q: What is the total amount due?
    A: $1,245.00
    
    Now answer this question (only the answer, no other words):
    {example['question']}
    """
    # ==================== END 优化PROMPT ====================

    chat_response = client.chat.completions.create(
        model="Qwen/Qwen2.5-VL-3B-Instruct",
        messages=[
            {"role": "system", "content": "You are a helpful assistant."},
            {
                "role": "user",
                "content": [
                    {
                        "type": "image_url",
                        "image_url": {"url": base64_image},
                    },
                    {"type": "text", "text": prompt},  # 使用优化后的prompt
                ],
            },
        ],
    )
    return chat_response.choices[0].message.content


def evaluate_results(results, save_errors=False, error_dir="./error_cases"):
    """
    Evaluate the results and optionally save error cases
    Args:
        results: list, the results
        save_errors: bool, whether to save error cases
        error_dir: str, directory to save error cases
    Returns:
        score: float, the score
    """
    # Create directory for error cases if needed
    if save_errors and not os.path.exists(error_dir):
        os.makedirs(error_dir)

    score = 0
    error_cases = []

    for idx, result in enumerate(results):
        lower_answers = [answer.lower() for answer in result["answers"]]
        if result["generation"].lower() in lower_answers:
            score += 1
        elif save_errors:
            # Add additional information for error cases
            error_case = {
                "index": idx,
                "question": result.get("question", ""),
                "generated_answer": result["generation"],
                "correct_answers": result["answers"],
                "image_path": result.get(
                    "image_path", ""
                ),  # Assuming image path is stored
            }
            error_cases.append(error_case)

    # Save error cases to JSON file
    if save_errors and error_cases:
        error_file = os.path.join(error_dir, "error_cases.json")
        with open(error_file, "w") as f:
            json.dump(error_cases, f, indent=2)
        print(f"Saved {len(error_cases)} error cases to {error_file}")

    return round(score / len(results), 2)


def main(args):
    if not args.eval_only:
        # Load data
        ds = load_data(args.data_path)

        # Generate
        results = []
        for idx, example in tqdm(
            enumerate(ds),
            desc="Generating answers",
            total=len(ds),
            leave=True,
            position=0,
        ):
            answer = generate_answer(example)

            # Store additional information for potential error analysis
            result = {
                "index": idx,
                "question": example["question"],
                "generation": answer,
                "answers": example["answers"],
                "image_path": f"tmp/tmp_image_{idx}.png",  # Save each image with index
            }

            # Save individual image for error analysis
            image = preprocess_image(example)
            os.makedirs("tmp", exist_ok=True)
            image.save(f"tmp/tmp_image_{idx}.png", format="PNG")

            results.append(result)

        # Evaluate with error saving
        pass_rate = evaluate_results(
            results, save_errors=True, error_dir=args.error_dir
        )
        print(f"Pass rate: {pass_rate}")

        # Save results to disk
        output_dir = os.path.dirname(args.output_path)
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
        with open(args.output_path, "w") as f:
            json.dump(results, f, indent=2)
    else:
        # Load results
        with open(args.output_path, "r") as f:
            results = json.load(f)
        # Evaluate with error saving
        pass_rate = evaluate_results(
            results, save_errors=True, error_dir=args.error_dir
        )
        print(f"Pass rate: {pass_rate}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--data_path", type=str, default="./data/docvqa_100")
    parser.add_argument(
        "--output_path", type=str, default="./results/prompt.json"
    )
    parser.add_argument(
        "--error_dir", type=str, default="./errors/prompt"
    )  # New argument
    parser.add_argument("--eval_only", action="store_true")
    args = parser.parse_args()

    main(args)
