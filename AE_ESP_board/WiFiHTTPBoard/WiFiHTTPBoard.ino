#include "WiFiProcess.h"

WiFiProcess process;

void setup() {
    process.setup();
}

void loop() {
    process.doProcess();
}

