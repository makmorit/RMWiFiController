#include "RMWiFiClient.h"

RMWiFiClient wiFiClient;

void setup() {
	wiFiClient.setup();
}

void loop() {
	wiFiClient.doProcess();
}
