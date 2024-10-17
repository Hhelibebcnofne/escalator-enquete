#include <SoftwareSerial.h>
#include "MQTTPublish.h"
#include "ToF_Sensor.h"

#define BT_RX_PIN PIN_D00
#define BT_TX_PIN PIN_D01

#define LR_INVERSION false
#define LR_THRESHOLD 200

int distance_value = 100000;
// センサからの入力値がLR_threshold以下なら「０」以上なら「１」が格納される変数
int LR_result = -1;

SoftwareSerial BT(BT_TX_PIN, BT_RX_PIN);

ToF_Sensor tof_sensor;
MQTTPublish mqttpublish;

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
#if LR_INVERSION
    if (LR_result == 0) {
        message = "left_count";
    } else if (LR_result == 1) {
        message = "right_count";
    } else {
        message = "count_error";
    }
#else

    if (LR_result == 0) {
        message = "right_count";
    } else if (LR_result == 1) {
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
    tof_sensor.start_distance_sensor_process(&LR_result, LR_THRESHOLD);
}

void loop() {
    pthread_create(&bluetooth_process, NULL, start_bluetooth_process, NULL);
    delay(1000);
}
