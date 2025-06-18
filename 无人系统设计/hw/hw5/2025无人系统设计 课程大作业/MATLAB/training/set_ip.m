% 设置TCP模块的IP和端口号
% ip为战斗服务器IP，本机训练为'127.0.0.1'
% port为战斗服务器端口号，默认为1000
ip = '10.119.13.38';
port = '1004';
set_param('rlflight/Send Initial Pack/TCP Send Initial Pack','Host',ip);
set_param('rlflight/Send Initial Pack/TCP Send Initial Pack','Port',port);
set_param('rlflight/Training/TCP Send Control Pack','Host',ip);
set_param('rlflight/Training/TCP Send Control Pack','Port',port);
set_param('rlflight/Training/TCP Receive Observation Pack','Host',ip);
set_param('rlflight/Training/TCP Receive Observation Pack','Port',port);
