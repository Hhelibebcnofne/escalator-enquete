from flask import Blueprint

from flask_inertia import render_inertia

index_bp = Blueprint("index", __name__)


@index_bp.route("/")
def index():
    data = {}
    return render_inertia(
        component_name="Index",
        props=data,
        view_data={},
    )
