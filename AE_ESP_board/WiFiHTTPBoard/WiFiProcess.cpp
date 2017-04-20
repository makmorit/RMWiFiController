#include <arduino.h>

#include "WiFiInfo.h"
#include "WiFiHTTPBoard.h"
#include "WiFiProcess.h"

WiFiProcess::WiFiProcess() {
    wiFiServerProcess = NULL;
    wiFiClientProcess = NULL;
}

void WiFiProcess::setup() {
    int led = GPIO_LOW;

    // prepare GPIO (IO2, IO4, IO5)
    pinMode(PIN_LED_HCHECK, OUTPUT);
    pinMode(PIN_LED_CONNECTING, OUTPUT);
    pinMode(PIN_LED_SVRCLI, OUTPUT);
    digitalWrite(PIN_LED_HCHECK, GPIO_LOW);
    digitalWrite(PIN_LED_CONNECTING, GPIO_LOW);
    digitalWrite(PIN_LED_SVRCLI, GPIO_LOW);

    // prepare GPIO (IO16)
    pinMode(PIN_SVRCLI_MODE, INPUT);

    // prepare GPIO (IO13=GND)
    pinMode(PIN_SWSERIAL_GND, INPUT);

    // prepare UART_RX/TX
    Serial.begin(BAUD_RATE);
    debugPrint("WiFiProcess::setup: Hardware UART started");

    // prepare UART TX for Software Serial
    if (digitalRead(PIN_SWSERIAL_GND) == GPIO_LOW) {
        swSerial = new SoftwareSerial(PIN_SWSERIAL_RX, PIN_SWSERIAL_TX);
        swSerial->begin(BAUD_RATE);
        delay(WIFI_SVR_BEGIN_INTMS);
        debugPrint("WiFiProcess::setup: Software UART started");
    }

    // setup phase
    if (digitalRead(PIN_SVRCLI_MODE) == GPIO_LOW) {
        digitalWrite(PIN_LED_SVRCLI, GPIO_HIGH);
        wiFiServerProcess = new WiFiServerProcess();
        wiFiClientProcess = NULL;
        debugPrint("WiFiProcess::setup: This device works as HTTP server");

        wiFiServerProcess->setup(swSerial);

    } else {
        digitalWrite(PIN_LED_SVRCLI, GPIO_LOW);
        wiFiServerProcess = NULL;
        wiFiClientProcess = new WiFiClientProcess();
        debugPrint("WiFiProcess::setup: This device works as HTTP client");

        wiFiClientProcess->setup(swSerial);
    }
}

void WiFiProcess::debugPrint(String str) {
    Serial.println(str);
}

void WiFiProcess::doProcess() {
    if (wiFiServerProcess) {
        wiFiServerProcess->doProcess();
    }
    if (wiFiClientProcess) {
        wiFiClientProcess->doProcess();
    }
}
