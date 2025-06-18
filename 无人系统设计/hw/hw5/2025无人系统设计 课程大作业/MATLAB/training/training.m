%最大轮数maxepisodes
maxepisodes = 5000;
%每个episode最长持续时间maxsteps
maxsteps = ceil(Tf/Ts);
%当智能体在m个连续情节中获得的平均累积奖励大于n时，停止训练
m = 20;
n = 1000;

% Verbose参数用于指定是否在控制台输出训练过程的详细信息
% 当Verbose为true时，训练过程中将输出包括每个训练迭代的平均回报和损失等详细信息
% 'SaveAgentCriteria'该参数可以保存训练过程中的agent
trainOpts = rlTrainingOptions(...
    'MaxEpisodes',maxepisodes, ...h
    'MaxStepsPerEpisode',maxsteps, ... 
    'ScoreAveragingWindowLength',m, ...
    'Verbose',false, ...
    'Plots','training-progress',...
    'SaveAgentCriteria', 'AverageReward',...
    'SaveAgentValue', 100,...
    'StopTrainingCriteria','AverageReward',...
    'StopTrainingValue',n);

doTraining = true;

if doTraining
    % Train the agent.
    trainingStats = train(agent,env,trainOpts);
    save('save.mat');
else
    % Load the pretrained agent for the example.
    load('save.mat');
    trainingStats = train(agent,env,trainOpts);
    save('save.mat')
end