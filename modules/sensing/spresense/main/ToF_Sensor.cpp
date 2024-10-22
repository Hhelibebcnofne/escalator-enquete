#include "ToF_Sensor.h"

ToF_Sensor::ToF_Sensor() {
    VL53L0X distance_sensor;
}

/**
 * @brief setup()関数
 */
void ToF_Sensor::setup() {
    // シリアルモニタ出力設定
    Serial.begin(115200);

    bool sensor_initialized = distance_sensor.init();
    if (sensor_initialized == false) {
        // int8_t is_recovered = try_recovering_sensor();
        if (try_recovering_sensor() == 0) {
            // 再初期化処理
            sensor_soft_reset();
            sensor_initialized = distance_sensor.init();
        }

        if (sensor_initialized == false) {
            // センサエラー
            Serial.println("cannnot start a distance sensor.");
            return;
        }
    }

    // 測定開始
    distance_sensor.startContinuous();
}

/**
 * @brief loop()関数
 */
uint16_t ToF_Sensor::get_distance() {
    // uint16_t distance;
    // 距離センサから測定値を取得
    // distance = distance_sensor.readRangeContinuousMillimeters();
    return distance_sensor.readRangeContinuousMillimeters();
}

/**
 * @brief ToF距離センサから対象物との距離を取得し、最新値を更新
 */
void ToF_Sensor::measure_distance() {  // 距離を制御。距離を取得
    uint16_t distance;
    // 距離センサから測定値を取得
    distance = distance_sensor.readRangeContinuousMillimeters();
    // 最新値の更新
    // last_distance = distance;  //値を出力

    // 最新値をシリアルモニタへ出力
    Serial.print("[measure]distance: ");
    Serial.print(distance);
    Serial.println(" mm");
}

/**
 * @brief センサ復旧処理
 * @retval 0 成功
 * @retval -1 失敗
 */
int8_t ToF_Sensor::try_recovering_sensor() {
    // bool is_i2c_locked = false;
    // I2C異常か確認
    Wire.end();
    int i2c_sda = digitalRead(PIN_D14);
    if (i2c_sda == HIGH) {
        // I2C異常ではない場合、復旧不可のため、処理失敗
        return -1;
    }
    // I2C復旧処理
    pinMode(PIN_D14, INPUT_PULLUP);
    pinMode(PIN_D15, OUTPUT);
    for (int count = 0; count < 10; count++) {
        digitalWrite(PIN_D15, 1);
        delayMicroseconds(10);
        digitalWrite(PIN_D15, 0);
        delayMicroseconds(10);
    }
    digitalWrite(PIN_D15, 1);
    delayMicroseconds(10);
    i2c_sda = digitalRead(PIN_D14);
    if (i2c_sda == LOW) {
        // I2C異常が継続しているため、復旧失敗
        return -1;
    }

    // I2C再起動
    Wire.begin();

    return 0;
}

/**
 * @brief センサのソフトリセット処理
 */
void ToF_Sensor::sensor_soft_reset(void) {
    // set rest bit
    distance_sensor.writeReg(VL53L0X::regAddr::SOFT_RESET_GO2_SOFT_RESET_N,
                             0x00);
    if (distance_sensor.last_status != 0) {
        Serial.println("[sensor_soft_reset]maybe bus busy.");
    }
    uint8_t value;
    uint64_t start_millis;
    start_millis = millis();
    do {
        value =
            distance_sensor.readReg(VL53L0X::regAddr::IDENTIFICATION_MODEL_ID);
        if (millis() - start_millis > 2 * 1000) {
            Serial.println("[sensor_soft_reset]error timeup.");
            // break;
            return;
        }
    } while (value != 0);

    // release reset bit
    distance_sensor.writeReg(VL53L0X::regAddr::SOFT_RESET_GO2_SOFT_RESET_N,
                             0x01);
    start_millis = millis();
    do {
        value =
            distance_sensor.readReg(VL53L0X::regAddr::IDENTIFICATION_MODEL_ID);
        if (millis() - start_millis > 2 * 1000) {
            Serial.println("[sensor_soft_reset]error timeup.");
            // break;
            return;
        }
    } while (value == 0);
}