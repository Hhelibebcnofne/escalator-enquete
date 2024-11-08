from flask import Blueprint, redirect, url_for, request, jsonify
from flask_app import db, mqtt, logger
from flask_inertia import render_inertia
from flask_app.models import Questions
import json

question_bp = Blueprint("question_bp", __name__)


@question_bp.route("/questions/publish/<int:id>", methods=["POST"])
def publish_question(id):
    question = Questions.query.get_or_404(id)
    question = question.toDict()
    question["created_at"] = question["created_at"].isoformat()
    question["updated_at"] = question["updated_at"].isoformat()

    mqtt.publish("question/topic", json.dumps(question))

    logger.info("Published question to MQTT")
    return redirect(url_for("index.index"))


# GET: Create question page
@question_bp.route("/questions/create", methods=["GET"])
def create_question_view():
    return render_inertia(component_name="CreateQuestion", props={}, view_data={})


# GET: Read all questions / POST: Create a new question
@question_bp.route("/questions", methods=["GET", "POST"])
def manage_questions():
    if request.method == "GET":
        questions = Questions.query.all()
        questions = [quesiton.toDict() for quesiton in questions]
        return render_inertia(
            component_name="ReadQuestions",
            props={"questions": questions},
            view_data={},
        )

    if request.method == "POST":
        try:
            request_data = request.json
            logger.info(f"Request data: {request_data}")
            if not request_data:
                return jsonify({"error": "Invalid data"}), 400

            model_columns = Questions.__table__.columns.keys()
            valid_data = {
                key: value
                for key, value in request_data.items()
                if key in model_columns
            }

            question = Questions(**valid_data)
            db.session.add(question)
            db.session.commit()
            return jsonify({"message": "Questions created successfully"}), 201
        except Exception as e:
            db.session.rollback()
            return jsonify({"error": str(e)}), 500


# GET: Read a specific question
@question_bp.route("/questions/<int:id>", methods=["GET"])
def get_question_view(id):
    question = Questions.query.get_or_404(id)
    return render_inertia(
        component_name="ReadQuestion",
        props={"question": question.toDict()},
        view_data={},
    )


# GET: Edit question page
@question_bp.route("/questions/edit/<int:id>", methods=["GET"])
def edit_question_view(id):
    question = Questions.query.get_or_404(id)
    return render_inertia(
        component_name="UpdateQuestion",
        props={"question": question.toDict()},
        view_data={},
    )


# PUT: Update a specific question
@question_bp.route("/questions/<int:id>", methods=["PUT"])
def update_question(id):
    try:
        question = Questions.query.get_or_404(id)
        request_data = request.json
        logger.info(f"Request data: {request_data}")
        if not request_data:
            return jsonify({"error": "Invalid data"}), 400

        model_columns = Questions.__table__.columns.keys()
        for key, value in request_data.items():
            if key in model_columns:
                setattr(question, key, value)

        db.session.add(question)
        db.session.commit()
        logger.info(f"Questions updated: {question}")
        return jsonify({"message": "Questions updated successfully"}), 200
    except Exception as e:
        db.session.rollback()
        logger.error(f"Failed to update question: {e}")
        return jsonify({"error": str(e)}), 500


# DELETE: Delete a specific question
@question_bp.route("/questions/<int:id>", methods=["DELETE"])
def delete_question(id):
    try:
        question = Questions.query.get_or_404(id)
        db.session.delete(question)
        db.session.commit()
        return jsonify({"message": "Questions deleted successfully"}), 200
    except Exception as e:
        db.session.rollback()
        logger.error(f"Failed to delete question: {e}")
        return jsonify({"error": str(e)}), 500
