from flask_app import mqtt, db, create_app
from flask_app.models import MqttMessages

app = create_app()


@mqtt.on_connect()
def handle_connect(client, userdata, flags, rc):
    mqtt.subscribe("hello/topic")


@mqtt.on_message()
def handle_mqtt_message(client, userdata, message):
    message_text = message.payload.decode()
    print(f"Received message '{message_text}' on topic '{message.topic}'")
    with app.app_context():
        mqtt_message = MqttMessages(
            topic=message.topic, message=message_text, qos=message.qos
        )
        db.session.add(mqtt_message)
        db.session.commit()


if __name__ == "__main__":
    app.run(debug=True)
