#include "RMWiFiServer.h"

RMWiFiServer wiFiServer;

void setup() {
	wiFiServer.setup();
}

void loop() {
	wiFiServer.doProcess();
}
