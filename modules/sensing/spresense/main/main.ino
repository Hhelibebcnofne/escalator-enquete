#include <SoftwareSerial.h>
#include <mutex>
#include <queue>
#include "MQTTPublish.h"
#include "ToF_Sensor.h"

#define BT_RX_PIN PIN_D00
#define BT_TX_PIN PIN_D01

#define LR_INVERSION false
#define LR_THRESHOLD 200

SoftwareSerial BT(BT_TX_PIN, BT_RX_PIN);

ToF_Sensor tof_sensor;
MQTTPublish mqttpublish;

std::queue<int> sensorQueue;
std::mutex queueMutex;

pthread_t distance_sensor_process, bluetooth_process;

void setup() {
    Serial.begin(115200);
    BT.begin(9600);
    tof_sensor.setup();
    // mqttpublish.setup();

    pthread_create(&distance_sensor_process, NULL, start_distance_sensor, NULL);
}

void start_bluetooth_process() {
    String message;
    int lr_result = -1;
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (!sensorQueue.empty()) {
            lr_result = sensorQueue.front();
            sensorQueue.pop();
        }
    }
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

    BT.println(message);
    Serial.println(message);
}

void start_distance_sensor() {
    /*
    センサからの入力値が閾値を超えてるか超えてないかを判定する関数
    センサからの入力値がLR_THRESHOLD以下なら「０」以上なら「１」がLR_result格納される
    */
    int counter = 0;
    uint16_t distance_value;
    int lr_result = -1;

    while (true) {
        // try {
        distance_value = tof_sensor.get_distance();
        if (distance_value > LR_THRESHOLD) {
            lr_result = 1;
        } else if (distance_value < LR_THRESHOLD) {
            lr_result = 0;
        } else {
            lr_result = -1;
        }
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            sensorQueue.push(lr_result);
        }
        // Serial.println(lr_result);
        // } catch(...) {
        // lr_result = -1;
        // }
    }
}

void loop() {
    pthread_create(&bluetooth_process, NULL, start_bluetooth_process, NULL);
    delay(1000);
}
