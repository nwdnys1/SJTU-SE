import numpy as np


def generate_initial_state():
    my_initial_state = np.zeros(12)
    enemy_initial_state = np.zeros(12)
    enemy_initial_state[0] = 100 # x position
    initial_state = np.append(my_initial_state, enemy_initial_state)
    return initial_state
