## LAB3 - VLM

- docvqa 中包含 4 个代码
  - baseline.py：原始的 baseline
  - method_preprocess.py：将图片进行合适的预处理
  - method_prompt.py：提供合适的 prompt
  - method_context.py：提供合适的 context
- mp_docvqa 中包含 4 个代码
  - baseline.py：原始的 baseline
  - method_concat.py：将图片进行拼接
  - method_ocrrag.py：将图片进行 ocr 后用 rag 检索出最符合的图片
  - method_enhance.py：将图片进行增强
- result 中每个 json 对应不同代码 其中 mp_docvqa 中 baseline 包括 first 和 correct 两个策略
