# escalator-enquete

## 実行方法

### frontend

nodejs のバージョンは 20.17.0

可能であれば asdf でのインストールを推奨します．
legacy_version を使っています．
.asdfrc を編集して，
`legacy_version_file = yes`
としてください．

普通に nodejs をバージョン指定でインストールしても構いません．

#### 以下実行までの手順

`cd ./servers/frontend`

ここから 2 回目以降不要

`asdf install` asdf 利用者のみ

`npm install -g pnpm@latest-8`

`pnpm install` パッケージ変更時と 1 回目のみ

ここまで 2 回目以降不要

`pnpm run dev`

### backend
