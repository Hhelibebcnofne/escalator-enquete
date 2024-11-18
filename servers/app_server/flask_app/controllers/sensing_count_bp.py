from flask import Blueprint
from flask_app import db
from flask_inertia import render_inertia
from flask_app.models import SensingCounts, Questions

sensing_count_bp = Blueprint("sensing_count_bp", __name__)


@sensing_count_bp.route("/sensing_counts", methods=["GET"])
def sensing_count():
    # Question_id毎のoptionA_count, optionB_count, error_countそれぞれのsumを取得
    sensing_counts = (
        db.session.query(
            SensingCounts.question_id,
            db.func.sum(SensingCounts.optionA_count).label("optionA_count"),
            db.func.sum(SensingCounts.optionB_count).label("optionB_count"),
            db.func.sum(SensingCounts.error_count).label("error_count"),
        )
        .group_by(SensingCounts.question_id)
        .all()
    )

    question_ids = [s.question_id for s in sensing_counts]

    questions = Questions.query.filter(Questions.id.in_(question_ids)).all()

    statistics = {
        question.id: {
            "id": question.id,
            "sentence": question.sentence,
            "optionA": question.optionA,
            "optionB": question.optionB,
            "optionA_count": sensing_counts[question.id].optionA_count,
            "optionB_count": sensing_counts[question.id].optionB_count,
            "error_count": sensing_counts[question.id].error_count,
        }
        for question in questions
    }

    print(statistics)

    return render_inertia("SensingCounts", {"statistics": statistics}, view_data={})
