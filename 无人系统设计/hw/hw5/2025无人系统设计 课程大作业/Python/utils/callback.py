"""
修改gym的callback函数，实现训练过程中保存模型和日志
"""
from stable_baselines3.common.callbacks import BaseCallback
import os
import pandas as pd

class TrainLoggerCallback(BaseCallback):
    def __init__(self, save_freq=10, save_path="./logs/single_training/", verbose=1):
        super().__init__(verbose)
        self.save_freq = save_freq
        self.save_path = save_path
        self.rewards = []
        self.timesteps = []
        self.episode_reward = 0
        self.episode_idx = 0


    def _on_step(self) -> bool:
        # 解包 Monitor/Venv 包裹
        env = self.training_env.envs[0]
        while hasattr(env, "env"):
            env = env.env

        # 累计当前 step 的 reward
        reward = self.locals["rewards"][0]
        self.episode_reward += reward

        # 监测 episode 是否结束
        done = self.locals["dones"][0]
        if done:
            self.episode_idx += 1
            self.rewards.append(self.episode_reward)
            self.timesteps.append(self.num_timesteps)

            if self.verbose > 0:
                print(f"[Episode {self.episode_idx}] Reward: {self.episode_reward:.2f} | Step: {self.num_timesteps}")

            self.episode_reward = 0

            # 保存日志
            if self.episode_idx % self.save_freq == 0:
                self._save()

        return True

    def _on_training_end(self) -> None:
        self._save()

    def _save(self):
        df = pd.DataFrame({
            "timesteps": self.timesteps,
            "rewards": self.rewards
        })
        csv_path = os.path.join(self.save_path, "training_log.csv")
        df.to_csv(csv_path, index=False)
        if self.verbose > 0:
            print(f"[Logger] Saved training log to {csv_path}")