import os
from envs.train_env import TrainEnv
from stable_baselines3.common.monitor import Monitor
from stable_baselines3.common.vec_env import DummyVecEnv
from utils import custom_model

from stable_baselines3.common.vec_env import DummyVecEnv

def demo():
    # 加载环境并包装为VecEnv
    base_env = TrainEnv(config_path='./config/envs.yaml')
    env = DummyVecEnv([lambda: Monitor(base_env)])  # 使用DummyVecEnv包装
    
    # 加载训练好的模型
    model_path = "./model/model_300000_steps.zip"
    model = custom_model.load_model(config_path='./config/algs.yaml', env=env, model_path=model_path)
    
    # 演示模型
    obs = env.reset()
    for _ in range(1000):
        action, _states = model.predict(obs, deterministic=True)
        obs, rewards, dones, infos = env.step(action)
        
        if dones[0]:  # 检查是否结束
            obs = env.reset()
    
    # 关闭环境
    env.close()

if __name__ == "__main__":
    demo()