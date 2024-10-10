from flask_app import create_app, db, mqtt
from flask import send_from_directory, request, redirect, url_for
from flask_inertia import render_inertia
from flask_app.models import User, Mqtt

app = create_app()


@mqtt.on_connect()
def handle_connect(client, userdata, flags, rc):
    mqtt.subscribe("hello/topic")


@mqtt.on_message()
def handle_mqtt_message(client, userdata, message):
    message_text = message.payload.decode()
    print(f"Received message '{message_text}' on topic '{message.topic}'")
    with app.app_context():
        mqtt = Mqtt(topic=message.topic, message=message_text, qos=message.qos)
        db.session.add(mqtt)
        db.session.commit()


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


@app.route("/users/create", methods=["GET"])
def add_user():
    data = {}
    return render_inertia(
        component_name="CreateUsers",
        props=data,
        view_data={},
    )


@app.route("/users", methods=["GET", "POST"])
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


@app.route("/users/<int:id>", methods=["GET", "PUT"])
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


@app.route("/users/<int:id>", methods=["DELETE"])
def delete_user(id):
    user = User.query.get(id)
    db.session.delete(user)
    db.session.commit()
    return redirect(url_for("read_user"))


if __name__ == "__main__":
    app.run(debug=True)
