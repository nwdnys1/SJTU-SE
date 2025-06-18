from openai import OpenAI
from datasets import load_from_disk
import json
import argparse
import base64
from tqdm import tqdm
import os
from PIL import Image, ImageEnhance, ImageFilter  # 新增图像处理库

client = OpenAI(
    base_url="http://47.242.151.133:24576/v1/",
    api_key="ml2025",
)


def load_data(path):
    ds = load_from_disk(path)
    return ds


def preprocess_image(example):
    # ==================== 图像预处理优化 ====================
    # 优化策略：
    # 1. 对比度增强（针对扫描文档）
    # 2. 锐化处理（提高文字清晰度）
    # 3. 二值化处理（减少颜色干扰）
    image = example["image"].convert("RGB")

    # 对比度增强
    enhancer = ImageEnhance.Contrast(image)
    image = enhancer.enhance(1.5)  # 增强50%对比度

    # 锐化处理
    image = image.filter(ImageFilter.SHARPEN)

    # 转换为灰度图（简化颜色信息）
    image = image.convert("L")

    # 二值化处理（可选，根据文档质量）
    # image = image.point(lambda x: 0 if x < 200 else 255, '1')

    return image
    # ==================== END 图像预处理 ====================


def generate_answer(example):
    image = preprocess_image(example)  # 使用优化后的预处理

    # 图像保存部分保持不变
    tmp_dir = "./tmp"
    os.makedirs(tmp_dir, exist_ok=True)
    tmp_file = os.path.join(tmp_dir, "tmp_image.png")
    image.save(tmp_file, format="PNG")

    with open(tmp_file, "rb") as f:
        encoded_image = base64.b64encode(f.read())
    encoded_image_text = encoded_image.decode("utf-8")
    base64_image = f"data:image;base64,{encoded_image_text}"

    # 使用原始prompt或可结合策略1的优化prompt
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
    parser.add_argument("--output_path", type=str, default="./results/preprocess.json")
    parser.add_argument(
        "--error_dir", type=str, default="./errors/preprocess"
    )  # New argument
    parser.add_argument("--eval_only", action="store_true")
    args = parser.parse_args()

    main(args)
