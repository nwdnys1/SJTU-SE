from agent.transformer_option_agent import (
    CustomTransformerPolicy,
    CustomTransformerTD3Policy,
    CustomTransformerSACPolicy
)
import yaml
from stable_baselines3 import PPO, TD3, SAC
from stable_baselines3.ppo import MlpPolicy as PPOMlpPolicy
from stable_baselines3.td3 import MlpPolicy as TD3MlpPolicy
from stable_baselines3.sac import MlpPolicy as SACMlpPolicy


def create_model(config_path, env):
    with open(config_path, 'r') as f:
        config = yaml.safe_load(f)
    agent_cfg = config['agent']
    train_cfg = config['training']
    algorithm = train_cfg['algorithm'].lower()
    policy_kwargs = {}

    # 策略类映射表
    policy_map = {
        'ppo': (PPO, {'transformer': CustomTransformerPolicy, 'mlp': PPOMlpPolicy}),
        'td3': (TD3, {'transformer': CustomTransformerTD3Policy, 'mlp': TD3MlpPolicy}),
        'sac': (SAC, {'transformer': CustomTransformerSACPolicy, 'mlp': SACMlpPolicy})
    }

    if algorithm not in policy_map:
        raise ValueError(f"Unsupported algorithm: {algorithm}, choose from {list(policy_map.keys())}")

    model_class, policy_dict = policy_map[algorithm]
    policy_type = agent_cfg.get('policy_type', 'mlp')  # 默认为mlp

    # 获取策略类
    policy = policy_dict.get(policy_type)
    if not policy:
        raise ValueError(f"Unsupported policy type: {policy_type} for {algorithm}")

    # 配置策略参数
    if policy_type == 'transformer':
        policy_kwargs = {
            'features_extractor_kwargs': {
                'features_dim': agent_cfg['features_dim'],
                'nhead': agent_cfg['nhead'],
                'num_layers': agent_cfg['num_layers']
            }
        }
        # SAC需要特别处理
        if algorithm == 'sac':
            policy_kwargs.update({
                'net_arch': {
                    'pi': agent_cfg.get('pi_arch', [256, 256]),
                    'qf': agent_cfg.get('qf_arch', [256, 256])
                }
            })
    else:
        # 配置MLP网络结构
        arch_mapping = {
            'ppo': {'pi': 'pi', 'vf': 'vf'},
            'td3': {'pi': 'pi', 'qf': 'qf'},
            'sac': {'pi': 'pi', 'qf': 'qf'}
        }
        policy_kwargs = {
            'net_arch': {
                key: agent_cfg.get(f"{val}_arch", [64, 64])
                for key, val in arch_mapping[algorithm].items()
            }
        }

    # SAC需要额外的参数
    sac_specific = {}
    if algorithm == 'sac':
        sac_specific = {
            'tau': agent_cfg.get('tau', 0.005),
            'ent_coef': agent_cfg.get('ent_coef', 'auto'),
            'target_entropy': agent_cfg.get('target_entropy', 'auto')
        }

    model = model_class(
        policy,
        env,
        verbose=1,
        learning_rate=train_cfg['learning_rate'],
        gamma=train_cfg['gamma'],
        policy_kwargs=policy_kwargs,
        device=train_cfg['device'],
        **sac_specific  # 添加SAC专用参数
    )
    return model

def load_model(config_path, env, model_path):
    """
    加载训练好的模型，复用 create_model 的策略配置逻辑。
    
    Args:
        config_path (str): 算法配置文件路径（algs.yaml）
        env (gym.Env): 环境实例
        model_path (str): 模型文件路径（.zip）
    
    Returns:
        model: 加载的模型实例
    """
    with open(config_path, 'r') as f:
        config = yaml.safe_load(f)
    train_cfg = config['training']
    algorithm = train_cfg['algorithm'].lower()

    # 算法到模型类的映射
    model_map = {
        'ppo': PPO,
        'td3': TD3,
        'sac': SAC
    }
    
    if algorithm not in model_map:
        raise ValueError(f"Unsupported algorithm: {algorithm}")

    # 直接加载模型（Stable-Baselines3 的 load 方法）
    model = model_map[algorithm].load(
        model_path,
        env=env,
        device=train_cfg['device']
    )
    return model