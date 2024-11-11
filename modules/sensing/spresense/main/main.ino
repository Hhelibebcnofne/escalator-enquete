#include <SoftwareSerial.h>
#include <condition_variable>
#include <mutex>
#include <queue>
#include "ToF_Sensor.h"
#include "WiFi_Module_Manager.h"

#define BT_RX_PIN PIN_D00
#define BT_TX_PIN PIN_D01

#define LR_INVERSION false
#define LR_THRESHOLD 200
#define WALL_THRESHOLD 500
#define USE_HALF_WALL_THRESHOLD true
#if USE_HALF_WALL_THRESHOLD
    #define LR_THRESHOLD
    #define LR_THRESHOLD (int)(WALL_THRESHOLD/2)
#endif
#define TIMER_INTERVAL_US 10000000
#define SUBSCRIBE_TIMEOUT 1000
#define CONSOLE_BAUDRATE 115200

SoftwareSerial BT(BT_TX_PIN, BT_RX_PIN);

ToF_Sensor tof_sensor;
WiFi_Module_Manager wifi_module_manager;

std::queue<int> sensorQueue;
std::mutex queueMutex;
std::condition_variable cv;

int left_count = 0;
int right_count = 0;
int error_count = 0;
std::mutex countMutex;

bool mqtt_flag = false;

pthread_t distance_sensor_process, bluetooth_process;

enum class SensorResult {
    RightDetected = 1,   // 右判定
    LeftDetected = 0,    // 左判定
    ErrorDetected = -1   // エラー判定
};

void start_bluetooth_process() {
    while (true) {
        std::unique_lock<std::mutex> lock(queueMutex);
        cv.wait(lock, [] {
            return !sensorQueue.empty();
        });  // キューにデータが来るまで待機

        int lr_result = sensorQueue.front();
        sensorQueue.pop();
        Serial.print("Popped LR Result: ");
        Serial.println(lr_result);

        String message;
#if LR_INVERSION
        if (lr_result == 0) {
            message = "left_count";
        } else if (lr_result == 1) {
            message = "right_count";
        } else {
            message = "count_error";
        }
#else
        if (lr_result == 0) {
            message = "right_count";
        } else if (lr_result == 1) {
            message = "left_count";
        } else {
            message = "count_error";
        }
#endif

        BT.println(message);      // Bluetooth送信
        Serial.println(message);  // デバッグ出力
        {
            std::lock_guard<std::mutex> countLock(countMutex);
            if (lr_result == 0) {
                right_count++;
            } else if (lr_result == 1) {
                left_count++;
            } else {
                error_count++;
            }
        }
    }
}

void start_distance_sensor() {
    SensorResult lr_result = SensorResult::ErrorDetected;  // 初期値をエラー判定に
    while (true){
        uint16_t distance_value = tof_sensor.get_distance();
        Serial.print("Distance: ");
        Serial.println(distance_value);

        if (distance_value > LR_THRESHOLD && distance_value < WALL_THRESHOLD) {
            if (lr_result == SensorResult::RightDetected) {
                continue;
            }
            lr_result = SensorResult::RightDetected;
        } else if (distance_value < LR_THRESHOLD && distance_value > 60) { // ノイズ対策の60
            if (lr_result == SensorResult::LeftDetected) {
                continue;
            }
            lr_result = SensorResult::LeftDetected;
        } else {
            lr_result = SensorResult::ErrorDetected;
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            sensorQueue.push(static_cast<int>(lr_result));  // enum を int 型に変換してキューに追加
        }

        cv.notify_one();  // Bluetoothスレッドに通知
    }
}

unsigned int set_mqtt_flag() {
    mqtt_flag = true;
    return TIMER_INTERVAL_US;
}

void publish_mqtt_counts(WiFi_Module_Manager& wifi_manager) {
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
    wifi_manager.mqttPublish(payload.c_str());  // MQTTでカウントを送信
}

void initLED() {
    pinMode(LED0, OUTPUT);
    digitalWrite(LED0, LOW);  // LEDを消灯
    Serial.println("LEDの初期化が完了し、消灯しました。");
}

bool send_count_mqtt_publish(WiFi_Module_Manager& wifi_manager) {
    // // Publishモード時の再接続
    if (wifi_manager.initMQTT()) {
        Serial.println("MQTTに再接続しました (Publish モード)");
        // // Publish 処理
        publish_mqtt_counts(wifi_manager);  // カウントデータをMQTTで送信
        return true;
    } else {
        Serial.println("MQTTの再接続に失敗しました。処理を停止します。");
        return false;
    }
}

bool get_question_mqtt_subscribe(WiFi_Module_Manager& wifi_manager) {
    if (wifi_manager.initMQTT()) {
        Serial.println("MQTTに再接続しました (Subscribe モード)");
        // Subscribe 処理
        wifi_manager.mqttSubscribe();
        return true;
    } else {
        Serial.println("MQTTの再接続に失敗しました。処理を停止します。");
        return false;
    }
}

void setup() {
    Serial.begin(CONSOLE_BAUDRATE);  // PCとの通信
    BT.begin(9600);
    initLED();
    tof_sensor.setup();
    wifi_module_manager.setup();

    attachTimerInterrupt(set_mqtt_flag, TIMER_INTERVAL_US);
    pthread_create(&bluetooth_process, NULL, start_bluetooth_process, NULL);
    pthread_create(&distance_sensor_process, NULL, start_distance_sensor, NULL);
}

void loop() {
    Serial.println(tof_sensor.get_distance());
    delay(5000); // 切り替え間隔を調整
    if (mqtt_flag) {
        get_question_mqtt_subscribe(wifi_module_manager);
        send_count_mqtt_publish(wifi_module_manager);
        mqtt_flag = false;
    }
}
