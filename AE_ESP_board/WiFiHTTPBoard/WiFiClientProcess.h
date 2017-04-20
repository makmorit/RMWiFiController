#ifndef __WIFI_CLIENT_PROCESS_H
#define __WIFI_CLIENT_PROCESS_H

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <SoftwareSerial.h>

class WiFiClientProcess {
public:
    WiFiClientProcess();
    void setup(SoftwareSerial *ss);
    void doProcess();
private:
    ESP8266WiFiMulti WiFiMulti;
    SoftwareSerial *swSerial;
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

#endif // __WIFI_CLIENT_PROCESS_H
