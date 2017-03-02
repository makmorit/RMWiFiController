#ifndef __RMWIFISERVER_H
#define __RMWIFISERVER_H

#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

#define PIN_LED_HCHECK 2
#define PIN_LED_CONNECTING 4
#define PIN_SWSERIAL_RX 14
#define PIN_SWSERIAL_TX 12
#define PIN_SWSERIAL_GND 13
#define GPIO_HIGH 1
#define GPIO_LOW 0

class RMWiFiServer {
public:
	RMWiFiServer();
	void setup();
	void doProcess();
private:
	WiFiServer *wiFiServer;
	SoftwareSerial *debugSerial;
	int val;
	String inputString;
	String serialString;
	String readRequest(WiFiClient *client);
	void getStringFromSerial();
	void debugPrint(String str);
	bool processQuery(WiFiClient *client, String req);
	bool processHealthCheck(WiFiClient *client, String req);
};

#endif // __RMWIFISERVER_H
