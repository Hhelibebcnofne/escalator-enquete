// 共通処理
#include <MP.h>
#define LR_INVERSION false

#define BLUETOOTH_CORE 1
#define MQTT_PUBLISH_CORE 2
#define SENSOR_CORE 3
#define MQTT_SUBSCRIBE_CORE 4

#define SENSOR_TO_MAIN 1
#define MAIN_TO_BLUETOOTH 2
#define MQTT_SUBSCRIBE_TO_MAIN 3
#define MAIN_TO_MQTT_PUBLISH 4

#define MSGLEN 512

enum class SensorResult {
    RightDetected = 1,  // 右判定
    LeftDetected = 0,   // 左判定
    ErrorDetected = -1  // エラー判定
};

struct MyPacket {
    volatile int status; /* 0:ready, 1:busy */
    char message[MSGLEN];
};

#if (SUBCORE == BLUETOOTH_CORE)
// SubCore1 ビルド
#include <SoftwareSerial.h>

#define BT_RX_PIN PIN_D00
#define BT_TX_PIN PIN_D01
#define BT_BAUDRATE 9600
SoftwareSerial BT(BT_TX_PIN, BT_RX_PIN);

void setup() {
    MP.begin();             // SubCore起動完了通知
    BT.begin(BT_BAUDRATE);  // Bluetooth通信開始

    MPLog("[SubCore1] Bluetooth communication started\n");
}

void loop() {
    SensorResult received_data = SensorResult::ErrorDetected;

    if (MP.Recv(MAIN_TO_BLUETOOTH, &received_data) == MAIN_TO_BLUETOOTH) {
        MPLog("[SubCore1] Received data: %d\n",
              static_cast<int>(received_data));

        received_data = static_cast<SensorResult>(received_data);

        String message;
#if LR_INVERSION
        if (received_data == SensorResult::LeftDetected) {
            message = "left_count";
        } else if (received_data == SensorResult::RightDetected) {
            message = "right_count";
        } else {
            message = "count_error";
        }
#else
        if (received_data == SensorResult::LeftDetected) {
            message = "right_count";
        } else if (received_data == SensorResult::RightDetected) {
            message = "left_count";
        } else {
            message = "count_error";
        }
#endif

        BT.println(message);  // Bluetooth送信
        // MPLog("%s", message);  // デバッグ出力
    }

    delay(100);  // Bluetooth通信間隔の遅延
}

#elif (SUBCORE == MQTT_PUBLISH_CORE)
// SubCore2 ビルド
#include <MqttGs2200.h>
#include <TelitWiFi.h>
#include "config.h"

#define CONSOLE_BAUDRATE 115200
#define SUBSCRIBE_TIMEOUT 2000

TelitWiFi gs2200;
TWIFI_Params gsparams;
MqttGs2200 theMqttGs2200 = MqttGs2200(&gs2200);

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

bool mqttPublish(char* send_message) {
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

bool mqttSubscribe() {
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

void setup() {
    MP.begin();
    Serial.begin(CONSOLE_BAUDRATE);  // PCとの通信
    MPLog("[SubCore2] WiFi Setup\n");
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
    MPLog("[SubCore2] Started\n");
}

void loop() {
    String message;
    if (MP.Recv(2, &message) == 2) {
        MPLog("[SubCore2] Received data: %s\n", message.c_str());
        mqttPublish((char*)message.c_str());
        MPLog("[SubCore2] MQTT Publish\n");
    } else {
        MPLog("[SubCore2] No data received\n");
    }
    delay(5000);
}

#elif (SUBCORE == SENSOR_CORE)
// SubCore3 ビルド
#include "ToF_Sensor.h"

#define WALL_THRESHOLD 1600
// #define WALL_THRESHOLD 500
#define USE_HALF_WALL_THRESHOLD true

#if USE_HALF_WALL_THRESHOLD

#define LR_THRESHOLD (int)(WALL_THRESHOLD / 2)

#else

#define LR_THRESHOLD 200

#endif

ToF_Sensor tof_sensor;

SensorResult lr_result = SensorResult::ErrorDetected;  // 初期値をエラー判定に

void setup() {
    MP.begin();
    tof_sensor.setup();
    MPLog("[SubCore3] Started\n");
}

void loop() {
    delay(100);
    uint16_t distance_value = tof_sensor.get_distance();
    // MPLog("Distance: %d\n", distance_value);

    if (distance_value > LR_THRESHOLD && distance_value < WALL_THRESHOLD) {
        if (lr_result == SensorResult::RightDetected) {
            return;
        }
        lr_result = SensorResult::RightDetected;
    } else if (distance_value < LR_THRESHOLD &&
               distance_value > 60) {  // ノイズ対策の60
        if (lr_result == SensorResult::LeftDetected) {
            return;
        }
        lr_result = SensorResult::LeftDetected;
    } else {
        lr_result = SensorResult::ErrorDetected;
        return;
    }

    MP.Send(SENSOR_TO_MAIN, static_cast<int>(lr_result));
    MPLog("[SubCore3] Sent data: %d\n", static_cast<int>(lr_result));
}

#elif (SUBCORE == MQTT_SUBSCRIBE_CORE)
// SubCore4 ビルド
#include <MqttGs2200.h>
#include <TelitWiFi.h>
#include "config.h"

#define CONSOLE_BAUDRATE 115200
// #define SUBSCRIBE_TIMEOUT 60 * 60000  // ms
#define SUBSCRIBE_TIMEOUT 60000  // ms
/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
TelitWiFi gs2200;
TWIFI_Params gsparams;
MqttGs2200 theMqttGs2200(&gs2200);
MyPacket packet;

// the setup function runs once when you press reset or power the board
void setup() {
    memset(&packet, 0, sizeof(packet));
    MP.begin();
    MQTTGS2200_HostParams hostParams;
    /* initialize digital pin LED_BUILTIN as an output. */
    pinMode(LED0, OUTPUT);
    digitalWrite(LED0, LOW);  // turn the LED off (LOW is the voltage level)
    Serial.begin(CONSOLE_BAUDRATE);  // talk to PC

    /* Initialize SPI access of GS2200 */
    Init_GS2200_SPI_type(iS110B_TypeC);

    /* Initialize AT Command Library Buffer */
    gsparams.mode = ATCMD_MODE_STATION;
    gsparams.psave = ATCMD_PSAVE_DEFAULT;
    if (gs2200.begin(gsparams)) {
        ConsoleLog("GS2200 Initilization Fails");
        while (1)
            ;
    }

    /* GS2200 Association to AP */
    if (gs2200.activate_station(AP_SSID, PASSPHRASE)) {
        ConsoleLog("Association Fails");
        while (1)
            ;
    }
    hostParams.host = (char*)MQTT_SRVR;
    hostParams.port = (char*)MQTT_PORT;
    hostParams.clientID = (char*)MQTT_CLI_ID;
    hostParams.userName = (char*)MQTT_USERNAME;
    hostParams.password = (char*)MQTT_PASSWPRD;

    theMqttGs2200.begin(&hostParams);
    digitalWrite(LED0, HIGH);  // turn on LED
}

char server_cid = 0;
bool served = false;
uint16_t len, count = 0;
MQTTGS2200_Mqtt mqtt;
int ret;

// the loop function runs over and over again forever
void loop() {
    if (!served) {
        // Start a MQTT client
        ConsoleLog("Start MQTT Client");
        if (false == theMqttGs2200.connect()) {
            return;
        }

        ConsoleLog("Start to receive MQTT Message");
        // Prepare for the next chunck of incoming data
        WiFi_InitESCBuffer();

        // Start the loop to receive the data
        strncpy(mqtt.params.topic, MQTT_SUBSCRIBE_TOPIC,
                sizeof(mqtt.params.topic));
        mqtt.params.QoS = 0;
        mqtt.params.retain = 0;
        if (true == theMqttGs2200.subscribe(&mqtt)) {
            ConsolePrintf("Subscribed! \n");
        }

        served = true;
    } else {
        uint64_t start = millis();
        while (served) {
            if (msDelta(start) < SUBSCRIBE_TIMEOUT) {
                String data;
                /* just in case something from GS2200 */
                while (gs2200.available()) {
                    if (false == theMqttGs2200.receive(data)) {
                        served = false;  // quit the loop
                        break;
                    }

                    Serial.println("Recieve data: " + data);

                    if (packet.status == 0) {
                        /* status -> busy */
                        packet.status = 1;

                        /* Create a message */
                        snprintf(packet.message, MSGLEN, "%s", data.c_str());

                        /* Send to MainCore */
                        ret = MP.Send(MQTT_SUBSCRIBE_TO_MAIN, &packet);
                        if (ret < 0) {
                            printf("MP.Send error = %d\n", ret);
                        }
                    }
                }
                start = millis();
            } else {
                ConsolePrintf("Subscribed over for %d ms! \n",
                              SUBSCRIBE_TIMEOUT);
                theMqttGs2200.stop();
                served = false;
                exit(0);
            }
        }
    }
}

#elif (SUBCORE == 5)
// SubCore5 ビルド

#else
// MainCore ビルド
#include <mutex>

#define TIMER_INTERVAL_US 10000000
#define CONSOLE_BAUDRATE 115200

int left_count = 0;
int right_count = 0;
int error_count = 0;

bool mqtt_flag = false;

unsigned int set_mqtt_flag() {
    mqtt_flag = true;
    return TIMER_INTERVAL_US;
}

void publish_mqtt_counts() {
    String payload;
    payload = "Left: " + String(left_count) +
              ", Right: " + String(right_count) +
              ", Errors: " + String(error_count);
    // カウントをリセット
    left_count = 0;
    right_count = 0;
    error_count = 0;
    MPLog("Publishing: %s\n", payload.c_str());
    // MP.Send(MAIN_TO_MQTT_PUBLISH, &payload, MQTT_PUBLISH_CORE);
}

void initLED() {
    pinMode(LED0, OUTPUT);
    digitalWrite(LED0, LOW);  // LEDを消灯
    Serial.println("LEDの初期化が完了し、消灯しました。");
}

void setup() {
    Serial.begin(CONSOLE_BAUDRATE);  // PCとの通信
    while (!Serial)
        ;
    initLED();

    MP.begin(BLUETOOTH_CORE);
    // MP.begin(MQTT_PUBLISH_CORE);
    MP.begin(SENSOR_CORE);
    MP.begin(MQTT_SUBSCRIBE_CORE);
    attachTimerInterrupt(set_mqtt_flag, TIMER_INTERVAL_US);
    MP.RecvTimeout(MP_RECV_POLLING);
    Serial.println("MainCore: Started");
}

SensorResult lr_result = SensorResult::ErrorDetected;  // 初期値をエラー判定に
char recvBuffer[256];
String mqtt_message;

void loop() {
    int subid;
    int8_t msgid;
    MyPacket* packet;

    if (MP.Recv(SENSOR_TO_MAIN, &lr_result, SENSOR_CORE) == SENSOR_TO_MAIN) {
        MPLog("Received data: %d\n", static_cast<int>(lr_result));
        lr_result = static_cast<SensorResult>(lr_result);

        if (lr_result == SensorResult::RightDetected) {
            right_count++;
        } else if (lr_result == SensorResult::LeftDetected) {
            left_count++;
        } else {
            error_count++;
        }

        MP.Send(MAIN_TO_BLUETOOTH, static_cast<int>(lr_result), BLUETOOTH_CORE);
        MPLog("Sent data: %d\n", static_cast<int>(lr_result));
    }

    if (MP.Recv(MQTT_SUBSCRIBE_TO_MAIN, &packet, MQTT_SUBSCRIBE_CORE) ==
        MQTT_SUBSCRIBE_TO_MAIN) {
        mqtt_message = String(recvBuffer);
        MPLog("Received data from MQTT\n");
        MPLog("%s\n", packet->message);
        packet->status = 0;
    }

    if (mqtt_flag) {
        publish_mqtt_counts();
        mqtt_flag = false;
    }
}

#endif