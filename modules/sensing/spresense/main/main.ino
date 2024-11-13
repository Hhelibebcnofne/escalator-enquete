// 共通処理
#include <MP.h>
#define LR_INVERSION false

enum class SensorResult {
    RightDetected = 1,  // 右判定
    LeftDetected = 0,   // 左判定
    ErrorDetected = -1  // エラー判定
};

#if (SUBCORE == 1)
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

    // SensorCoreからのデータを受信
    if (MP.Recv(1, &received_data, 3) == 1) {
        // 受信したデータをBluetooth経由で送信
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

    delay(500);  // Bluetooth通信間隔の遅延
}

#elif (SUBCORE == 2)
// SubCore2 ビルド
#include "WiFi_Module_Manager.h"
#define SUBSCRIBE_TIMEOUT 1000

WiFi_Module_Manager wifi_module_manager;

bool send_count_mqtt_publish(WiFi_Module_Manager& wifi_manager,
                             String message) {
    // Publishモード時の再接続
    if (wifi_manager.initMQTT()) {
        MPLog("MQTTに再接続しました (Publish モード)");
        // Publish 処理
        wifi_manager.mqttPublish(message.c_str());  // MQTTでカウントを送信
        return true;
    } else {
        MPLog("MQTTの再接続に失敗しました。処理を停止します。");
        return false;
    }
}
void setup() {
    MP.begin();
    MPLog("[SubCore2] WiFi Setup\n");
    wifi_module_manager.setup();
    MPLog("[SubCore2] Started\n");
}

void loop() {
    String message;
    if (MP.Recv(2, &message) == 2) {
        MPLog("[SubCore2] Received data: %s\n", message.c_str());
        MPLog("[SubCore2] MQTT Publish\n");
        send_count_mqtt_publish(wifi_module_manager, message.c_str());
    } else {
        wifi_module_manager.mqttSubscribe();
    }
}

#elif (SUBCORE == 3)
// SubCore3 ビルド
#include "ToF_Sensor.h"

#define WALL_THRESHOLD 500
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

    MP.Send(1, static_cast<int>(lr_result), 1);
    MPLog("[SubCore3] Sent data: %d\n", static_cast<int>(lr_result));
}

#elif (SUBCORE == 4)
// SubCore4 ビルド

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
std::mutex countMutex;

bool mqtt_flag = false;

unsigned int set_mqtt_flag() {
    mqtt_flag = true;
    return TIMER_INTERVAL_US;
}

void publish_mqtt_counts() {
    String payload;
    {
        std::lock_guard<std::mutex> countLock(countMutex);
        payload = "Left: " + String(left_count) +
                  ", Right: " + String(right_count) +
                  ", Errors: " + String(error_count);
        // カウントをリセット
        left_count = 0;
        right_count = 0;
        error_count = 0;
    }
    MPLog("Publishing: %s\n", payload.c_str());
    MP.Send(2, &payload, 2);
}

void initLED() {
    pinMode(LED0, OUTPUT);
    digitalWrite(LED0, LOW);  // LEDを消灯
    Serial.println("LEDの初期化が完了し、消灯しました。");
}

void setup() {
    Serial.begin(CONSOLE_BAUDRATE);  // PCとの通信
    initLED();

    MP.begin(1);
    MP.begin(2);
    MP.begin(3);
    // attachTimerInterrupt(set_mqtt_flag, TIMER_INTERVAL_US);
    Serial.println("MainCore: Started");
}

void loop() {
    delay(50000);
    // if (mqtt_flag) {
    publish_mqtt_counts();
    // mqtt_flag = false;
    // }
}

#endif