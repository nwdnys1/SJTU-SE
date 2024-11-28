% 加载数据集并拆分复数输入和输出为实数和虚数部分
load train.mat 
train_input_real = real(paInput);
train_input_imag = imag(paInput);
train_output_real = real(paOutput);
train_output_imag = imag(paOutput);
load test.mat 
test_input_real = real(paInput);
test_input_imag = imag(paInput);
test_output_real = real(paOutput);
test_output_imag = imag(paOutput);

% 计算训练数据集的均值和标准差
train_input_real_mean = mean(train_input_real);
train_input_real_std = std(train_input_real);
train_input_imag_mean = mean(train_input_imag);
train_input_imag_std = std(train_input_imag);
train_output_real_mean = mean(train_output_real);
train_output_real_std = std(train_output_real);
train_output_imag_mean = mean(train_output_imag);
train_output_imag_std = std(train_output_imag);

% 对训练数据集进行归一化
train_input_real = (train_input_real - train_input_real_mean) / train_input_real_std;
train_input_imag = (train_input_imag - train_input_imag_mean) / train_input_imag_std;
train_output_real = (train_output_real - train_output_real_mean) / train_output_real_std;
train_output_imag = (train_output_imag - train_output_imag_mean) / train_output_imag_std;

% 使用训练数据集的均值和标准差对测试数据集进行归一化
test_input_real = (test_input_real - train_input_real_mean) / train_input_real_std;
test_input_imag = (test_input_imag - train_input_imag_mean) / train_input_imag_std;
test_output_real = (test_output_real - train_output_real_mean) / train_output_real_std;
test_output_imag = (test_output_imag - train_output_imag_mean) / train_output_imag_std;

% 定义记忆深度 m
m = 5;

% 构建m维的输入序列和1维的输出序列进行训练
train_input_real_split = [];
train_input_imag_split = [];
train_output_real_split = [];
train_output_imag_split = [];

for i = m+1:length(train_input_real)
    window_input_real = train_input_real(i-m:i-1);
    window_input_imag = train_input_imag(i-m:i-1);
    train_input_real_split = [train_input_real_split; window_input_real];
    train_input_imag_split = [train_input_imag_split; window_input_imag];
    train_output_real_split = [train_output_real_split; train_output_real(i)];
    train_output_imag_split = [train_output_imag_split; train_output_imag(i)];
end

test_input_real_split = [];
test_input_imag_split = [];
test_output_real_split = [];
test_output_imag_split = [];

for i = m+1:length(test_input_real)
    window_input_real = test_input_real(i-m:i-1);
    window_input_imag = test_input_imag(i-m:i-1);
    test_input_real_split = [test_input_real_split; window_input_real];
    test_input_imag_split = [test_input_imag_split; window_input_imag];
    test_output_real_split = [test_output_real_split; test_output_real(i)];
    test_output_imag_split = [test_output_imag_split; test_output_imag(i)];
end

% 定义模型函数为多项式函数（三次）
fun = @(beta, x) beta(1) + sum(beta(2:m+1)' .* x, 2) + sum(beta(m+2:2*m+1)' .* x.^2, 2) + sum(beta(2*m+2:3*m+1)' .* x.^3, 2);

% 初始化参数
beta0 = zeros(3*m + 1, 1);

% 使用非线性回归进行建模
rng default % 设置随机数种子，以确保结果可重复
nlm_regressor_real = fitnlm(train_input_real_split, train_output_real_split,fun, beta0);
nlm_regressor_imag = fitnlm(train_input_imag_split, train_output_imag_split,fun, beta0);

% 训练集预测
train_predicted_real = predict(nlm_regressor_real, train_input_real_split);
train_predicted_imag = predict(nlm_regressor_imag, train_input_imag_split);

% 测试集预测
test_predicted_real = predict(nlm_regressor_real, test_input_real_split);
test_predicted_imag = predict(nlm_regressor_imag, test_input_imag_split);

% 计算 NMSE
train_nmse =log10( (sum((train_predicted_real - train_output_real_split).^2)+sum((train_predicted_imag - train_output_imag_split).^2)) / (sum(train_output_real_split.^2)+sum(train_output_imag_split.^2)));
test_nmse =log10( (sum((test_predicted_real - test_output_real_split).^2)+sum((test_predicted_imag - test_output_imag_split).^2)) / (sum(test_output_real_split.^2)+sum(test_output_imag_split.^2)));

disp(['训练集 NMSE: ', num2str(train_nmse)]);
disp(['测试集 NMSE: ', num2str(test_nmse)]);

% 计算归一化幅度
train_input_amp = abs(train_input_real + 1j * train_input_imag);
train_output_amp = abs(train_output_real + 1j * train_output_imag);
train_predicted_amp = abs(train_predicted_real + 1j * train_predicted_imag);

test_input_amp = abs(test_input_real + 1j * test_input_imag);
test_output_amp = abs(test_output_real + 1j * test_output_imag);
test_predicted_amp = abs(test_predicted_real + 1j * test_predicted_imag);


% 绘制训练集实数部分和归一化幅度曲线
figure;
scatter(test_input_amp, test_output_amp, 'b','bo', 'filled', 'SizeData', 20);
hold on;
scatter(test_input_amp(m+1:end), test_predicted_amp', 'r','bo', 'filled', 'SizeData', 20);
title('测试集归一化幅度','FontName','楷体');
xlabel('输入归一化幅度','FontName','楷体');
ylabel('输出归一化幅度','FontName','楷体');
