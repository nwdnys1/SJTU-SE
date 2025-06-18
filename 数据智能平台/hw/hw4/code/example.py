import requests


def turn_on_light(light_id, api_url, api_key, brightness_pct=50):
    headers = {"Authorization": f"Bearer {api_key}", "Content-Type": "application/json"}
    data = {
        "entity_id": light_id,
    }
    response = requests.post(
        f"{api_url}/api/services/light/turn_on", headers=headers, json=data
    )
    if response.status_code == 200:
        print("Light turned on successfully")
    else:
        print(response._content.decode("utf-8"))


def turn_off_light(light_id, api_url, api_key):
    headers = {"Authorization": f"Bearer {api_key}", "Content-Type": "application/json"}
    data = {"entity_id": light_id}
    response = requests.post(
        f"{api_url}/api/services/light/turn_off", headers=headers, json=data
    )
    if response.status_code == 200:
        print("Light turned off osuccessfully")
    else:
        print(f"Failed to turn off light: {response.status_code}")


# 使用示例on
api_url = "http://10.119.15.62:8123"
api_key = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiIzZTA0YTcwMzIzYjc0OTRhODFhZjkxMDZmNGQ4ZDk1NiIsImlhdCI6MTc1MDEzOTQxNiwiZXhwIjoyMDY1NDk5NDE2fQ.R0iWZukW_6hNqdG2XUVcjUDNUzUP3FrvTBDhjqXveIY"
light_id = "light.xiaomi_cn_944303385_lamp31_s_2_light"


if __name__ == "__main__":
    turn_on_light(
        light_id, api_url, api_key,brightness_pct=10
    ) 