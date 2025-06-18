from openai import OpenAI
from datasets import load_from_disk
import json
import argparse
import base64
from tqdm import tqdm
import os
import pytesseract  # 新增OCR库
from openai import OpenAI
from transformers import BertTokenizer, BertModel
import torch
import numpy as np


client = OpenAI(
    base_url="http://47.242.151.133:24576/v1/",
    api_key="ml2025",
)
# Initialize BERT model for embeddings
tokenizer = BertTokenizer.from_pretrained("bert-base-uncased")
bert_model = BertModel.from_pretrained("bert-base-uncased")
pytesseract.pytesseract.tesseract_cmd = r'E:\Tesseract\tesseract.exe'  # Update this path to your Tesseract installation

def load_data(path):
    ds = load_from_disk(path)
    return ds


def get_text_embedding(text):
    """
    Get BERT embedding for text
    """
    inputs = tokenizer(
        text, return_tensors="pt", padding=True, truncation=True, max_length=512
    )
    with torch.no_grad():
        outputs = bert_model(**inputs)
    return outputs.last_hidden_state.mean(dim=1).squeeze().numpy()


def get_image_text(image):
    """
    Extract text from image using OCR
    """
    # Convert image to grayscale for better OCR results
    if image is not None and image.mode != "L" :
        image = image.convert("L")
    # Apply some basic preprocessing
    image = image.point(lambda x: 0 if x < 128 else 255, "1")
    return pytesseract.image_to_string(image)


def preprocess_image(example):
    """
    Preprocess the image using OCR and RAG to select the most relevant image
    Args:
        example: dict, the example
    Returns:
        image: Image, the most relevant image
    """
    # Get question embedding
    question_embedding = get_text_embedding(example["question"])

    # Process each image and calculate similarity
    best_image = None
    best_similarity = -1

    # Iterate through all available images (assuming up to 5 images)
    for i in range(1, 20):
        image_key = f"image_{i}"
        if image_key not in example or example[image_key] is None:
            continue

        image = example[image_key]

        # Extract text from image
        image_text = get_image_text(image)

        if not image_text.strip():  # Skip if no text found
            continue

        # Get text embedding
        text_embedding = get_text_embedding(image_text)

        # Calculate cosine similarity
        similarity = np.dot(question_embedding, text_embedding) / (
            np.linalg.norm(question_embedding) * np.linalg.norm(text_embedding)
        )

        # Update best image if current is better
        if similarity > best_similarity:
            best_similarity = similarity
            best_image = image

    # Fallback to first image if no text was found in any image
    if best_image is None:
        return example["image_1"]

    return best_image


def generate_answer(example):
    # 预处理图像（现在使用OCR+RAG选择的最佳图像）
    image = preprocess_image(example)

    # 转换为base64
    tmp_dir = "./tmp"
    os.makedirs(tmp_dir, exist_ok=True)
    tmp_file = os.path.join(tmp_dir, "tmp_image.png")
    image.save(tmp_file, format="PNG")

    with open(tmp_file, "rb") as f:
        encoded_image = base64.b64encode(f.read())
    encoded_image_text = encoded_image.decode("utf-8")
    base64_image = f"data:image;base64,{encoded_image_text}"

    # 提示词
    text = f"{example['question']}\nOnly return the answer, no other words."

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
                    {"type": "text", "text": text},
                ],
            },
        ],
    )
    return chat_response.choices[0].message.content


def evaluate_results(results):
    score = 0
    for result in results:
        if result["generation"].lower() in result["answers"].lower():
            score += 1
    return round(score / len(results), 2)


def main(args):
    if not args.eval_only:
        ds = load_data(args.data_path)
        results = []
        for example in tqdm(
            ds, desc="Generating answers", total=len(ds), leave=True, position=0
        ):
            answer = generate_answer(example)
            results.append({"generation": answer, "answers": example["answers"]})

        pass_rate = evaluate_results(results)
        print(f"Pass rate: {pass_rate}")

        output_dir = os.path.dirname(args.output_path)
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
        with open(args.output_path, "w") as f:
            json.dump(results, f)
    else:
        with open(args.output_path, "r") as f:
            results = json.load(f)
        pass_rate = evaluate_results(results)
        print(f"Pass rate: {pass_rate}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--data_path", type=str, default="./data/mp_docvqa_100")
    parser.add_argument("--output_path", type=str, default="./results/ocr_rag.json")
    parser.add_argument("--eval_only", action="store_true")
    args = parser.parse_args()
    main(args)
