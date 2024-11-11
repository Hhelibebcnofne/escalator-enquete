# データベース設計

## ER 図

```mermaid
erDiagram

    questions {
        int id PK "Primary Key"
        text question_text "Not Null, Question text"
        text optionA "Not Null, Option text 1"
        text optionB "Not Null, Option text 2"
        timestamp created_at "Created timestamp"
        timestamp updated_at "Update timestamp"
    }

    mqtt_messages {
        int id PK "Primary Key"
        text topic "MQTT topic"
        text message "MQTT message text"
        int qos "Quality of service"
        timestamp created_at "Created timestamp"
        timestamp updated_at "Update timestamp"
    }

```

## 説明

質問文に対する選択肢は 1 対 2 が想定されるため、テーブルの分割はしない．
