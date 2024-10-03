#include <SoftwareSerial.h>

#define BT_RX_PIN PIN_D00
#define BT_TX_PIN PIN_D01

SoftwareSerial BT(BT_TX_PIN, BT_RX_PIN);

String message;

void setup() {
  BT.begin(9600);
  Serial.begin(9600);
}

void loop() {
  // 受信処理
  if (BT.available()) {
    message = BT.readStringUntil('\n');
    message += " received";
    BT.println(message);
    Serial.println(message);
  }

  // 1秒ごとにブラウザへメッセージを送信する
  BT.println("Data from MCU");
  delay(1000);
}
