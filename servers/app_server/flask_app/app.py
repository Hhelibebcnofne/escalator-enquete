from flask_app import create_app
from flask import send_from_directory
from flask_inertia import render_inertia

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


if __name__ == "__main__":
    app.run(debug=True)
