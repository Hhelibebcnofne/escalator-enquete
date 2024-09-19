#include <EltresAddonBoard.h>

void setup() {
    Serial.begin(115200);
    Serial.println("Hello, Spresense!");
}

void loop() {
    Serial.println("Hello, Spresense!");
    delay(1000);
}
