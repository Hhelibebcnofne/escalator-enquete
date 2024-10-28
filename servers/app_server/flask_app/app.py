from flask import send_from_directory
from flask_app import create_app

app = create_app()


@app.route("/_vite-static/<path:path>")
def send_public(path):
    return send_from_directory("vite/dist", path)


if __name__ == "__main__":
    app.run(debug=True)
