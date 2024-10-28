from flask import Blueprint, redirect, url_for
from flask_app import mqtt, logger
import json

question_bp = Blueprint("question_bp", __name__)


@question_bp.route("/question")
def publish_question():
    question = {
        "sentence": "What is the answer to life, the universe, and everything?",
        "optionA": "42",
        "optionB": "24",
    }
    mqtt.publish("question/topic", json.dumps(question))

    logger.info("Published question to MQTT")
    return redirect(url_for("index.index"))
