from flask_app import db
from flask import Blueprint, request, redirect, url_for
from flask_inertia import render_inertia
from flask_app.models import User

user_bp = Blueprint("user", __name__)


@user_bp.route("/users/create", methods=["GET"])
def add_user():
    data = {}
    return render_inertia(
        component_name="CreateUsers",
        props=data,
        view_data={},
    )


@user_bp.route("/users", methods=["GET", "POST"])
def read_user():
    if request.method == "GET":
        users = User.query.all()
        users = [user.toDict() for user in users]
        data = {"users": users}
        return render_inertia(
            component_name="ReadUsers",
            props=data,
            view_data={},
        )
    if request.method == "POST":
        username = request.json["name"]
        user = User(
            name=username,
        )
        db.session.add(user)
        db.session.commit()
        return redirect(url_for("read_user"))


@user_bp.route("/users/<int:id>", methods=["GET", "PUT"])
def update_user(id):
    user = User.query.get(id)
    user = user.toDict()
    if request.method == "GET":
        data = {"user": user}
        return render_inertia(
            component_name="UpdateUsers",
            props=data,
            view_data={},
        )
    if request.method == "PUT":
        user = User.query.get(id)
        user.name = request.json["name"]
        db.session.merge(user)
        db.session.commit()
        return redirect(url_for("read_user"))


@user_bp.route("/users/<int:id>", methods=["DELETE"])
def delete_user(id):
    user = User.query.get(id)
    db.session.delete(user)
    db.session.commit()
    return redirect(url_for("read_user"))
