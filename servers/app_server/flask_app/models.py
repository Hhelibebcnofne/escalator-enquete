from flask_app import db


class User(db.Model):
    __tablename__ = "escalator-enquete"
    id = db.Column(db.Integer, primary_key=True)
    name = db.Column(db.String(255))

    def __init__(self, name):
        self.name = name

    def __repr__(self):
        return f"<Test {self.name}>"

    def toDict(self):
        return {
            "id": self.id,
            "name": self.name,
        }
