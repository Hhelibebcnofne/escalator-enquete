/*
 *  SPRESENSE_WiFi.ino - GainSpan WiFi Module Control Program
 *  Copyright 2019 Norikazu Goto
 *
 *  This work is free software; you can redistribute it and/or modify it under the terms 
 *  of the GNU Lesser General Public License as published by the Free Software Foundation; 
 *  either version 2.1 of the License, or (at your option) any later version.
 *
 *  This work is distributed in the hope that it will be useful, but without any warranty; 
 *  without even the implied warranty of merchantability or fitness for a particular 
 *  purpose. See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with 
 *  this work; if not, write to the Free Software Foundation, 
 *  Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <MqttGs2200.h>
#include <TelitWiFi.h>
#include <EltresAddonBoard.h>
#include <stdio.h>
#include <stdio.h>
#include "config.h"


#define  CONSOLE_BAUDRATE  115200

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
TelitWiFi gs2200;
TWIFI_Params gsparams;
MqttGs2200 theMqttGs2200(&gs2200);

// the setup function runs once when you press reset or power the board
void setup() {
	MQTTGS2200_HostParams hostParams;
	Serial.begin( CONSOLE_BAUDRATE ); // talk to PC
  Serial.println("setup start");

	/* initialize digital pin LED_BUILTIN as an output. */
	pinMode(LED0, OUTPUT);
	digitalWrite( LED0, LOW );   // turn the LED off (LOW is the voltage level)
	/* Initialize SPI access of GS2200 */
	Init_GS2200_SPI_type(iS110B_TypeC);
  Serial.println("setup _1");

	/* Initialize AT Command Library Buffer */
	gsparams.mode = ATCMD_MODE_STATION;
	gsparams.psave = ATCMD_PSAVE_DEFAULT;
	if (gs2200.begin(gsparams)) {
		// ConsoleLog("GS2200 Initilization Fails");
    Serial.println("gs2200.begin...");
		while(1);
	}
  Serial.println("setup _2");
  Serial.println(AP_SSID);
  Serial.println(PASSPHRASE);
  
	/* GS2200 Association to AP */
	if (gs2200.activate_station(AP_SSID, PASSPHRASE)) {
    Serial.println("Association Fails");
		while(1);
	}
  Serial.println("setup _3");

	hostParams.host = (char *)MQTT_SRVR;
	hostParams.port = (char *)MQTT_PORT;
	hostParams.clientID = (char *)MQTT_CLI_ID;
	hostParams.userName = NULL;
	hostParams.password = NULL;

	theMqttGs2200.begin(&hostParams);
  Serial.println("setup _4");
	digitalWrite( LED0, HIGH ); // turn on LED
  Serial.println("setup end");
}

bool served = false;
uint16_t len, count=0;
char* send_message;
MQTTGS2200_Mqtt mqtt;

// the loop function runs over and over again forever
void loop() {
	if (!served) {
		// Start a MQTT client
		if (false == theMqttGs2200.connect()) {
			return;
		}
		served = true;
	}
	else {
		Serial.println( "Start to send MQTT Message");
		// Prepare for the next chunck of incoming data
		WiFi_InitESCBuffer();
		// Start the loop to send the data
		strncpy(mqtt.params.topic, MQTT_TOPIC , sizeof(mqtt.params.topic));
		mqtt.params.QoS = 0;

		while (served) {
      if (count%2 == 0){
        send_message = "0";
      } else {
        send_message = "1";
      }

      strcpy(mqtt.params.message, send_message);
			snprintf(mqtt.params.message, sizeof(mqtt.params.message), "%d", (int) atoi(send_message));
      count++;

			mqtt.params.len = strlen(mqtt.params.message);
			if (true == theMqttGs2200.publish(&mqtt)) {
        Serial.println(send_message);
			}

			delay(5000); // every 5 sec
		}
	}
}
