function [obs] = get_my_obs(org_obs)
    % org_obs(1-13)第一架飞机的参数org_obs(14-26)第二架飞机参数
    % org_obs(1-3)x\y\z坐标 （单位为10m）
    % org_obs(4-6)欧拉角𝜑\𝜃\𝜓对应翻滚(roll)、俯仰(pitch)、偏航(yaw) ）（单位是rad）
    % org_obs(7-9)线速度u\v\w （单位是m/s）
    % org_obs(10-12)角速度ω\β\η （单位是rad/s）
    % org_obs(13)血量
    % TODO:将高维的原始观察数据降维，并根据降维后的观察数据设计reward
    % 注：matlab的数组下标从1开始

    obs = zeros(1,27);
    obs = org_obs;
end