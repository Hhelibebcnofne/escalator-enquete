from flask_app import db, logger
from flask import Blueprint, request, jsonify
from flask_inertia import render_inertia
from flask_app.models import Users

user_bp = Blueprint("user", __name__)


# GET: Create user page
@user_bp.route("/users/create", methods=["GET"])
def create_user_view():
    return render_inertia(component_name="CreateUsers", props={}, view_data={})


# GET: Read all users / POST: Create a new user
@user_bp.route("/users", methods=["GET", "POST"])
def manage_users():
    if request.method == "GET":
        users = Users.query.all()
        users = [user.toDict() for user in users]
        return render_inertia(
            component_name="ReadUsers", props={"users": users}, view_data={}
        )

    if request.method == "POST":
        try:
            username = request.json.get("name")
            if not username:
                return jsonify({"error": "Name is required"}), 400

            user = Users(name=username)
            db.session.add(user)
            db.session.commit()
            return jsonify({"message": "User created successfully"}), 201
        except Exception as e:
            db.session.rollback()
            return jsonify({"error": str(e)}), 500


# GET: Read a specific user
@user_bp.route("/users/<int:id>", methods=["GET"])
def get_user_view(id):
    user = Users.query.get_or_404(id)
    return render_inertia(
        component_name="ReadUser", props={"user": user.toDict()}, view_data={}
    )


# GET: Edit user page
@user_bp.route("/users/edit/<int:id>", methods=["GET"])
def edit_user_view(id):
    user = Users.query.get_or_404(id)
    return render_inertia(
        component_name="UpdateUsers", props={"user": user.toDict()}, view_data={}
    )


# PUT: Update a specific user
@user_bp.route("/users/<int:id>", methods=["PUT"])
def update_user(id):
    try:
        user = Users.query.get_or_404(id)
        username = request.json.get("name")
        if not username:
            return jsonify({"error": "Name is required"}), 400

        user.name = username
        db.session.add(user)
        db.session.commit()
        logger.info(f"User updated: {user}")
        return jsonify({"message": "User updated successfully"}), 200
    except Exception as e:
        db.session.rollback()
        logger.error(f"Failed to update user: {e}")
        return jsonify({"error": str(e)}), 500


# DELETE: Delete a specific user
@user_bp.route("/users/<int:id>", methods=["DELETE"])
def delete_user(id):
    try:
        user = Users.query.get_or_404(id)
        db.session.delete(user)
        db.session.commit()
        return jsonify({"message": "User deleted successfully"}), 200
    except Exception as e:
        db.session.rollback()
        logger.error(f"Failed to delete user: {e}")
        return jsonify({"error": str(e)}), 500
