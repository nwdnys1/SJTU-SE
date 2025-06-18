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