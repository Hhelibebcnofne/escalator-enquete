#include <MqttGs2200.h>
#include <TelitWiFi.h>
#include "config.h"

#define  CONSOLE_BAUDRATE  115200
#define  SUBSCRIBE_TIMEOUT 1000	//ms

TelitWiFi gs2200;
TWIFI_Params gsparams;
MqttGs2200 theMqttGs2200(&gs2200);

// LEDの初期化
void initLED() {
    pinMode(LED0, OUTPUT);
    digitalWrite(LED0, LOW);   // LEDを消灯
    Serial.println("LEDの初期化が完了し、消灯しました。");
}

// Wi-Fi接続の初期化
bool initWiFi() {
    gsparams.mode = ATCMD_MODE_STATION;
    gsparams.psave = ATCMD_PSAVE_DEFAULT;

    Serial.println("Wi-Fiの初期化中...");
    if (gs2200.begin(gsparams)) {
        Serial.println("GS2200の初期化に失敗しました");
        return false;
    }
    Serial.println("GS2200の初期化が成功しました。");

    if (gs2200.activate_station(AP_SSID, PASSPHRASE)) {
        Serial.println("Wi-Fiの接続に失敗しました");
        return false;
    }
    Serial.println("SSID: " + String(AP_SSID) + " に接続しました");
    return true;
}

// MQTTの初期化
bool initMQTT() {
    MQTTGS2200_HostParams hostParams;
    hostParams.host = (char*)MQTT_SRVR;
    hostParams.port = (char*)MQTT_PORT;
    hostParams.clientID = (char*)MQTT_CLI_ID;
    hostParams.userName = (char*)MQTT_USERNAME;
    hostParams.password = (char*)MQTT_PASSWPRD;

    Serial.println("MQTTの初期化中...");
    theMqttGs2200.begin(&hostParams);
    if (!theMqttGs2200.connect()) {
        Serial.println("MQTTの接続に失敗しました");
        return false;
    }
    Serial.println("MQTTに接続しました。");
    return true;
}

// Publish処理
void mqttPublish() {
    Serial.println("MQTTメッセージの送信準備中");
    WiFi_InitESCBuffer();
    MQTTGS2200_Mqtt mqtt;
    
    // Publishトピックを指定
    strncpy(mqtt.params.topic, MQTT_PUBLISH_TOPIC, sizeof(mqtt.params.topic));
    mqtt.params.QoS = 0;
    mqtt.params.retain = 0;
    static uint16_t count = 0;

    snprintf(mqtt.params.message, sizeof(mqtt.params.message), "%d", count++);
    mqtt.params.len = strlen(mqtt.params.message);
    if (theMqttGs2200.publish(&mqtt)) {
        Serial.printf("%d を送信しました\n", count - 1);
        Serial.println("送信したメッセージ: " + String(count - 1));
    } else {
        Serial.println("メッセージの送信に失敗しました: " + String(count - 1));
    }
}

// Subscribe処理
void mqttSubscribe() {
    Serial.println("MQTTメッセージの受信準備中");
    MQTTGS2200_Mqtt mqtt;
    
    // Subscribeトピックを指定
    strncpy(mqtt.params.topic, MQTT_SUBSCRIBE_TOPIC, sizeof(mqtt.params.topic));
    mqtt.params.QoS = 0;
    mqtt.params.retain = 0;

    if (theMqttGs2200.subscribe(&mqtt)) {
        Serial.println("/mqtt/spresense/subscribe に購読しました");
    } else {
        Serial.println("購読に失敗しました");
    }
}

void setup() {
    Serial.begin(CONSOLE_BAUDRATE); // PCとの通信
    Init_GS2200_SPI_type(iS110B_TypeC); // GS2200のSPI初期化
    initLED(); // LEDの初期化

    if (!initWiFi()) {
        Serial.println("Wi-Fiの初期化に失敗しました。処理を停止します。");
        while (1);
    }

    if (!initMQTT()) {
        Serial.println("MQTTの初期化に失敗しました。処理を停止します。");
        while (1);
    }
    
    digitalWrite(LED0, HIGH); // LEDを点灯
    Serial.println("LEDを点灯しました。");
}

void loop() {
    static bool publishing = true; // どちらの処理を行うかのフラグ
    static unsigned long lastActionTime = 0; // 最後の処理を行った時刻

    // 再接続の処理
    if (publishing) {
        // Publishモード時の再接続
        if (!initMQTT()) {
            Serial.println("MQTTの再接続に失敗しました。処理を停止します。");
            while (1);
        }
        Serial.println("MQTTに再接続しました (Publish モード)");
        
        // Publish 処理
        mqttPublish();
        theMqttGs2200.stop(); // Publish後に接続を停止
        lastActionTime = millis();
        publishing = false; // 次は subscribe を実行
    } else {
        // Subscribeモード時の再接続
        if (!initMQTT()) {
            Serial.println("MQTTの再接続に失敗しました。処理を停止します。");
            while (1);
        }
        Serial.println("MQTTに再接続しました (Subscribe モード)");
        
        // Subscribe 処理
        mqttSubscribe();

        // 受信処理
        unsigned long start = millis();
        while (millis() - start < SUBSCRIBE_TIMEOUT) {
            String data;
            while (gs2200.available()) {
                if (!theMqttGs2200.receive(data)) {
                    Serial.println("データの受信に失敗しました。");
                    break;
                }
                Serial.println("受信データ: " + data);
            }
        }

        Serial.printf("購読タイムアウト: %d ms経過しました！\n", SUBSCRIBE_TIMEOUT);
        theMqttGs2200.stop(); // Subscribe後に接続を停止
        lastActionTime = millis();
        publishing = true; // 次は publish を実行
    }

    delay(5000); // 切り替え間隔を調整
}
