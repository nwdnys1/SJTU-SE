previousRngState = rng(0,"twister");

env = rlPredefinedEnv("BasicGridWorld");

env.ResetFcn = @() 4;

qTable = rlTable(getObservationInfo(env), ...
    getActionInfo(env));

qFcnAppx = rlQValueFunction(qTable, ...
    getObservationInfo(env), ...
    getActionInfo(env));

trainOpts = rlTrainingOptions;
trainOpts.MaxStepsPerEpisode = 50;
trainOpts.MaxEpisodes= 200;
trainOpts.StopTrainingCriteria = "AverageReward";
trainOpts.StopTrainingValue = 9;
trainOpts.ScoreAveragingWindowLength = 30;

sarsaAgent = rlSARSAAgent(qFcnAppx);
sarsaAgent.AgentOptions.EpsilonGreedyExploration.Epsilon = .45;
sarsaAgent.AgentOptions.CriticOptimizerOptions.LearnRate = 2.5;

sarsaTrainingStats = train(sarsaAgent,env,trainOpts);

rng(0,"twister");

plot(env)
env.Model.Viewer.ShowTrace = true;
env.Model.Viewer.clearTrace;

sim(sarsaAgent,env)

rng(previousRngState);