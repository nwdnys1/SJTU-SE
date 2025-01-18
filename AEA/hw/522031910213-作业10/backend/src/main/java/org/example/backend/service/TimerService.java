package org.example.backend.service;

public interface TimerService {
    void start();//开始计时
    void end();//结束计时
    long getTime();//返回时间差，单位秒
}
