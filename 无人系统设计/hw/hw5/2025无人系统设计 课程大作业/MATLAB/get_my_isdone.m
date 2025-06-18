function [isdone] = get_my_isdone(org_obs)
    % 自定义训练中止条件，输入为原始战场数据，同get_my_obs.m的输入
    % isdone为1的时候，当前轮训练结束
    % 回忆gridworld，watertank，bipedrobot，都只在"无法继续行动"或"超过最大训练时间"时停止训练
    
    isdone = 0.0;
    tmp = zero(1,3);
    tmp(1:3) = org_obs(1:3) - org_obs (14:16);
    dist = norm(tmp); 
    if(org_obs(3) < -400 || org_obs(3) > 400 ||dist > 300)
        isdone = 1.0;
    end
    
end