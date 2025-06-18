import numpy as np


def check_truncation(my_state, enemy_state):
    """
    检查是否应该终止当前训练回合(episode)
    终止条件包括：
    - 超出边界位置（z/y轴）
    - 与敌机的角度偏差过大（>90度）
    
    Args:
        my_state: 当前飞机状态(13维数组)
        enemy_state: 敌机状态(13维数组)
    
    Returns:
        bool: 是否应该终止
    """
    # 1. 检查位置边界条件
    if (
        my_state[2] < -10  # z轴过低
        or my_state[2] > 10  # z轴过高
        or my_state[1] > 30  # y轴过远
        or my_state[1] < -30  # y轴过远
    ):  
        return True

    # 2. 检查角度偏差条件（新增）
    def calculate_angle_diff(my_state, enemy_state):
        """计算飞机前向向量与敌机方向的夹角（弧度）"""
        # 计算敌机方向向量（世界坐标系）
        enemy_dir = enemy_state[0:3] - my_state[0:3]
        enemy_dir_normalized = enemy_dir / np.linalg.norm(enemy_dir)
        
        # 获取飞机当前姿态角
        pitch = my_state[4]  # 俯仰角
        yaw = my_state[5]    # 偏航角
        
        # 计算飞机前向向量（世界坐标系）
        my_forward = np.array([
            np.cos(pitch) * np.cos(yaw),
            np.cos(pitch) * np.sin(yaw),
            np.sin(pitch)
        ])
        
        # 计算夹角（点积后反余弦）
        dot_product = np.dot(my_forward, enemy_dir_normalized)
        return np.arccos(np.clip(dot_product, -1.0, 1.0))
    
    angle_diff = calculate_angle_diff(my_state, enemy_state)
    MAX_ALLOWED_ANGLE = np.pi/4  # 45度（弧度）
    
    if angle_diff > MAX_ALLOWED_ANGLE:
        return True

    return False