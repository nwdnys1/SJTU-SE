function [reward] = get_my_reward(obs, pre_obs,mytime)
    % 计算奖励值，输入为当前轮的obs和上一轮的obs
   
    % 血量reward
    reward_e = 200 * (pre_obs(26) - obs(26));
    reward_m =  -40 * (pre_obs(13) - obs(13));
    % 距离reward
    pre_place = zeros(1,3);
    cur_place = zeros(1,3);
    pre_place = pre_obs(1:3) - pre_obs (14:16);
    cur_place = obs(1:3) - obs(14:16);
    dis_pre = sqrt(pre_place(1)^2 + pre_place(2)^2 + pre_place(3)^2);
    dis_cur = sqrt(cur_place(1)^2 + cur_place(2)^2 + cur_place(3)^2);
    reward_d = 3*(dis_pre-dis_cur);
    % 距离变大有惩罚
    if reward_d < 0 
        reward_d = 2*reward_d;
    end
    % 距离太近有惩罚
    if (reward_d > 0 && reward_d < 30)
        reward_d = -1 * reward_d;
    end
    % 角度reward

    reward = reward_m + reward_e + reward_d;
end