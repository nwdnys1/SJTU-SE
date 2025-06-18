import numpy as np
from sklearn.preprocessing import MinMaxScaler

# def norm(x, feature_range=(-1, 1)):
#     """将x归一化到feature_range的范围（默认[-1,1]）"""
#     scaler = MinMaxScaler(feature_range=feature_range)
#     return scaler.fit_transform(x.reshape(-1, 1)).flatten()

def norm(x, min_val=-100, max_val=100):
    """将x归一化到[-1, 1]范围"""
    x = np.clip(x, min_val, max_val)  # 限制在min_val和max_val之间
    return 2 * (x - min_val) / (max_val - min_val) - 1  # 归一化到[-1, 1]范围

# This is the observation processing function. Remember to modify the declarations in trainenv.py correspondingly.
def marshal_observation(my_state, enemy_state):
    # 直接拼接
    agent_state = np.concatenate([my_state, enemy_state])
    #print(f"Agent state before normalization:\n {agent_state}")

    # 归一化，将my_state和enemy_state的前三维归一化到[-1,1]范围，具体min和max分别为-100和100
    my_state[0:3] = norm(my_state[0:3])
    enemy_state[0:3] = norm(enemy_state[0:3])

    # 转换为float64
    agent_state = np.concatenate([my_state, enemy_state])
    agent_state = agent_state.astype(np.float64)
    #print(f"Agent state after normalization:\n {agent_state}")
    
    # 返回
    return agent_state
