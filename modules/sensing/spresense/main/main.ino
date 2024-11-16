// 共通処理
#include <MP.h>

#define LR_INVERSION false

#define BLUETOOTH_CORE 1
#define MQTT_PUBLISH_CORE 2
#define SENSOR_CORE 3
#define MQTT_SUBSCRIBE_CORE 4

#define SENSOR_TO_MAIN 1
#define MAIN_TO_BLUETOOTH 2
#define MQTT_TO_MAIN 3
#define MAIN_TO_MQTT 4

#define MSGLEN 512

uint32_t myMsDelta(uint32_t start);

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
    MyPacket* main_to_bluetooth_packet;

    if (MP.Recv(MAIN_TO_BLUETOOTH, &main_to_bluetooth_packet) ==
        MAIN_TO_BLUETOOTH) {
        MPLog("Received data: %s\n", main_to_bluetooth_packet->message);

        BT.println(main_to_bluetooth_packet->message);     // Bluetooth送信
        MPLog("%s\n", main_to_bluetooth_packet->message);  // デバッグ出力
        main_to_bluetooth_packet->status = 0;
    }

    delay(100);  // Bluetooth通信間隔の遅延
}

#elif (SUBCORE == MQTT_PUBLISH_CORE)

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
MyPacket mqtt_to_main_packet;

// the setup function runs once when you press reset or power the board
void setup() {
    memset(&mqtt_to_main_packet, 0, sizeof(mqtt_to_main_packet));
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
    MP.RecvTimeout(MP_RECV_POLLING);

    digitalWrite(LED0, HIGH);  // turn on LED
}

char server_cid = 0;
bool served = false;
uint16_t len, count = 0;
MQTTGS2200_Mqtt mqtt;
int ret;

// the loop function runs over and over again forever
void loop() {
    MyPacket* main_to_mqtt_packet;
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
            if (MP.Recv(MAIN_TO_MQTT, &main_to_mqtt_packet) == MAIN_TO_MQTT) {
                MPLog("Received data: %s\n", main_to_mqtt_packet->message);

                strncpy(mqtt.params.topic, MQTT_PUBLISH_TOPIC,
                        sizeof(mqtt.params.topic));
                strncpy(mqtt.params.message, main_to_mqtt_packet->message,
                        sizeof(mqtt.params.message));

                mqtt.params.len = strlen(mqtt.params.message);
                if (true == theMqttGs2200.publish(&mqtt)) {
                    MPLog("Published: %s\n", main_to_mqtt_packet->message);
                }
                strncpy(mqtt.params.topic, MQTT_SUBSCRIBE_TOPIC,
                        sizeof(mqtt.params.topic));
                main_to_mqtt_packet->status = 0;
            }

            if (myMsDelta(start) < SUBSCRIBE_TIMEOUT) {
                String data;
                /* just in case something from GS2200 */
                while (gs2200.available()) {
                    if (false == theMqttGs2200.receive(data)) {
                        served = false;  // quit the loop
                        break;
                    }

                    Serial.println("Recieve data: " + data);

                    if (mqtt_to_main_packet.status == 0) {
                        /* status -> busy */
                        mqtt_to_main_packet.status = 1;

                        /* Create a message */
                        snprintf(mqtt_to_main_packet.message, MSGLEN, "%s",
                                 data.c_str());

                        /* Send to MainCore */
                        ret = MP.Send(MQTT_TO_MAIN, &mqtt_to_main_packet);
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

#define ARDUINOJSON_ENABLE_PROGMEM 0
#include <ArduinoJson.h>

#define MQTT_INTERVAL_MS 60000 * 1  // ms
#define CONSOLE_BAUDRATE 115200

JsonDocument mqtt_subscribe_json;

SensorResult lr_result = SensorResult::ErrorDetected;  // 初期値をエラー判定に
uint64_t start = millis();

int current_question_id = 0;
int left_count = 0;
int right_count = 0;
int error_count = 0;

MyPacket main_to_bluetooth_packet;
MyPacket main_to_mqtt_packet;
int ret;

void publish_mqtt_counts() {
    JsonDocument mqtt_publish_json;
    String mqtt_publish_str;

    mqtt_publish_json["question_id"] = current_question_id;
#if LR_INVERSION
    mqtt_publish_json["optionA_count"] = right_count;
    mqtt_publish_json["optionB_count"] = left_count;
    mqtt_publish_json["error_count"] = error_count;

#else
    mqtt_publish_json["optionA_count"] = left_count;
    mqtt_publish_json["optionB_count"] = right_count;
    mqtt_publish_json["error_count"] = error_count;

#endif
    serializeJson(mqtt_publish_json, mqtt_publish_str);
    // カウントをリセット
    left_count = 0;
    right_count = 0;
    error_count = 0;
    MPLog("Publishing: %s\n", mqtt_publish_str.c_str());
    // MQTTコアに送信

    if (main_to_mqtt_packet.status == 0) {
        main_to_mqtt_packet.status = 1;
        snprintf(main_to_mqtt_packet.message, MSGLEN, "%s",
                 mqtt_publish_str.c_str());
        ret = MP.Send(MAIN_TO_MQTT, &main_to_mqtt_packet, MQTT_SUBSCRIBE_CORE);
        if (ret < 0) {
            printf("MP.Send error = %d\n", ret);
        }
    }
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
    MP.begin(SENSOR_CORE);
    MP.begin(MQTT_SUBSCRIBE_CORE);
    MP.RecvTimeout(MP_RECV_POLLING);
    Serial.println("MainCore: Started");
}

void loop() {
    if (MP.Recv(SENSOR_TO_MAIN, &lr_result, SENSOR_CORE) == SENSOR_TO_MAIN) {
        MPLog("Received data: %d\n", static_cast<int>(lr_result));
        lr_result = static_cast<SensorResult>(lr_result);

        main_to_bluetooth_packet.status = 1;
#if LR_INVERSION
        if (lr_result == SensorResult::LeftDetected) {
            right_count++;
            snprintf(main_to_bluetooth_packet.message, MSGLEN, "left_count");
        } else if (lr_result == SensorResult::RightDetected) {
            left_count++;
            snprintf(main_to_bluetooth_packet.message, MSGLEN, "right_count");
        } else {
            error_count++;
            snprintf(main_to_bluetooth_packet.message, MSGLEN, "count_error");
        }
#else
        if (lr_result == SensorResult::RightDetected) {
            right_count++;
            snprintf(main_to_bluetooth_packet.message, MSGLEN, "left_count");
        } else if (lr_result == SensorResult::LeftDetected) {
            left_count++;
            snprintf(main_to_bluetooth_packet.message, MSGLEN, "right_count");
        } else {
            error_count++;
            snprintf(main_to_bluetooth_packet.message, MSGLEN, "count_error");
        }
#endif

        ret = MP.Send(MAIN_TO_BLUETOOTH, &main_to_bluetooth_packet,
                      BLUETOOTH_CORE);
        if (ret < 0) {
            printf("MP.Send error = %d\n", ret);
        }
        main_to_bluetooth_packet.status = 0;
        MPLog("Sent data: %s\n", main_to_bluetooth_packet.message);
    }

    MyPacket* mqtt_to_main_packet;

    if (MP.Recv(MQTT_TO_MAIN, &mqtt_to_main_packet, MQTT_SUBSCRIBE_CORE) ==
        MQTT_TO_MAIN) {
        MPLog("Received data from MQTT\n");
        // MPLog("%s\n", mqtt_to_main_packet->message);
        DeserializationError error =
            deserializeJson(mqtt_subscribe_json, mqtt_to_main_packet->message);
        if (error) {
            MPLog("Failed to parse JSON\n");
            MPLog("%s\n", error.c_str());
        } else {
            MPLog("Success to parse JSON\n");

            const int id = mqtt_subscribe_json["id"];
            const char* sentence = mqtt_subscribe_json["sentence"];
            const char* optionA = mqtt_subscribe_json["optionA"];
            const char* optionB = mqtt_subscribe_json["optionB"];
            const char* created_at = mqtt_subscribe_json["created_at"];
            const char* updated_at = mqtt_subscribe_json["updated_at"];

            MPLog("Received id: %d\n", id);
            MPLog("Received sentence: %s\n", sentence);
            MPLog("Received optionA: %s\n", optionA);
            MPLog("Received optionB: %s\n", optionB);
            MPLog("Received created_at: %s\n", created_at);
            MPLog("Received updated_at: %s\n", updated_at);

            current_question_id = id;

            if (main_to_bluetooth_packet.status == 0) {
                main_to_bluetooth_packet.status = 1;
                snprintf(main_to_bluetooth_packet.message, MSGLEN, "%s,%s",
                         optionA, optionB);
                ret = MP.Send(MAIN_TO_BLUETOOTH, &main_to_bluetooth_packet,
                              BLUETOOTH_CORE);
                if (ret < 0) {
                    printf("MP.Send error = %d\n", ret);
                }
            }
        }
        mqtt_to_main_packet->status = 0;
    }

    if (myMsDelta(start) > MQTT_INTERVAL_MS) {
        publish_mqtt_counts();
        start = millis();
    }
}

#endif

uint32_t myMsDelta(uint32_t start) {
    uint32_t now = millis();
    return now - start;
}