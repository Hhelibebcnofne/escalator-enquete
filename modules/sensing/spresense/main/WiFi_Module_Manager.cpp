#include "WiFi_Module_Manager.h"

WiFi_Module_Manager::WiFi_Module_Manager() : gs2200(), theMqttGs2200(&gs2200) {
    TWIFI_Params gsparams;
    // bool publish_served = false;
    // bool subscribe_served = false;
    uint16_t len;
}

bool WiFi_Module_Manager::setup() {
    Init_GS2200_SPI_type(iS110B_TypeC);  // GS2200のSPI初期化
    // initLED(); // LEDの初期化

    if (!initWiFi()) {
        Serial.println("Wi-Fiの初期化に失敗しました。処理を停止します。");
        while (1)
            ;
    }

    if (!initMQTT()) {
        Serial.println("MQTTの初期化に失敗しました。処理を停止します。");
        while (1)
            ;
    }

    digitalWrite(LED0, HIGH);  // LEDを点灯
    Serial.println("LEDを点灯しました。");
}

bool WiFi_Module_Manager::initWiFi() {
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

bool WiFi_Module_Manager::initMQTT() {
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

bool WiFi_Module_Manager::mqttPublish(char* send_message) {
    Serial.println("MQTTメッセージの送信準備中");
    WiFi_InitESCBuffer();
    MQTTGS2200_Mqtt mqtt;

    // Publishトピックを指定
    strncpy(mqtt.params.topic, MQTT_PUBLISH_TOPIC, sizeof(mqtt.params.topic));
    mqtt.params.QoS = 0;
    mqtt.params.retain = 0;

    // メッセージとして引数 `send_message` をセット
    strncpy(mqtt.params.message, send_message, sizeof(mqtt.params.message));
    mqtt.params.len = strlen(mqtt.params.message);

    // メッセージ送信
    if (theMqttGs2200.publish(&mqtt)) {
        Serial.println("送信したメッセージ: " + String(send_message));
    } else {
        Serial.println("メッセージの送信に失敗しました: " +
                       String(send_message));
        return false;
    }
    theMqttGs2200.stop();
    return true;
}

bool WiFi_Module_Manager::mqttSubscribe() {
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
        return false;
    }

    unsigned long start = millis();
    while (millis() - start < SUBSCRIBE_TIMEOUT) {
        String data;
        while (gs2200.available()) {
            if (!theMqttGs2200.receive(data)) {
                Serial.println("データの受信に失敗しました。");
            }
            Serial.println("受信データ: " + data);
        }
    }

    Serial.printf("購読タイムアウト: %d ms経過しました！\n", SUBSCRIBE_TIMEOUT);
    theMqttGs2200.stop();
    return true;
}
