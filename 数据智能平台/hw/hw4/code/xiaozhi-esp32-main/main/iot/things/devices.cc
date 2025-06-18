#include "iot/thing.h"
#include "board.h"
#include "audio_codec.h"
#include <esp_log.h>
#include <esp_http_client.h>
#include "application.h"

#define TAG "Devices"

static char *lamp_id = "light.xiaomi_cn_944303385_lamp31_s_2_light";
static char *camera_id = "switch.chuangmi_cn_1152777781_069a01_on_p_2_1";
static char *md_id = "switch.chuangmi_cn_1152777781_069a01_motion_detection_p_5_1";
static char *mt_id = "switch.chuangmi_cn_1152777781_069a01_motion_tracking_p_2_8";
static char *socket_id = "switch.cuco_cn_941380850_v3_on_p_2_1";
static char *token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJkM2NlM2RkZDU2ZWI0ZjkyOTA0NDVjYmZmZGM4NGFmNyIsImlhdCI6MTc0ODM1MDg1NiwiZXhwIjoyMDYzNzEwODU2fQ.TQedJyKSkmxXNTOt_pdHqCIQHMTZBUi_gaW4TqN41_Q";

namespace iot
{

    // 这里仅定义 Speaker 的属性和方法，不包含具体的实现
    class Devices : public Thing
    {
    public:
        bool turn_on = false; // 设备是否打开
        bool left = false;    // 主人是否离开家

        Devices() : Thing("Devices", "这个物联网设备包含了多个实体，包括摄像头、插座和台灯，可以被联合控制")
        {
            // 定义设备的属性

            // 定义设备可以被远程执行的指令
            methods_.AddMethod("LeftHome", "离开家，当你听到类似于“我出门了”时，执行此方法", ParameterList(),
                               [this](const ParameterList &parameters)
                               {
                                   LeftHome();
                               });
            methods_.AddMethod("ReturnHome", "回家，当你听到类似于“我回来了”时，执行此方法", ParameterList(),
                               [this](const ParameterList &parameters)
                               {
                                   ReturnHome();
                               });
        }
        void LampOn()
        {
            const char *url = "http://10.119.15.62:8123/api/services/light/turn_on";

            std::string body = std::string("{\"entity_id\":\"") + lamp_id + "\"}";

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

        void SocketOn()
        {
            const char *url = "http://10.119.15.62:8123/api/services/switch/turn_on";

            std::string body = std::string("{\"entity_id\":\"") + socket_id + "\"}";

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
        void CameraOn()
        {
            const char *url = "http://10.119.15.62:8123/api/services/switch/turn_on";

            std::string body = std::string("{\"entity_id\":\"") + camera_id + "\"}";

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
        void MDOn()
        {
            const char *url = "http://10.119.15.62:8123/api/services/switch/turn_on";

            std::string body = std::string("{\"entity_id\":\"") + md_id + "\"}";

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
        void MTOn()
        {
            const char *url = "http://10.119.15.62:8123/api/services/switch/turn_on";

            std::string body = std::string("{\"entity_id\":\"") + mt_id + "\"}";

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
        void LampOff()
        {
            const char *url = "http://10.119.15.62:8123/api/services/light/turn_off";

            std::string body = std::string("{\"entity_id\":\"") + lamp_id + "\"}";

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

        void SocketOff()
        {
            const char *url = "http://10.119.15.62:8123/api/services/switch/turn_off";

            std::string body = std::string("{\"entity_id\":\"") + socket_id + "\"}";

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
        void CameraOff()
        {
            const char *url = "http://10.119.15.62:8123/api/services/switch/turn_off";

            std::string body = std::string("{\"entity_id\":\"") + camera_id + "\"}";

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
        void MDOff()
        {
            const char *url = "http://10.119.15.62:8123/api/services/switch/turn_off";

            std::string body = std::string("{\"entity_id\":\"") + md_id + "\"}";

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
        void MTOff()
        {
            const char *url = "http://10.119.15.62:8123/api/services/switch/turn_off";

            std::string body = std::string("{\"entity_id\":\"") + mt_id + "\"}";

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
        void LeftHome()
        {
            LampOff();
            SocketOff();
            CameraOn();
            MDOn();
            MTOn();
        }

        void ReturnHome()
        {
            LampOn();
            SocketOn();
            MDOff();
            MTOff();
        }
    };

} // namespace iot

DECLARE_THING(Devices);
