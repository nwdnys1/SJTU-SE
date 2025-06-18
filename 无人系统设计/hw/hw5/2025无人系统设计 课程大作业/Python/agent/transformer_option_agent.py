"""
利用stable_baselines3中的PPO、TD3和SAC算法进行改进，使之能够使用transformer进行训练
"""
import torch
import torch.nn as nn
from stable_baselines3.common.torch_layers import BaseFeaturesExtractor
from stable_baselines3.common.policies import ActorCriticPolicy
from stable_baselines3.td3.policies import TD3Policy
from stable_baselines3.sac.policies import SACPolicy


class TransformerFeatureExtractor(BaseFeaturesExtractor):
    def __init__(self, observation_space, features_dim=128, nhead=4, num_layers=2):
        super().__init__(observation_space, features_dim)
        obs_dim = observation_space.shape[0]

        self.input_fc = nn.Linear(obs_dim, features_dim)
        encoder_layer = nn.TransformerEncoderLayer(d_model=features_dim, nhead=nhead)
        self.transformer = nn.TransformerEncoder(encoder_layer, num_layers=num_layers)

    def forward(self, observations):
        x = self.input_fc(observations).unsqueeze(0)  # [1, batch, d_model]
        x = self.transformer(x).squeeze(0)            # [batch, d_model]
        return x


class CustomTransformerPolicy(ActorCriticPolicy):
    def __init__(self, *args, **kwargs):
        super().__init__(*args,
                         features_extractor_class=TransformerFeatureExtractor,
                         **kwargs)


class CustomTransformerTD3Policy(TD3Policy):
    def __init__(self, *args, **kwargs):
        super().__init__(*args,
                         features_extractor_class=TransformerFeatureExtractor,
                         **kwargs)

class CustomTransformerSACPolicy(SACPolicy):
    def __init__(self, *args, **kwargs):
        super().__init__(*args,
                       features_extractor_class=TransformerFeatureExtractor,
                       **kwargs)