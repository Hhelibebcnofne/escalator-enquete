// VL53L0X用のライブラリのインクルード
#include <VL53L0X.h>
#include "define.h"//#includeで呼び出し
#include "measure_distance.h"
#include "try_recovering_sensor.h"
#include "sensor_soft_reset.h"
/**
 * @brief setup()関数
 */
void setup() {
  // シリアルモニタ出力設定
  Serial.begin(115200);

  #include "LED.h"
  #include "syokisettei.h"

  bool sensor_initialized = distance_sensor.init();
  if (sensor_initialized == false) {
    int8_t is_recovered = try_recovering_sensor();
    if (is_recovered == 0) {
      // 再初期化処理
      sensor_soft_reset();
      sensor_initialized = distance_sensor.init();
    }

    if (sensor_initialized == false) {
      // センサエラー
      digitalWrite(LED_ERR, HIGH);//LED3点灯
      program_sts = PROGRAM_STS_STOPPED;
      Serial.println("cannnot start a distance sensor.");
      return;
    }
  }

  // 測定開始
  distance_sensor.startContinuous();

  // 正常
  program_sts = PROGRAM_STS_RUNNING;
}



/**
 * @brief loop()関数
 */
void loop() {
  switch (program_sts){
    case PROGRAM_STS_RUNNING:
    // プログラム内部状態：起動中
    measure_distance();//距離測定、最新値更新
    break;
    
    case PROGRAM_STS_STOPPED:
    // プログラム内部状態：終了
    break;
    }

  // 次のループ処理まで100ミリ秒待機
  delay(100);
}

/**
 * @brief イベント通知受信コールバック
 * @param event イベント種別
 */

/**
 * @brief 気圧圧力照度距離ペイロード設定（距離のみ利用）
 * @param distance 距離
 */

/**
 * @brief ToF距離センサから対象物との距離を取得し、最新値を更新
 */
void measure_distance() {  //距離を制御。距離を取得
  uint16_t distance;
  // 距離センサから測定値を取得
  distance = distance_sensor.readRangeContinuousMillimeters();
  // 最新値の更新
  last_distance = distance;  //値を出力

  // 最新値をシリアルモニタへ出力
  Serial.print("[measure]distance: ");
  Serial.print(last_distance);
  Serial.println(" mm");
}




/**
 * @brief センサ復旧処理
 * @retval 0 成功
 * @retval -1 失敗
 */
int8_t try_recovering_sensor() {
  bool is_i2c_locked = false;
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
void sensor_soft_reset(void) {
  // set rest bit
  distance_sensor.writeReg(VL53L0X::regAddr::SOFT_RESET_GO2_SOFT_RESET_N, 0x00);
  if (distance_sensor.last_status != 0) {
    Serial.println("[sensor_soft_reset]maybe bus busy.");
  }
  uint8_t value;
  uint64_t start_millis;
  start_millis = millis();
  do {
    value = distance_sensor.readReg(VL53L0X::regAddr::IDENTIFICATION_MODEL_ID);
    if (millis() - start_millis > 2 * 1000) {
      Serial.println("[sensor_soft_reset]error timeup.");
      break;
    }
  } while (value != 0);

  // release reset bit
  distance_sensor.writeReg(VL53L0X::regAddr::SOFT_RESET_GO2_SOFT_RESET_N, 0x01);
  start_millis = millis();
  do {
    value = distance_sensor.readReg(VL53L0X::regAddr::IDENTIFICATION_MODEL_ID);
    if (millis() - start_millis > 2 * 1000) {
      Serial.println("[sensor_soft_reset]error timeup.");
      break;
    }
  } while (value == 0);
}