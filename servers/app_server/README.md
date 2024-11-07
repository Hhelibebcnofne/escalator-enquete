# server

## 実行方法

### frontend

nodejs のバージョンは 20.17.0

可能であれば asdf でのインストールを推奨します．
legacy_version を使っています．
.asdfrc を編集して，
`legacy_version_file = yes`
としてください．

普通に nodejs をバージョン指定でインストールしても構いません．

`cd ./servers/app_server/flask_app/vite`

ここから 2 回目以降不要

`asdf install` asdf 利用者のみ

`npm install -g pnpm@latest-8`

ここまで 2 回目以降不要

`pnpm install` パッケージ変更時と 1 回目のみ

### backend

python のバージョンは 3.12.3

rye のインストールを推奨します．

windows であれば，scoop をインストールして，そこに rye をインストールするのを推奨します．

scoop 無しでも使いやすいとは思います．

#### 以下サーバ実行までの手順

##### 定期的に実行すること

データベースの更新

```bash

cd ./servers/app_server/flask_app

rye run flask db upgrade

```

モデル変更時

```bash

cd ./servers/app_server/flask_app

rye run flask migrate -m "更新内容を示す英語のメッセージ"

rye run flask db upgrade

```

##### サーバーの起動

ターミナル 1 つ目

```bash
cd ./servers/app_server/flask_app

rye sync

rye run flask --debug run
```

ターミナル 2 つ目

```bash
cd ./servers/app_server/flask_app

rye run flask vite start
```
