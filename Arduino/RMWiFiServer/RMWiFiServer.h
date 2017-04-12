#ifndef __RMWIFISERVER_H
#define __RMWIFISERVER_H

#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

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
