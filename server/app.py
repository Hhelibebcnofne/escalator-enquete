from flask import Flask, send_from_directory
from flask_vite import Vite
from flask_inertia import Inertia
from flask_inertia import render_inertia

SECRET_KEY = "secret!"
INERTIA_TEMPLATE = "base.html"

app = Flask(__name__)
app.config.from_object(__name__)
app.config["JSON_AS_ASCII"] = False
vite = Vite(app)

inertia = Inertia()
inertia.init_app(app)


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


if __name__ == "__main__":
    app.run(debug=True)
