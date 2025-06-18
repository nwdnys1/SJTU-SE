from openai import OpenAI
from datasets import load_from_disk
import json
import argparse
import base64
from tqdm import tqdm
import os
from PIL import Image, ImageEnhance, ImageFilter  # 新增图像增强
import pytesseract  # 新增OCR

client = OpenAI(
    base_url="http://47.242.151.133:24576/v1/",
    api_key="ml2025",
)
pytesseract.pytesseract.tesseract_cmd = r'E:\Tesseract\tesseract.exe'  # Update this path to your Tesseract installation

def load_data(path):
    ds = load_from_disk(path)
    return ds

def preprocess_image(example):
    """改进部分：图像增强+OCR文本提示
    目的：通过图像增强提高可读性，并提取关键文本作为提示"""

    idx = int(example["answer_page_idx"])+1
    image = example[f"image_{idx}"]
    
    # 图像增强处理
    # 1. 对比度增强
    enhancer = ImageEnhance.Contrast(image)
    image = enhancer.enhance(1.5)
    # 2. 锐化
    image = image.filter(ImageFilter.SHARPEN)
    # 3. 二值化（适用于文档）
    image = image.convert('L').point(lambda x: 0 if x < 128 else 255, '1')
    
    return image

def generate_answer(example):
    # 预处理图像（使用增强后的图像）
    image = preprocess_image(example)
    
    # 提取OCR文本作为额外上下文
    ocr_text = pytesseract.image_to_string(image)
    # 只保留前200个字符避免过长
    ocr_context = ocr_text[:200] + "..." if len(ocr_text) > 200 else ocr_text

    # 转换为base64
    tmp_dir = "./tmp"
    os.makedirs(tmp_dir, exist_ok=True)
    tmp_file = os.path.join(tmp_dir, "tmp_image.png")
    image.save(tmp_file, format="PNG")

    with open(tmp_file, "rb") as f:
        encoded_image = base64.b64encode(f.read())
    encoded_image_text = encoded_image.decode("utf-8")
    base64_image = f"data:image;base64,{encoded_image_text}"

    # 改进提示词：加入OCR提取的文本作为上下文
    text = (f"Document Context (OCR extracted): {ocr_context}\n"
            f"Question: {example['question']}\n"
            "Based on both the image and the OCR context, provide the most accurate answer.\n"
            "Only return the answer, no other words.")

    chat_response = client.chat.completions.create(
        model="Qwen/Qwen2.5-VL-3B-Instruct",
        messages=[
            {"role": "system", "content": "You are a helpful assistant skilled in document understanding."},
            {
                "role": "user",
                "content": [
                    {
                        "type": "image_url",
                        "image_url": {
                            "url": base64_image
                        },
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
        for example in tqdm(ds, desc="Generating answers", total=len(ds), leave=True, position=0):
            answer = generate_answer(example)
            results.append({"generation": answer, "answers": example['answers']})

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
    parser.add_argument("--output_path", type=str, default="./results/enhanced.json")
    parser.add_argument("--eval_only", action="store_true")
    args = parser.parse_args()
    main(args)