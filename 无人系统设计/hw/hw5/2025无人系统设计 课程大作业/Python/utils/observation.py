import numpy as np
from sklearn.preprocessing import MinMaxScaler


def norm(x, feature_range=(-1, 1)):
    """将x归一化到feature_range的范围（默认[-1,1]）"""
    scaler = MinMaxScaler(feature_range=feature_range)
    return scaler.fit_transform(x.reshape(-1, 1)).flatten()


# This is the observation processing function. Remember to modify the declarations in trainenv.py correspondingly.
def marshal_observation(my_state, enemy_state):
    # 直接拼接
    agent_state = np.concatenate([my_state, enemy_state])
    # 归一化
    agent_state = norm(agent_state)
    # 转换为float64
    agent_state = agent_state.astype(np.float64)
    # 返回
    return agent_state
