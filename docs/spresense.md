# Spresense 設計

```mermaid
sequenceDiagram
    participant Main as メインプロセス
    participant Bt as Bluetoothプロセス
    participant Sensor as センサープロセス
    participant Mqtt as MQTTプロセス

    Sensor ->> Sensor: データ取得
    Sensor ->> Sensor: 左右判定
    Sensor -->> Main: 結果送信
    Main ->> Main: カウント処理
    Main ->> Bt: カウントデータ送信
    Mqtt ->> Mqtt: MQTTサブスクライブ
    Mqtt -->> Main: データ送信
    Main ->> Mqtt: パブリッシュデータ送信
    Mqtt ->> Mqtt: MQTTパブリッシュ
```
