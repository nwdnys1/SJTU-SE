import numpy as np
from sklearn.preprocessing import MinMaxScaler


def norm(x, feature_range=(-1, 1)):
    """将x归一化到feature_range的范围（默认[-1,1]）"""
    scaler = MinMaxScaler(feature_range=feature_range)
    return scaler.fit_transform(x.reshape(-1, 1)).flatten()


def marshal_action(action):
    # action[1:4] = action[1:4].clip(-0.1, 0.1)
    return action.astype(np.float64)
