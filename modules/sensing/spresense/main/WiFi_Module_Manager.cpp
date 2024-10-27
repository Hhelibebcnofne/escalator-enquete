#include "WiFi_Module_Manager.h"

WiFi_Module_Manager::WiFi_Module_Manager() {
    TelitWiFi gs2200;
    MqttGs2200 theMqttGs2200(&gs2200);
    // MqttGs2200 theMqttGs2200 = MqttGs2200(&gs2200);
    TWIFI_Params gsparams;
    bool publish_served = false;
    bool subscribe_served = false;
    uint16_t len;
    MQTTGS2200_Mqtt mqtt_publish;
    MQTTGS2200_Mqtt mqtt_subscribe;
}

void WiFi_Module_Manager::setup() {
    MQTTGS2200_HostParams hostParams;
    Serial.begin(CONSOLE_BAUDRATE);  // talk to PC
    Serial.println("setup start");

    /* initialize digital pin LED_BUILTIN as an output. */
    pinMode(LED0, OUTPUT);
    digitalWrite(LED0, LOW);  // turn the LED off (LOW is the voltage level)
    /* Initialize SPI access of GS2200 */
    Init_GS2200_SPI_type(iS110B_TypeC);

    /* Initialize AT Command Library Buffer */
    gsparams.mode = ATCMD_MODE_STATION;
    gsparams.psave = ATCMD_PSAVE_DEFAULT;
    if (gs2200.begin(gsparams)) {
        Serial.println("gs2200.begin...");
        while (1);
    }

    /* GS2200 Association to AP */
    if (gs2200.activate_station(AP_SSID, PASSPHRASE)) {
        Serial.println("Association Fails");
        while (1);
    }
    Serial.println("connected AP info");
    Serial.println(AP_SSID);
    Serial.println(PASSPHRASE);

    hostParams.host = (char*)MQTT_SRVR;
    hostParams.port = (char*)MQTT_PORT;
    hostParams.clientID = (char*)MQTT_CLI_ID;
    hostParams.userName = (char*)MQTT_USERNAME;
    hostParams.password = (char*)MQTT_PASSWPRD;

    theMqttGs2200.begin(&hostParams);
    Serial.println("set mqtt hostParams");
    digitalWrite(LED0, HIGH);  // turn on LED
    Serial.println("setup end");
}

// the loop function runs over and over again forever
void WiFi_Module_Manager::publish(char* send_message) {
    if (!publish_served) {
        // Start a MQTT client
        if (false == theMqttGs2200.connect()) {
            Serial.println("theMqttGs2200.connect() false");
            return;
        }
        publish_served = true;
    } else {
        Serial.println("Start to send MQTT Message");
        // Prepare for the next chunck of incoming data
        WiFi_InitESCBuffer();
        // Start the loop to send the data
        strncpy(mqtt_publish.params.topic, MQTT_PUBLISH_TOPIC, sizeof(mqtt_publish.params.topic));
        mqtt_publish.params.QoS = 0;

        strcpy(mqtt_publish.params.message, send_message);
        snprintf(mqtt_publish.params.message, sizeof(mqtt_publish.params.message), "%d",
                 (int)atoi(send_message));

        mqtt_publish.params.len = strlen(mqtt_publish.params.message);
        if (true == theMqttGs2200.publish(&mqtt_publish)) {
            Serial.println(send_message);
        }
    }
}

void WiFi_Module_Manager::start_subscribe_process() {
	if (!subscribe_served) {
		ConsoleLog( "Start MQTT Client");
		if (false == theMqttGs2200.connect()) {
			return;
		}
        strncpy(mqtt_subscribe.params.topic, MQTT_SUBSCRIBE_TOPIC , sizeof(mqtt_subscribe.params.topic));
		mqtt_subscribe.params.QoS = 0;
		mqtt_subscribe.params.retain = 0;

		if (true == theMqttGs2200.subscribe(&mqtt_subscribe)) {
			ConsolePrintf( "Subscribed! \n" );
		}
        subscribe_served = true;
	}
	else {
        WiFi_InitESCBuffer();

        String data;
        while (gs2200.available()) {
            if (theMqttGs2200.receive(data)) {
                // publish_served = false; // quite the loop
                Serial.println("Recieve data: " + data);
                break;
            } else {
                Serial.println("Recieve break");
                break;
            }
        }
	}
}

void WiFi_Module_Manager::start_subscribe() {
    pthread_create(&bluetooth_process, NULL, start_subscribe_process, NULL);
}