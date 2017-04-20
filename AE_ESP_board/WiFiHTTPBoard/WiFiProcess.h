#ifndef __WIFI_PROCESS_H
#define __WIFI_PROCESS_H

#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

#include "WiFiServerProcess.h"
#include "WiFiClientProcess.h"

class WiFiProcess {
public:
    WiFiProcess();
    void setup();
    void doProcess();
private:
    SoftwareSerial *swSerial;
    WiFiServerProcess *wiFiServerProcess;
    WiFiClientProcess *wiFiClientProcess;
    void debugPrint(String str);
};

#endif // __WIFI_PROCESS_H
