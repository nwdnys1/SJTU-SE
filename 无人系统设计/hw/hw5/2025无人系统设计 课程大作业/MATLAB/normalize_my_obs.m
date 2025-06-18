function [norm_obs] = normalize_my_obs(obs)
    % 输入为get_my_obs的输出
    % 将observation进行归一化处理
    % 从而保证参数更新以及激活函数的有效性
    

    norm_obs = zeros(1, 27); 
    norm_obs(1) = normalization(obs(1), -1000, 1000);
    norm_obs(2) = normalization(obs(2), -1000, 1000);
    norm_obs(3) = normalization(obs(3), -1000, 1000);
    norm_obs(4) = normalization(obs(4), -1000, 1000);
    norm_obs(5) = normalization(obs(5), -1000, 1000);
    norm_obs(6) = normalization(obs(6), -1000, 1000);
    norm_obs(7) = normalization(obs(7), -1000, 1000);
    norm_obs(8) = normalization(obs(8), -1000, 1000);
    norm_obs(9) = normalization(obs(9), -1000, 1000);
    norm_obs(10) = normalization(obs(10), -1000, 1000);
    norm_obs(11) = normalization(obs(11), -1000, 1000);
    norm_obs(12) = normalization(obs(12), -1000, 1000);
    norm_obs(13) = normalization(obs(13), -1000, 1000);
    norm_obs(14) = normalization(obs(14), -1000, 1000);
    norm_obs(15) = normalization(obs(15), -1000, 1000);
    norm_obs(16) = normalization(obs(16), -1000, 1000);
    norm_obs(17) = normalization(obs(17), -1000, 1000);
    norm_obs(18) = normalization(obs(18), -1000, 1000);
    norm_obs(19) = normalization(obs(19), -1000, 1000);
    norm_obs(20) = normalization(obs(20), -1000, 1000);
    norm_obs(21) = normalization(obs(21), -1000, 1000);
    norm_obs(22) = normalization(obs(22), -1000, 1000);
    norm_obs(23) = normalization(obs(23), -1000, 1000);
    norm_obs(24) = normalization(obs(24), -1000, 1000);
    norm_obs(25) = normalization(obs(25), -1000, 1000);
    norm_obs(26) = normalization(obs(26), -1000, 1000);
    norm_obs(27) = normalization(obs(27), -1000, 1000);
end
