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
    created_at = db.Column(db.DateTime, server_default=db.func.now())
    updated_at = db.Column(
        db.DateTime, server_default=db.func.now(), onupdate=db.func.now()
    )

    def __init__(self, sentence: str, optionA: str, optionB: str) -> None:
        self.sentence = sentence
        self.optionA = optionA
        self.optionB = optionB

    __table_args__ = (
        db.UniqueConstraint(
            "sentence", "optionA", "optionB", name="uq_sentence_optionA_optionB"
        ),
    )

    def __repr__(self) -> str:
        return f"<Question {self.sentence}>"

    def toDict(self) -> dict:
        return {
            "id": self.id,
            "sentence": self.sentence,
            "optionA": self.optionA,
            "optionB": self.optionB,
            "created_at": self.created_at,
            "updated_at": self.updated_at,
        }


class SensingCounts(db.Model):
    __tablename__ = "sensing_counts"
    id: int = db.Column(db.Integer, primary_key=True)
    question_id: int = db.Column(
        db.Integer, db.ForeignKey("questions.id", ondelete="CASCADE"), nullable=False
    )
    optionA_count: int = db.Column(db.Integer, default=0, nullable=False)
    optionB_count: int = db.Column(db.Integer, default=0, nullable=False)
    error_count: int = db.Column(db.Integer, default=0, nullable=False)
    created_at = db.Column(db.DateTime, server_default=db.func.now())
    updated_at = db.Column(
        db.DateTime, server_default=db.func.now(), onupdate=db.func.now()
    )

    def __init__(
        self, question_id: int, optionA_count: int, optionB_count: int, error_count: int
    ) -> None:
        self.question_id = question_id
        self.optionA_count = optionA_count
        self.optionB_count = optionB_count
        self.error_count = error_count

    def __repr__(self) -> str:
        return f"<SensingCount {self.question_id}>"

    def toDict(self) -> dict:
        return {
            "id": self.id,
            "question_id": self.question_id,
            "optionA_count": self.optionA_count,
            "optionB_count": self.optionB_count,
            "error_count": self.error_count,
            "created_at": self.created_at,
            "updated_at": self.updated_at,
        }
