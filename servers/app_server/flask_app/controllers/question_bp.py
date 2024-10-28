from flask import Blueprint, redirect, url_for
from flask_app import mqtt, logger

question_bp = Blueprint("question_bp", __name__)


@question_bp.route("/question")
def publish_question():
    mqtt.publish(
        "question/topic", "What is the answer to life, the universe, and everything?"
    )
    logger.info("Published question to MQTT")
    return redirect(url_for("index.index"))
