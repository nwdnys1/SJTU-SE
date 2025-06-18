function [initial] = get_my_initial()
    % 此函数在第一个episode传递给服务器，用于定义本次训练中两机的初始运动信息。
    initial = zeros(1, 2, "int32");
    
    room_id = 0;
    unit_id = 0;

    initial(1) = room_id;
    initial(2) = unit_id;

    others = zeros(1, 2, "int32");

    state = 1;
    time_step = 1;

    others(1) = state;
    others(2) = time_step;

    % TODO:修改初始值，使靶机在以战机为原点的直角坐标系中（1000，0，0）位置，并进行训练
    % 战机初始值
    initial1 = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0 ,0];
    % 靶机初始值
    initial2 = [1000, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0 ,0];
 
    padding = zeros(1, 72, "int32");

    % 通信协议如下
    % initial(1)为房间id，本地训练时不会用到，联机训练时会使用
    % initial(2)为控制id，本地训练时不会用到，联机训练时会使用
    % initial(3-5)为初始战机的XYZ坐标，单位是10m
    % initial(6-8)为初始战机的UVW，单位是rad
    % initial(9-11)为初始战机XYZ方向的速度，单位是m/s
    % initial(12-14)为初始战机UVW方向的速度，单位是rad/s
    % initial(15-26)为靶机的相应初始值，具体内容和战机初始值一致
    % initial(27)为状态值，本地训练时为1，联机训练时为2
    % initial(28)为同步时间步长，默认设置为1
    % initial(29-100)为预留值，目前不需要处理
    
    initial = [initial, initial1, initial2, others, padding];
end