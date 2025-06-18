import matplotlib.pyplot as plt
import numpy as np
import os

class TrajectoryRecorder:
    def __init__(self):
        self.own_positions = []
        self.enemy_positions = []

    def record(self, my_state, enemy_state):
        own_pos = [my_state[0], my_state[1], my_state[2]]
        enemy_pos = [enemy_state[0], enemy_state[1], enemy_state[2]]

        # 添加到轨迹记录
        self.own_positions.append(own_pos)
        self.enemy_positions.append(enemy_pos)


    def reset(self):
        self.own_positions = []
        self.enemy_positions = []

    def plot(self, save_path=None, show=True):
        if not self.own_positions or not self.enemy_positions:
            print("No trajectory to plot.")
            return

        own_pos = np.array(self.own_positions)
        enemy_pos = np.array(self.enemy_positions)

        fig = plt.figure(figsize=(10, 8))
        ax = fig.add_subplot(111, projection='3d')

        # 己方轨迹
        ax.plot(own_pos[:, 0], own_pos[:, 1], own_pos[:, 2], label='Own Aircraft', linewidth=2)
        ax.scatter(own_pos[0, 0], own_pos[0, 1], own_pos[0, 2], color='green', label='Start (Own)')
        ax.scatter(own_pos[-1, 0], own_pos[-1, 1], own_pos[-1, 2], color='blue', label='End (Own)')

        # 敌方轨迹
        ax.plot(enemy_pos[:, 0], enemy_pos[:, 1], enemy_pos[:, 2], label='Enemy Aircraft', linestyle='--', linewidth=2)
        ax.scatter(enemy_pos[0, 0], enemy_pos[0, 1], enemy_pos[0, 2], color='orange', label='Start (Enemy)')
        ax.scatter(enemy_pos[-1, 0], enemy_pos[-1, 1], enemy_pos[-1, 2], color='red', label='End (Enemy)')

        ax.set_title("Trajectory of Aircrafts")
        ax.set_xlabel("X")
        ax.set_ylabel("Y")
        ax.set_zlabel("Z")
        ax.legend()

        if save_path:
            os.makedirs(os.path.dirname(save_path), exist_ok=True)
            plt.savefig(save_path)
            print(f"Trajectory saved to: {save_path}")
        if show:
            plt.show()

        plt.close()
