from openai import OpenAI
from datasets import load_from_disk
import json
import argparse
import base64
from tqdm import tqdm
import os
from PIL import Image  # 新增导入

client = OpenAI(
    base_url="http://47.242.151.133:24576/v1/",
    api_key="ml2025",
)

def load_data(path):
    ds = load_from_disk(path)
    return ds


def preprocess_image(example):
    '''
    Preprocess images by stitching relevant pages together with smart resizing
    Args:
        example: dict containing the example data with multiple images
    Returns:
        Image: The processed image (either single or stitched)
    '''
    # Collect all available non-None images (up to 20)
    images = []
    for i in range(1, 21):  # Assuming max 20 images
        img_key = f"image_{i}"
        if img_key in example and example[img_key] is not None:
            images.append(example[img_key])
    
    if not images:
        return None
    
    # If only one image, return it directly
    if len(images) == 1:
        return images[0]
    
    # Get answer page index (1-based)
    answer_idx = int(example.get("answer_page_idx", 0)) + 1
    
    # Determine which pages to include (prioritizing pages around the answer)
    target_indices = set()
    
    # Always include answer page if available
    if 1 <= answer_idx <= len(images):
        target_indices.add(answer_idx - 1)  # Convert to 0-based
    
    # Include adjacent pages (2 before and 2 after answer page)
    for offset in [-2, -1, 1, 2]:
        idx = answer_idx - 1 + offset  # 0-based
        if 0 <= idx < len(images):
            target_indices.add(idx)
    
    # If we didn't find answer page, use first 3 pages as fallback
    if not target_indices:
        target_indices = set(range(min(3, len(images))))
    
    # Convert to sorted list
    target_indices = sorted(target_indices)
    
    # Select up to 5 most relevant pages (to keep size manageable)
    selected_images = [images[i] for i in target_indices[:5]]
    
    # Calculate target height (420px or smaller if many images)
    max_height = 420
    if len(selected_images) > 3:
        max_height = 350  # Slightly smaller when we have more images
    
    # Resize all images to same height while maintaining aspect ratio
    resized_images = []
    for img in selected_images:
        original_width, original_height = img.size
        if original_height == 0:  # prevent division by zero
            continue
        
        # Calculate new dimensions
        new_height = min(max_height, original_height)
        new_width = int(original_width * (new_height / original_height))
        
        # Resize using high-quality downsampling
        resized_img = img.resize((new_width, new_height), Image.LANCZOS)
        resized_images.append(resized_img)
    
    # Calculate total width and max height after resizing
    total_width = sum(img.width for img in resized_images)
    max_resized_height = max(img.height for img in resized_images) if resized_images else 0
    
    # If total width is too large (more than 840px), scale down proportionally
    max_total_width = 840
    if total_width > max_total_width:
        scale_factor = max_total_width / total_width
        scaled_images = []
        for img in resized_images:
            new_width = int(img.width * scale_factor)
            new_height = int(img.height * scale_factor)
            scaled_img = img.resize((new_width, new_height), Image.LANCZOS)
            scaled_images.append(scaled_img)
        resized_images = scaled_images
        total_width = sum(img.width for img in resized_images)
    
    # Create blank canvas for stitching (white background)
    stitched_image = Image.new(
        'RGB', 
        (total_width, max_resized_height), 
        color=(255, 255, 255)
    )
    
    # Paste images side by side
    x_offset = 0
    for img in resized_images:
        stitched_image.paste(img, (x_offset, 0))
        x_offset += img.width
    
    return stitched_image

def generate_answer(example):
    # 预处理图像（现在使用拼接后的图像）
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
    parser.add_argument("--output_path", type=str, default="./results/preprocess.json")
    parser.add_argument("--eval_only", action="store_true")
    args = parser.parse_args()
    main(args)