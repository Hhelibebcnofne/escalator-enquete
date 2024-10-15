
#include "MQTTPublish.h"
#include <EltresAddonBoard.h>
#include <MqttGs2200.h>
#include <TelitWiFi.h>
#include <stdio.h>
#include "config.h"

MQTTPublish::MQTTPublish() {
    TelitWiFi gs2200;
    MqttGs2200 theMqttGs2200(&gs2200);
    // MqttGs2200 theMqttGs2200 = MqttGs2200(&gs2200);
    TWIFI_Params gsparams;
    bool served = false;
    uint16_t len;
    MQTTGS2200_Mqtt mqtt;
}

void MQTTPublish::setup() {
    MQTTGS2200_HostParams hostParams;
    Serial.begin(CONSOLE_BAUDRATE);  // talk to PC
    Serial.println("setup start");

    /* initialize digital pin LED_BUILTIN as an output. */
    pinMode(LED0, OUTPUT);
    digitalWrite(LED0, LOW);  // turn the LED off (LOW is the voltage level)
    /* Initialize SPI access of GS2200 */
    Init_GS2200_SPI_type(iS110B_TypeC);
    Serial.println("setup _1");

    /* Initialize AT Command Library Buffer */
    gsparams.mode = ATCMD_MODE_STATION;
    gsparams.psave = ATCMD_PSAVE_DEFAULT;
    if (gs2200.begin(gsparams)) {
        // ConsoleLog("GS2200 Initilization Fails");
        Serial.println("gs2200.begin...");
        while (1)
            ;
    }
    Serial.println("setup _2");
    Serial.println(AP_SSID);
    Serial.println(PASSPHRASE);

    /* GS2200 Association to AP */
    if (gs2200.activate_station(AP_SSID, PASSPHRASE)) {
        Serial.println("Association Fails");
        while (1)
            ;
    }
    Serial.println("setup _3");

    hostParams.host = (char*)MQTT_SRVR;
    hostParams.port = (char*)MQTT_PORT;
    hostParams.clientID = (char*)MQTT_CLI_ID;
    hostParams.userName = (char*)MQTT_USERNAME;
    hostParams.password = (char*)MQTT_PASSWPRD;

    theMqttGs2200.begin(&hostParams);
    Serial.println("setup _4");
    digitalWrite(LED0, HIGH);  // turn on LED
    Serial.println("setup end");
}

// the loop function runs over and over again forever
void MQTTPublish::send(char* send_message) {
    if (!served) {
        // Start a MQTT client
        if (false == theMqttGs2200.connect()) {
            return;
        }
        served = true;
    } else {
        Serial.println("Start to send MQTT Message");
        // Prepare for the next chunck of incoming data
        WiFi_InitESCBuffer();
        // Start the loop to send the data
        strncpy(mqtt.params.topic, MQTT_TOPIC, sizeof(mqtt.params.topic));
        mqtt.params.QoS = 0;

        strcpy(mqtt.params.message, send_message);
        snprintf(mqtt.params.message, sizeof(mqtt.params.message), "%d",
                 (int)atoi(send_message));

        mqtt.params.len = strlen(mqtt.params.message);
        if (true == theMqttGs2200.publish(&mqtt)) {
            Serial.println(send_message);
        }
    }
}
