% 定义observation的数量
% TODO：根据自己的设计来定义numObs的值
numObs = 27;
% 定义action的数量
% TODO：根据自己的设计来定义numAct的值
numAct = 3;
% 定义observation的范围
obsInfo = rlNumericSpec([numObs 1],...
    'LowerLimit',[-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1]',...
    'UpperLimit',[ 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1]');
obsInfo.Name = 'observations';
numObservations = obsInfo.Dimension(1);
% 定义action的范围
actInfo = rlNumericSpec([numAct 1],...
    'LowerLimit',[0 -1 -1]',...
    'UpperLimit',[1  1  1]');
actInfo.Name = 'operation';
numActions = actInfo.Dimension(1);

% 定义env
env = rlSimulinkEnv('rlflight','rlflight/Training/RL Agent',...
    obsInfo,actInfo,'UseFastRestart','off');

env.ResetFcn = @(in) (in);

%智能体采样时间
Ts = 1;
%模拟时间
Tf = 300;

rng(0)

agent = createTD3Agent(numObs,obsInfo,numAct,actInfo,Ts);
