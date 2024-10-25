from flask import Flask
from flask_vite import Vite
from flask_inertia import Inertia
from flask_sqlalchemy import SQLAlchemy
from flask_mqtt import Mqtt

db = SQLAlchemy()
inertia = Inertia()
mqtt = Mqtt()


def create_app():
    # setup app
    app = Flask(__name__, instance_relative_config=True)
    app.config.from_pyfile("config.py")
    app.config["JSON_AS_ASCII"] = False

    # database
    db.init_app(app)
    # don't delete if it's not used
    from flask_app import models  # noqa: F401

    # vite setup
    # don't delete if it's not used
    vite = Vite(app)  # noqa: F841

    inertia.init_app(app)

    mqtt.init_app(app)

    from flask_app.controllers.index_bp import index_bp
    from flask_app.controllers.user_bp import user_bp

    app.register_blueprint(index_bp)
    app.register_blueprint(user_bp)

    return app
