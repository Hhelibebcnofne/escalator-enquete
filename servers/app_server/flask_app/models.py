from flask_app import db


class Users(db.Model):
    __tablename__: str = "users"
    id: int = db.Column(db.Integer, primary_key=True)
    name: str = db.Column(db.String(255), nullable=False)
    created_at = db.Column(db.DateTime, server_default=db.func.now())
    updated_at = db.Column(
        db.DateTime, server_default=db.func.now(), onupdate=db.func.now()
    )

    def __init__(self, name: str) -> None:
        self.name = name

    def __repr__(self) -> str:
        return f"<Test {self.name}>"

    def toDict(self) -> dict:
        return {
            "id": self.id,
            "name": self.name,
            "created_at": self.created_at,
            "updated_at": self.updated_at,
        }


class MqttMessages(db.Model):
    __tablename__ = "mqtt_messages"
    id: int = db.Column(db.Integer, primary_key=True)
    topic: str = db.Column(db.String(255))
    message: str = db.Column(db.String(255))
    qos: int = db.Column(db.Integer)
    created_at = db.Column(db.DateTime, server_default=db.func.now())
    updated_at = db.Column(
        db.DateTime, server_default=db.func.now(), onupdate=db.func.now()
    )

    def __init__(self, topic: str, message: str, qos: int) -> None:
        self.topic = topic
        self.message = message
        self.qos = qos

    def __repr__(self) -> str:
        return f"<Mqtt {self.message}>"

    def toDict(self) -> dict:
        return {
            "id": self.id,
            "topic": self.topic,
            "message": self.message,
            "qos": self.qos,
            "created_at": self.created_at,
            "updated_at": self.updated_at,
        }


class Questions(db.Model):
    __tablename__ = "questions"
    id: int = db.Column(db.Integer, primary_key=True)
    sentence: str = db.Column(db.String(255), nullable=False)
    optionA: str = db.Column(db.String(255), nullable=False)
    optionB: str = db.Column(db.String(255), nullable=False)

    def __init__(self, sentence: str, optionA: str, optionB: str) -> None:
        self.sentence = sentence
        self.optionA = optionA
        self.optionB = optionB

    def __repr__(self) -> str:
        return f"<Question {self.sentence}>"

    def toDict(self) -> dict:
        return {
            "id": self.id,
            "sentence": self.sentence,
            "optionA": self.optionA,
            "optionB": self.optionB,
        }
