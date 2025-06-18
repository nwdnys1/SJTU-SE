import numpy as np


def calculate_reward(prev_my_state, prev_enemy_state, my_state, enemy_state):
    """
    改进版奖励函数：
    - 距离变近时给予正向奖励，变远时惩罚
    - 角度偏差变化：减小奖励，增大惩罚
    - 保留原有的血量奖励机制

    State structure (13 elements):
    0-2: x,y,z position (units: 10m)
    3-5: roll, pitch, yaw (radians)
    6-8: linear velocities u,v,w (m/s)
    9-11: angular velocities ω,β,η (rad/s)
    12: health
    """
    # 健康度奖励（保持不变）
    enemy_health_lost = prev_enemy_state[12] - enemy_state[12]
    my_health_lost = prev_my_state[12] - my_state[12]
    if enemy_health_lost ==0:
        enemy_health_lost = 0.01
    if my_health_lost ==0:
        my_health_lost = 0.01
    ENEMY_HEALTH_REWARD = 200
    MY_HEALTH_PENALTY = 40
    reward_health = (
        ENEMY_HEALTH_REWARD * enemy_health_lost - MY_HEALTH_PENALTY * my_health_lost
    )

    # 距离变化奖励（新增：变近奖励，变远惩罚）
    prev_pos_diff = prev_my_state[0:3] - prev_enemy_state[0:3]
    curr_pos_diff = my_state[0:3] - enemy_state[0:3]

    prev_dist = np.linalg.norm(prev_pos_diff)
    curr_dist = np.linalg.norm(curr_pos_diff)
    dist_change = prev_dist - curr_dist  # 正数表示变近

    # 距离奖励参数
    DISTANCE_REWARD_SCALE = 2.0  # 比健康奖励小的权重
    reward_distance = DISTANCE_REWARD_SCALE * dist_change
    # 如果距离过大 给予额外惩罚
    if curr_dist > 30:
        reward_distance -= 2.0
    # 加大惩罚
    if reward_distance < 0:
        reward_distance *= 3.0

    # 角度偏差变化奖励（新增：偏差减小奖励，增大惩罚）
    def calculate_angle_diff(state, enemy_pos):
        """计算当前状态与敌人的角度偏差"""
        enemy_dir = enemy_pos - state[0:3]
        enemy_dir_normalized = enemy_dir / np.linalg.norm(enemy_dir)

        pitch = state[4]
        yaw = state[5]
        my_forward = np.array(
            [np.cos(pitch) * np.cos(yaw), np.cos(pitch) * np.sin(yaw), np.sin(pitch)]
        )

        dot_product = np.dot(my_forward, enemy_dir_normalized)
        return np.arccos(np.clip(dot_product, -1.0, 1.0)) # 计算夹角（弧度）

    prev_angle_diff = calculate_angle_diff(prev_my_state, enemy_state[0:3])
    curr_angle_diff = calculate_angle_diff(my_state, enemy_state[0:3])
    angle_change = prev_angle_diff - curr_angle_diff  # 正数表示偏差减小

    # 角度变化奖励参数
    ANGLE_CHANGE_SCALE = 100.0  # 适中权重
    reward_angle_change = ANGLE_CHANGE_SCALE * angle_change
    
    # 如果角度偏差小于0.1，给予额外奖励
    if curr_angle_diff < 0.1:
        reward_angle_change += 1.0

    # 加大惩罚
    if reward_angle_change < 0:
        reward_angle_change *= 3.0

    # 总奖励
    total_reward = reward_health + reward_distance + reward_angle_change

    # 调试输出
    print(
        f"Rewards - Health: {reward_health:.1f}, "
        f"Distance: {reward_distance:.1f} (Δ: {dist_change:.2f}), "
        f"Angle: {reward_angle_change:.1f} (Δ: {angle_change:.3f} rad) | "
        f"Total: {total_reward:.1f}"
    )

    return total_reward
