from app import create_app
from app.services.mqtt_service import run_mqtt
import threading

app = create_app()

if __name__ == '__main__':

    t1 = threading.Thread(target=run_mqtt)
    t1.start()

    app.run(host='0.0.0.0', port=5000)