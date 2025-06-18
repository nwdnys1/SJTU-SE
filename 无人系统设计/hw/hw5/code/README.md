## 细节

1. 战机要在 500 米内（即数值中设定的 50）、且机头对准敌机的瞄准角度在一定范围内才能对敌机进行攻击、造成血量的伤害

2. action 的数组中、四个元素分别对应、

   - 油门的变化量（0 到 1）：这里理论上应该是 0 到 1 的范围、测试时设置为负数的时候、飞机会不动（也许是减速了？）
   - 俯仰角度的变化量（-1 到 1）：正为机头向上，负为机头向下
   - 翻滚角度的变化量（-1 到 1）
   - 偏航角度的变化量（-1 到 1），正为顺时针，负为逆时针
   - 上面这几个值设置为超过 1 的时候、也不会有什么影响

3. nohup python3 main.py > output.log 2>&1 &

   - ps aux | grep main.py
   - kill -9 12345

# Train_Single

This is the code template for students training agent using python. Beta test.

transformer_option_agent.py:

- The agent is trained using the transformer option agent.
- Support SAC, PPO, TD3 algorithms.

algs.yaml:

- configure algorithm parameters

envs.yaml:

- configure environment parameters

custom_model.py:

- define custom model

callback.py:

- define callback functions to save the reward
- More information can be found on the use of stable baselines3

visualizer.py:

- visualize the flight path

Dependencies:

- anaconda
- python 3.9
- pytorch 2.2.1 with cuda 12.1: conda install pytorch==2.2.1 torchvision==0.17.1 torchaudio==2.2.1 pytorch-cuda=12.1 -c pytorch -c nvidia
- stable baselines 3: pip install stable-baselines3[extra]
- gymnasium: pip install gymnasium
