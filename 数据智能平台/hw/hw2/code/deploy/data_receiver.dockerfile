FROM python:3.12.7-slim

WORKDIR /app

COPY ./requirements.txt /app/requirements.txt
RUN pip config set global.index-url https://mirrors.tuna.tsinghua.edu.cn/pypi/web/simple
RUN --mount=type=cache,target=/root/.cache/pip \
    pip install --upgrade -r requirements.txt

COPY data_receiver.py data_receiver.py

CMD ["python", "data_receiver.py"]
