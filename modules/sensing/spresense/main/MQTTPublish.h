
#include <MqttGs2200.h>
#include <TelitWiFi.h>
#include <EltresAddonBoard.h>
#include <stdio.h>
#include "config.h"


#define  CONSOLE_BAUDRATE  115200

class MQTTPublish
{
private:
	TelitWiFi gs2200;
	TWIFI_Params gsparams;
	// MqttGs2200 theMqttGs2200(&gs2200);
	MqttGs2200 theMqttGs2200 = MqttGs2200(&gs2200);

	bool served = false;
	uint16_t len, count=0;
	char* send_message;
	MQTTGS2200_Mqtt mqtt;
public:
	MQTTPublish(/* args */);
	// ~MQTTPublish();
	void setup();
	void send(char* send_message);
};

