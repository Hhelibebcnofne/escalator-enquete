#include <MqttGs2200.h>
#include <TelitWiFi.h>
#include <stdio.h>
#include "config.h"

#define SUBSCRIBE_TIMEOUT 1000

class WiFi_Module_Manager {
   private:
    TelitWiFi gs2200;
    TWIFI_Params gsparams;
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
    bool setup();
    bool initWiFi();
    bool initMQTT();

    bool mqttPublish(char* send_message);
    bool mqttSubscribe();
};
