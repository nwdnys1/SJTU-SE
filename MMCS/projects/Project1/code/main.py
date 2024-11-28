import cvxpy as cp
import numpy as np
import matplotlib.pyplot as plt


# 生成合成图像
def create_image(size, shape="circle", radius=None, side_length=None):
    image = np.ones((size, size)) * 0.99
    center = size // 2
    Y, X = np.ogrid[:size, :size]

    if shape == "circle":
        if radius is None:
            radius = size // 4
        dist_from_center = np.sqrt((X - center) ** 2 + (Y - center) ** 2)
        mask = dist_from_center <= radius
    elif shape == "square":
        if side_length is None:
            side_length = size // 2
        mask = (abs(X - center) <= side_length // 2) & (
            abs(Y - center) <= side_length // 2
        )
    elif shape == "triangle":
        if side_length is None:
            side_length = size // 2
        top_y = center - int(np.sqrt(3) / 6 * side_length)
        left_x = center - side_length // 2
        right_x = center + side_length // 2
        mask = (
            (Y >= top_y)
            & (X >= left_x)
            & (X <= right_x)
            & (Y <= (top_y + (X - left_x) * np.sqrt(3)))
        )
    else:
        raise ValueError(
            "Unsupported shape. Supported shapes are 'circle', 'square', and 'triangle'."
        )

    image[mask] = 0.01
    return image


# 构建优化问题
def solve_optimization_problem(original_image):
    # 定义分割变量
    segmented_image = cp.Variable(original_image.shape, boolean=True)

    # 定义目标函数
    obj_1 = cp.sum(cp.square(cp.multiply(segmented_image, original_image)))
    obj_2 = cp.sum(cp.square(cp.multiply((1 - segmented_image), (1 - original_image))))
    objective = cp.Minimize(obj_1 + obj_2)

    # 定义约束条件
    constraints = []

    # 求解优化问题
    problem = cp.Problem(objective, constraints)
    problem.solve(solver=cp.ECOS_BB)

    return segmented_image.value


# 可视化分割结果
def visualize_segmentation(original_image, segmented_image):

    # 背景图像
    plt.subplot(1, 2, 1)
    plt.imshow(original_image, cmap="gray")
    plt.title("Original Image")
    plt.axis("off")

    # 分割结果
    plt.subplot(1, 2, 2)
    plt.imshow(segmented_image, cmap="gray")
    plt.title("Segmentation Result")
    plt.axis("off")

    plt.show()


if __name__ == "__main__":
    # 生成合成图像
    original_image = create_image(size=100, shape="triangle", side_length=20)

    # 解决优化问题
    segmented_image = solve_optimization_problem(original_image)

    # 可视化分割结果
    visualize_segmentation(original_image, segmented_image)
