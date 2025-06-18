previousRngState = rng(0,"twister");

env = rlPredefinedEnv("BasicGridWorld");

env.ResetFcn = @() 5;

qTable = rlTable(getObservationInfo(env), ...
    getActionInfo(env));

qFcnAppx = rlQValueFunction(qTable, ...
    getObservationInfo(env), ...
    getActionInfo(env));

qAgent = rlQAgent(qFcnAppx);

qAgent.AgentOptions.EpsilonGreedyExploration.Epsilon = 0.4;
qAgent.AgentOptions.CriticOptimizerOptions.LearnRate = 2.5;

trainOpts = rlTrainingOptions;
trainOpts.MaxStepsPerEpisode = 50;
trainOpts.MaxEpisodes= 200;
trainOpts.StopTrainingCriteria = "AverageReward";
trainOpts.StopTrainingValue = 8;
trainOpts.ScoreAveragingWindowLength = 30;

rng(0,"twister");

doTraining = true;
if doTraining
    % Train the agent.
    qTrainingStats = train(qAgent,env,trainOpts);
else
    % Load the pretrained agent for the example.
    load("basicGWQAgent.mat","qAgent")
end

rng(0,"twister");

plot(env)
env.Model.Viewer.ShowTrace = true;
env.Model.Viewer.clearTrace;

sim(qAgent,env)
