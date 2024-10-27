#include <SoftwareSerial.h>
#include <condition_variable>
#include <mutex>
#include <queue>
#include "WiFi_Module_Manager.h"
#include "ToF_Sensor.h"

#define BT_RX_PIN PIN_D00
#define BT_TX_PIN PIN_D01

#define LR_INVERSION false
#define LR_THRESHOLD 200
#define TIMER_INTERVAL_US 10000000
#define SENSING_RATE_MS 1000

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
    while (true) {
        uint16_t distance_value = tof_sensor.get_distance();
        int lr_result = -1;
        if (distance_value > LR_THRESHOLD) {
            lr_result = 1;
        } else if (distance_value < LR_THRESHOLD) {
            lr_result = 0;
        }
        Serial.print("Distance: ");
        Serial.println(distance_value);
        Serial.print("LR Result: ");
        Serial.println(lr_result);

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            sensorQueue.push(lr_result);  // センサデータをキューに追加
        }
        cv.notify_one();         // Bluetoothスレッドに通知
        delay(SENSING_RATE_MS);  // センサデータの取得周期
    }
}

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
    wifi_module_manager.publish(payload.c_str());  // MQTTでカウントを送信
}

void setup() {
    Serial.begin(115200);
    BT.begin(9600);
    // tof_sensor.setup();
    wifi_module_manager.setup();

    attachTimerInterrupt(set_mqtt_flag, TIMER_INTERVAL_US);
    pthread_create(&bluetooth_process, NULL, start_bluetooth_process, NULL);
    pthread_create(&distance_sensor_process, NULL, start_distance_sensor, NULL);
}

void loop() {
    // mqtt_flag = true;
    // if (mqtt_flag) {
    //     publish_mqtt_counts();  // カウントデータをMQTTで送信
    //     mqtt_flag = false;
    // }

    wifi_module_manager.subscribe();
    delay(5000);
}
