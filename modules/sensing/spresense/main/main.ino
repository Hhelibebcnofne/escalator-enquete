#include <EltresAddonBoard.h>
#include <VL53L0X.h>
#include "ToF_Sensor.h"
#include "MQTTPublish.h"

// ToF_Sensorのインスタンスを作成
ToF_Sensor tof_sensor;
MQTTPublish mqttpublish;

void setup() {
    Serial.begin(115200);

    tof_sensor.setup();
    mqttpublish.setup();
    // Serial.println("Hello, Spresense!");
}
int LR_threshold = 200;
int distans_value = 100000;
void loop() {
    distans_value = tof_sensor.get_distance();

    if (distans_value < LR_threshold){
        mqttpublish.send("1");
    } else if (distans_value > LR_threshold && distans_value < 1000){
        mqttpublish.send("0");
    } else {
        Serial.println("人物通過待機...");
    }

    Serial.println(distans_value);

    // mqttpublish.send("1");
    // Serial.println("Hello, Spresense!");
    delay(2000);
}
