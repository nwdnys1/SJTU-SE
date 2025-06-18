import numpy as np
from sklearn.preprocessing import MinMaxScaler

def marshal_action(action):
    # action[1:4] = action[1:4].clip(-0.1, 0.1)
    # 将第一项，油门，等比例缩放到[0, 1]范围
    action[0] = (action[0] + 1) / 2
    # 将后三项限制在[-1, 1]范围内
    action[1:4] = np.clip(action[1:4], -1, 1)
    return action.astype(np.float64)
