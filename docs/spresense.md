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
    Bt ->> Bt: Bluetooth出力
    Mqtt ->> Mqtt: MQTTサブスクライブ
    Mqtt -->> Main: 表示データ送信
    Main ->> Bt: 表示データ送信
    Bt ->> Bt: Bluetooth出力
    Main ->> Mqtt: パブリッシュデータ送信
    Mqtt ->> Mqtt: MQTTパブリッシュ
```
