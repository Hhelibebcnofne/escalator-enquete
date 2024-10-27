#include <MqttGs2200.h>
#include <TelitWiFi.h>
#include <stdio.h>
#include "config.h"

#define CONSOLE_BAUDRATE 115200

class WiFi_Module_Manager {
   private:
    TelitWiFi gs2200;
    TWIFI_Params gsparams;
    // MqttGs2200 theMqttGs2200(&gs2200);
    MqttGs2200 theMqttGs2200 = MqttGs2200(&gs2200);

    bool publish_served = false;
    bool subscribe_served = false;
    uint16_t len, count = 0;
    char* send_message;
    MQTTGS2200_Mqtt mqtt_publish;
    MQTTGS2200_Mqtt mqtt_subscribe;

   public:
    WiFi_Module_Manager(/* args */);
    // ~MQTTPublish();
    void setup();
    void publish(char* send_message);
    void subscribe();
    void start_subscribe_process();
};
