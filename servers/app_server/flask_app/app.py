from flask_app import create_app, db
from flask import send_from_directory, request, redirect, url_for
from flask_inertia import render_inertia
from flask_app.models import User

app = create_app()


@app.route("/")
def hello_world():
    data = {}
    return render_inertia(
        component_name="Index",
        props=data,
        view_data={},
    )


@app.route("/_vite-static/<path:path>")
def send_public(path):
    return send_from_directory("vite/dist", path)


@app.route("/users/create", methods=["GET", "POST"])
def add_user():
    if request.method == "GET":
        data = {}
        return render_inertia(
            component_name="CreateUsers",
            props=data,
            view_data={},
        )
    if request.method == "POST":
        user = User(
            name="Test",
        )
        db.session.add(user)
        db.session.commit()
        return redirect(url_for("read_user"))


@app.route("/users/read", methods=["GET"])
def read_user():
    users = User.query.all()
    users = [user.toDict() for user in users]
    print(users)
    data = {"users": users}
    return render_inertia(
        component_name="ReadUsers",
        props=data,
        view_data={},
    )


# @app.route("/users/<int:id>", methods=["GET", "PUT"])
# def update_user(id):
#     user = User.query.get(id)
#     if request.method == "GET":
#         data = {"user": user}
#         return render_inertia(
#             component_name="UpdateUsers",
#             props=data,
#             view_data={},
#         )
#     if request.method == "PUT":
#         user = User.query.get(id)
#         user.name = request.form["name"]
#         db.session.merge(user)
#         db.session.commit()
#         return redirect(url_for("read_user"))


# @app.route("/users/<int:id>", methods=["DELETE"])
# def delete_user(id):
#     user = User.query.get(id)
#     db.session.delete(user)
#     db.session.commit()
#     return redirect(url_for("read_user"))


if __name__ == "__main__":
    app.run(debug=True)
