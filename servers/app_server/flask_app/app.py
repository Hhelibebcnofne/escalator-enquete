from flask import send_from_directory
from flask_app import create_app
from flask_wtf.csrf import generate_csrf

app = create_app()


@app.route("/_vite-static/<path:path>")
def send_public(path):
    return send_from_directory("vite/dist", path)


@app.after_request
def set_csrf_cookie(response):
    csrf_token = generate_csrf()
    response.set_cookie("csrf_token", csrf_token, samesite="Lax", httponly=False)
    return response


if __name__ == "__main__":
    app.run(debug=True)
