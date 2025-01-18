package org.example.backend.service.Impl;

import org.example.backend.service.TimerService;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Service;

import java.time.Instant;

@Service
@Scope("session")//每个会话一个实例
public class TimerServiceImpl implements TimerService {
    private Instant start;
    private Instant end;
    public void start() {
        start = Instant.now();
    }
    public void end() {
        end = Instant.now();
    }
    public long getTime() {
        return end.getEpochSecond()-start.getEpochSecond();//返回时间差，单位秒
    }


}
