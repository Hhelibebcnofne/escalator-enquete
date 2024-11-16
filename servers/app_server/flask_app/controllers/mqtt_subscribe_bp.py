from flask import Blueprint
from flask_app import mqtt, db, logger
from flask_app.models import MqttMessages, SensingCounts
import json

mqtt_subscribe_bp = Blueprint("mqtt_subscribe_bp", __name__)


@mqtt.on_connect()
def handle_connect(client, userdata, flags, rc):
    mqtt.subscribe("hello/topic")


@mqtt.on_message()
def handle_mqtt_message(client, userdata, message):
    message_text = message.payload.decode()
    print(f"Received message '{message_text}' on topic '{message.topic}'")

    with mqtt.app.app_context():
        try:
            mqtt_message = MqttMessages(
                topic=message.topic, message=message_text, qos=message.qos
            )
            db.session.add(mqtt_message)
            db.session.commit()
        except Exception as e:
            db.session.rollback()
            logger.error(f"Failed to save MQTT message to database: {e}")
        try:
            message_json = json.loads(message_text)
            print(message_json)
            sensing_count = SensingCounts(
                question_id=message_json["question_id"],
                optionA_count=message_json["optionA_count"],
                optionB_count=message_json["optionB_count"],
                error_count=message_json["error_count"],
            )
            db.session.add(sensing_count)
            db.session.commit()
        except Exception as e:
            logger.error(f"Failed to save count data to database: {e}")
            db.session.rollback()
