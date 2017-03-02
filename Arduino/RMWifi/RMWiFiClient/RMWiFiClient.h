#ifndef __RMWIFICLIENT_H
#define __RMWIFICLIENT_H

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <SoftwareSerial.h>

class RMWiFiClient {
public:
	RMWiFiClient();
	void setup();
	void doProcess();
private:
	ESP8266WiFiMulti WiFiMulti;
	SoftwareSerial *debugSerial;
	const char *host;
	uint16_t port;
	String inputString;
	String serialString;
	unsigned long previous_ms;
	unsigned long current_ms;

	void debugPrint(String str);
	void getStringFromSerial();
	bool connectToServer(WiFiClient *client);
	void processQuery(WiFiClient *client);
	void process_on_quarter_second();
};

#endif // __RMWIFICLIENT_H
