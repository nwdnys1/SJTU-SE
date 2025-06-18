#include "iot/things/socket.h"
#include "application.h"
#include <esp_log.h>
#include <esp_http_client.h>
#include "iot/thing.h"
#define TAG "Socket"

static char *entity_id = "switch.cuco_cn_941380850_v3_on_p_2_1";
static char *token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJkM2NlM2RkZDU2ZWI0ZjkyOTA0NDVjYmZmZGM4NGFmNyIsImlhdCI6MTc0ODM1MDg1NiwiZXhwIjoyMDYzNzEwODU2fQ.TQedJyKSkmxXNTOt_pdHqCIQHMTZBUi_gaW4TqN41_Q";

namespace iot
{

    Socket::Socket() : Thing("Socket", "这是一个插座，可以控制开关")
    {
        properties_.AddBooleanProperty("power", "插座是否打开", [this]() -> bool
                                       { return power_; });
        properties_.AddBooleanProperty("timer", "定时器是否启动", [this]() -> bool
                                       { return timer_; });
        methods_.AddMethod("TurnOn", "打开插座,当你听到几秒后打开插座时选择此方法，你需要将分钟，小时转化为秒后，当参数传递给此方法", ParameterList({Parameter("timer", "计时器，多少秒后开灯，需要将分钟，小时转化为秒，默认为0", kValueTypeNumber, false)}), [this](const ParameterList &parameters)
                           {
                               TurnOn(parameters); //  直接调用 TurnOn
                           });

        methods_.AddMethod("TurnOff", "关闭插座", ParameterList({Parameter("timer", "计时器，多少秒后关闭插座，需要将分钟，小时转化为秒，默认为0", kValueTypeNumber, false)}), [this](const ParameterList &parameters)
                           {
                               TurnOff(parameters); //  直接调用 TurnOff
                           });

        methods_.AddMethod("CancelTimer", "取消定时器", ParameterList(), [this](const ParameterList &)
                           {
    CancelTimer();
    Application::GetInstance().UpdateIotStates(); });
    }

    void Socket::TurnOn(const ParameterList &parameters)
    {
        turn_on = true;
        timer_ = true;
        int timer = parameters["timer"].number(); // 获取定时器时间（秒）

        // 如果timer大于0，启动定时器任务
        if (timer > 0)
        {
            StartTimerTask(timer); // 启动定时器任务，并传递亮度参数
        }
        else
        {
            // 如果没有定时器，立即执行开灯操作
            SendTurnOnCommand();
        }

        Application::GetInstance().UpdateIotStates();
    }

    void Socket::TurnOff(const ParameterList &parameters)
    {
        turn_on = false;
        timer_ = true;
        int timer = parameters["timer"].number(); // 获取定时器时间（秒）

        // 如果timer大于0，启动定时器任务
        if (timer > 0)
        {
            StartTimerTask(timer); // 启动定时器任务，不传亮度参数
        }
        else
        {
            // 如果没有定时器，立即执行关灯操作
            SendTurnOffCommand();
        }
        Application::GetInstance().UpdateIotStates();
    }

    void Socket::CancelTimer()
    {
        if (timer_task_handle_ != nullptr)
        {
            cancel_timer_ = true; // 设置标志位以取消定时器任务
            ESP_LOGI(TAG, "Timer cancelled.");

            // 等待任务彻底结束，释放任务资源
            vTaskDelay(pdMS_TO_TICKS(100));

            // 删除任务并清除任务句柄
            vTaskDelete(timer_task_handle_);
            timer_task_handle_ = nullptr;

            // 重置定时器参数
            timer_seconds_ = 0;
            brightness_ = 0;
        }
        else
        {
            ESP_LOGI(TAG, "No active timer to cancel.");
        }
    }

    // === 核心新增: 直接HTTP请求Home Assistant ===

    void Socket::SendTurnOnCommand()
    {
        const char *url = "http://10.119.15.62:8123/api/services/switch/turn_on";

        std::string body = std::string("{\"entity_id\":\"") + entity_id + "\"}";

        esp_http_client_config_t config = {};
        config.url = url;
        config.method = HTTP_METHOD_POST;
        config.timeout_ms = 5000;

        esp_http_client_handle_t client = esp_http_client_init(&config);

        std::string auth_header = "Bearer " + std::string(token);
        esp_http_client_set_header(client, "Authorization", auth_header.c_str());
        esp_http_client_set_header(client, "Content-Type", "application/json");

        esp_http_client_set_post_field(client, body.c_str(), body.length());

        esp_err_t err = esp_http_client_perform(client);

        if (err == ESP_OK)
        {
            ESP_LOGI(TAG, "TurnOn HTTP POST succeeded, status = %d", esp_http_client_get_status_code(client));
        }
        else
        {
            ESP_LOGE(TAG, "TurnOn HTTP POST failed: %s", esp_err_to_name(err));
        }

        esp_http_client_cleanup(client);
    }

    void Socket::SendTurnOffCommand()
    {
        const char *url = "http://10.119.15.62:8123/api/services/switch/turn_off";
        std::string body = std::string("{\"entity_id\":\"") + entity_id + "\"}";

        esp_http_client_config_t config = {};
        config.url = url;
        config.method = HTTP_METHOD_POST;
        config.timeout_ms = 5000;

        esp_http_client_handle_t client = esp_http_client_init(&config);

        std::string auth_header = "Bearer " + std::string(token);
        esp_http_client_set_header(client, "Authorization", auth_header.c_str());
        esp_http_client_set_header(client, "Content-Type", "application/json");

        esp_http_client_set_post_field(client, body.c_str(), body.length());

        esp_err_t err = esp_http_client_perform(client);

        if (err == ESP_OK)
        {
            ESP_LOGI(TAG, "TurnOff HTTP POST succeeded, status = %d", esp_http_client_get_status_code(client));
        }
        else
        {
            ESP_LOGE(TAG, "TurnOff HTTP POST failed: %s", esp_err_to_name(err));
        }

        esp_http_client_cleanup(client);
    }

    void Socket::StartTimerTask(int timer_seconds)
    {
        // 设置定时参数
        timer_seconds_ = timer_seconds;

        // 如果之前有任务正在运行，先取消它
        CancelTimer();

        // 启动一个新任务来处理定时器倒计时，并保存任务句柄
        xTaskCreate([](void *arg)
                    {
                        Socket *lamp = static_cast<Socket *>(arg);
                        lamp->TimerTask(); // 执行定时器任务

                        // 任务结束后清除任务句柄，避免悬空句柄
                        lamp->timer_task_handle_ = nullptr;
                        vTaskDelete(NULL); // 删除当前任务
                    },
                    "lamp_timer",       // 任务名称
                    8192,               // 任务栈大小
                    this,               // 传递Lamp实例
                    5,                  // 任务优先级
                    &timer_task_handle_ // 保存任务句柄
        );

        ESP_LOGI(TAG, "Timer task started.");
    }

    void Socket::TimerTask()
    {
        while (timer_seconds_ > 0)
        {
            if (cancel_timer_)
            {
                ESP_LOGI(TAG, "Timer task cancelled, exiting.");
                return;
            }

            ESP_LOGI(TAG, "Timer running, %d seconds remaining.", timer_seconds_);
            vTaskDelay(pdMS_TO_TICKS(1000)); // 延时1秒
            timer_seconds_--;
        }

        // 定时器到期时，执行开关灯操作
        if (timer_seconds_ == 0)
        {
            timer_ = false;
            if (turn_on)
            {
                ESP_LOGI(TAG, "Timer ended, turning on the lamp.");
                SendTurnOnCommand(); // 传递亮度参数开灯
                power_ = true;
            }
            else
            {
                ESP_LOGI(TAG, "Timer ended, turning off the lamp.");
                SendTurnOffCommand(); // 执行关灯操作
                power_ = false;
            }
        }
    }

}
DECLARE_THING(Socket);