#ifndef __WIFI_SERVER_PROCESS_H
#define __WIFI_SERVER_PROCESS_H

#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

class WiFiServerProcess {
public:
    WiFiServerProcess();
    void setup(SoftwareSerial *ss);
    void doProcess();
private:
    WiFiServer *wiFiServer;
    SoftwareSerial *swSerial;
    int val;
    String inputString;
    String serialString;
    String readRequest(WiFiClient *client);
    void getStringFromSerial();
    void debugPrint(String str);
    bool processQuery(WiFiClient *client, String req);
    bool processHealthCheck(WiFiClient *client, String req);
};

#endif // __WIFI_SERVER_PROCESS_H
