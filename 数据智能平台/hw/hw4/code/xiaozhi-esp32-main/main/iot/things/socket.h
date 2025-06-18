#ifndef IOT_LAMP_H
#define IOT_LAMP_H

#include "iot/thing.h"
#include <string>
#include <functional> // 用于定时器功能
#include <freertos/FreeRTOS.h>  // FreeRTOS 基础定义
#include <freertos/task.h>      // TaskHandle_t 定义
namespace iot {
#define configCHECK_FOR_STACK_OVERFLOW 2
class Socket : public Thing {
public:
    Socket();

    // 新增的方法
    void TurnOn(const ParameterList& parameters); // 打开灯
    void TurnOff(const ParameterList& parameters); // 关闭灯
    void SendTurnOnCommand(); // 发送开灯命令
    void SendTurnOffCommand(); // 发送关灯命令

private:
    bool power_ = false;            // 灯的状态
    bool turn_on = false; //是开等任务还是关灯任务 
    bool timer_ = false; //定时器的状态
    bool cancel_timer_ = false;     // 用于控制定时器取消
    int timer_seconds_ = 0;         // 定时器的秒数
    int brightness_=0;
    void SetLampState(bool state);  // 内部设置灯并上报状态
    void SendHttpCommand(bool state); // 用HTTP控制灯
    void CancelTimer();
    // 定时器相关方法
    void TimerTask();  // 定时器任务
    void StartTimerTask(int);
    TaskHandle_t timer_task_handle_ = nullptr;  // 正确声明任务句柄
};

} // namespace iot

#endif // IOT_LAMP_H
