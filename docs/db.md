# データベース設計

## ER 図

```mermaid
erDiagram
    users {
        int id PK "Primary Key"
        text name "Not Null"
    }

    questions {
        int id PK "Primary Key"
        text question_text "question text"
        text optionA "option text 1"
        text optionB "option text 2"
    }

```

## 説明

質問文に対する選択肢は 1 対 2 が想定されるため、テーブルの分割はしない．

Users テーブルは現状使用していない．
