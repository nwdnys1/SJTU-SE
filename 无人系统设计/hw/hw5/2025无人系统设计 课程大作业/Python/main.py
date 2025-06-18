"""
main函数，用于训练模型并保存
"""
import os
import matplotlib.pyplot as plt
import pandas as pd
from envs.train_env import TrainEnv
from utils import custom_model
from stable_baselines3.common.monitor import Monitor
from stable_baselines3.common.logger import configure
from stable_baselines3.common.callbacks import CheckpointCallback

def main():
    log_dir = "./logs/"
    os.makedirs(log_dir, exist_ok=True)
    logger = configure(log_dir, ["stdout", "csv"])
    checkpoint_callback = CheckpointCallback(
        save_freq=100000,
        save_path="./model/",
        name_prefix="model",
        save_replay_buffer=True,
        save_vecnormalize=True,
    )
    base_env = TrainEnv(config_path='./config/envs.yaml')
    env = Monitor(base_env)

    model = custom_model.create_model(config_path= './config/algs.yaml',env = env)
    model.set_logger(logger)
    model.learn(total_timesteps=1000000,progress_bar=True, reset_num_timesteps=False,log_interval=1,
                callback=checkpoint_callback)

if __name__ == "__main__":
    main()