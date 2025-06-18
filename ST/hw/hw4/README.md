## HW4

1. 作业采取 4 个 生成式模型推理系统

   - VLLM
   - LM_studio
   - Ollama
   - llama.cpp

2. 使用模型

   - Qwen3-0.6B
   - Qwen3-1.7B

3. 实验环境

   - Windows 11
   - Docker Desktop 4.24.1 (Windows 11)
   - NVIDIA GPU RTX 4070Ti (12GB) 一张
   - CUDA

## 文件说明

```
|-- README.md
|-- connection_test.py  # 测试连接的脚本
|-- formal_test.py  # 最终测试的脚本
|-- results  # 测试结果的文件夹
```

## 环境配置及部署流程

### LM_studio

1. https://lmstudio.ai/ 官网下载安装包

2. 左侧 discover 中、下载 Qwen3-0.6B 和 Qwen3-1.7B 模型（这里下载的是 gguf 格式的、后面的 llama.cpp 会用到）

3. 左侧 developer 中启动 api 服务、修改端口号为 11434、然后点击 start 按钮

### llama.cpp

```shell
# 从docker hub 上拉取 llama.cpp 的 docker 镜像
docker pull ghcr.io/ggml-org/llama.cpp:server-cuda
```

```shell
# 启动docker容器、并挂载本地的模型目录到docker容器中
# 0.6B
docker run -p 11434:11434 -v C:/Users/Horiz/.lmstudio/models/lmstudio-community/Qwen3-0.6B-GGUF:/models --gpus all ghcr.io/ggml-org/llama.cpp:server-cuda -m /models/Qwen3-0.6B-Q8_0.gguf -c 512 --host 0.0.0.0 --port 11434 --n-gpu-layers 99
# 1.7B
docker run -p 11434:11434 -v C:/Users/Horiz/.lmstudio/models/lmstudio-community/Qwen3-1.7B-GGUF:/models --gpus all ghcr.io/ggml-org/llama.cpp:server-cuda -m /models/Qwen3-1.7B-Q6_K.gguf -c 512 --host 0.0.0.0 --port 11434 --n-gpu-layers 99
```

### Ollama

1. https://ollama.com/ 官网下载安装包，设置环境变量 OLLAMA_MODELS 为 D:\Ollama_Models 放在 D 盘、减少 C 盘的空间占用

2. 下载模型

```shell
ollama pull qwen3:0.6b
ollama pull qwen3:1.7b
```

3. 启动模型

```shell
ollama serve
# 另起一个终端、测试模型
ollama run qwen3:0.6b
ollama run qwen3:1.7b
```

4. 注意这里的默认端口是 11434、懒得改默认配置了、所以上面的端口也都设成了 11434、也可以用 docker 部署方便改端口

5. 同时这里需要把 script 文件中的 model 改成 qwen3:0.6b，否则会报错、其他的都可以是 Qwen/Qwen3-0.6B

### VLLM

官网 https://docs.vllm.ai/en/latest/deployment/docker.html#deployment-docker-pre-built-image

也可以在 huggingface 右侧的 Use this model 中找到启动 vllm 的 docker 命令

```shell
# 从docker hub 上拉取 VLLM 的 docker 镜像
docker pull vllm/vllm-openai:latest
```

```shell
# 可以提前下载模型、然后将模型放到指定目录，因为如果在docker中下载模型会连接异常、而且不好管理
# 这里的目录是 D:/Horizon/Projects/Ongoing/ST_HW4/vllm/.cache/huggingface （默认是在C盘的user/.cache/huggingface里、可以下载后复制然后粘贴到指定目录）
# 示例更改前路径：C:\Users\Horiz\.cache\huggingface\hub\models--Qwen--Qwen3-1.7B
# 示例更改后路径：D:\Horizon\Projects\Ongoing\ST_HW4\.cache\huggingface\hub\models--Qwen--Qwen3-1.7B
huggingface-cli download Qwen/Qwen3-0.6B
huggingface-cli download Qwen/Qwen3-1.7B
```

```shell
# 启动docker容器、并挂载本地的huggingface缓存目录到docker容器中
docker run --runtime nvidia --gpus all `
    -v D:/Horizon/Projects/Ongoing/ST_HW4/vllm/.cache/huggingface:/root/.cache/huggingface ` # 这里是本地的huggingface缓存目录、可以根据需要修改
    --env "HUGGING_FACE_HUB_TOKEN=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" ` # 这里是huggingface的token、可以在huggingface官网上申请、然后替换掉
    -p 11434:11434 `
    --ipc=host `
    vllm/vllm-openai:latest `
    --model Qwen/Qwen3-0.6B # 这里是模型的名称、可以根据需要修改、也可以修改成自己的模型目录、指定模型名称就会在huggingface的缓存目录中查找模型

docker run --runtime nvidia --gpus all `
    -v D:/Horizon/Projects/Ongoing/ST_HW4/.cache/huggingface:/root/.cache/huggingface `
    --env "HUGGING_FACE_HUB_TOKEN=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" `
    -p 11434:8000 `
    --ipc=host `
    vllm/vllm-openai:latest `
    --model Qwen/Qwen3-0.6B

docker run --runtime nvidia --gpus all `
    -v D:/Horizon/Projects/Ongoing/ST_HW4/.cache/huggingface:/root/.cache/huggingface `
    --env "HUGGING_FACE_HUB_TOKEN=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" `
    -p 11434:8000 `
    --ipc=host `
    vllm/vllm-openai:latest `
    --model Qwen/Qwen3-1.7B
```

## 实验设置

1. 结合并发数、输入序列长度、生成序列长度等参数设计多个测试场景，并实现基于 openai api 的测试脚本。

2. 在 prompt 最后加入/no_think、关闭思考模式进行测试

3. 每个场景都共计进行 40 次 request

4. 每轮这个 prompt 生成完之后、time.sleep(5)s、避免过快的请求影响测试

### LLM 性能测试场景设计

| 场景名称       | 并发数 | 输入长度 (tokens) | 最大生成长度 (tokens) | 测试目的                       |
| -------------- | ------ | ----------------- | --------------------- | ------------------------------ |
| **短文本问答** | 5      | 5-10              | 20                    | 测试高并发下短文本处理的吞吐量 |
| **长文问答**   | 3      | 50-100            | 100                   | 典型问答场景性能基准           |
| **长文生成**   | 1      | 40-50             | 300                   | 测试长文本生成稳定性           |
| **高并发压力** | 10     | 30-50             | 50                    | 极限并发下的错误处理能力       |
| **混合负载**   | 4      | 10-200            | 200                   | 模拟真实场景的随机负载         |
