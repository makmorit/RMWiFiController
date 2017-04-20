#include <arduino.h>

#include "WiFiInfo.h"
#include "WiFiHTTPBoard.h"
#include "WiFiClientProcess.h"

WiFiClientProcess::WiFiClientProcess() {
    host = WIFI_SVR_IPADDR;
    port = WIFI_SVR_PORTNO;
    swSerial = NULL;
    serialString = String("");
    inputString = String("");
    previous_ms = 0;
}

void WiFiClientProcess::setup(SoftwareSerial *ss) {
    int led = GPIO_LOW;

    // Connect to WiFi network
    WiFiMulti.addAP(WIFI_SSID, WIFI_PSWD);
    while (WiFiMulti.run() != WL_CONNECTED) {
        led = (led == GPIO_LOW) ? GPIO_HIGH : GPIO_LOW;
        digitalWrite(PIN_LED_CONNECTING, led);
        delay(WIFI_CLI_POLLING_INTMS);
    }

    // prepare UART TX for debug
    swSerial = ss;

    // end setup
    digitalWrite(PIN_LED_CONNECTING, GPIO_HIGH);
    debugPrint("WiFiClientProcess::setup completed");
}

void WiFiClientProcess::debugPrint(String str) {
    Serial.println(str);
}

void WiFiClientProcess::getStringFromSerial() {
    if (swSerial->available() == 0) {
        return;
    }

    char inChar = (char)swSerial->read();
    if (inChar == '\n' || inChar == '\r') {
        if (0 < inputString.length()) {
            serialString = String(inputString);
            inputString = String("");
            debugPrint("WiFiClientProcess::getStringFromSerial: RX[" + serialString + "]");
        }
    } else if (31 < inChar && inChar < 126) {
        inputString += inChar;
    }
}

bool WiFiClientProcess::connectToServer(WiFiClient *client) {
    if (!client->connect(host, port)) {
        debugPrint("WiFiClientProcess::connectToServer: connection failed");
        return false;
    }
    return true;
}

void WiFiClientProcess::processQuery(WiFiClient *client) {
    // RXから取得したデータを、サーバーへ送信
    // 形式：/query/<送信データ>
    String urlQuery = String(WIFI_URL_QUERY) + serialString;
    serialString = String("");
    client->print(urlQuery + "\r\n");

    // レスポンスを受領
    // 複数行ある場合、１行目以降のデータは捨てられます
    String res = client->readStringUntil('\r');
    client->stop();
    delay(1);

    // 受領レスポンスが空文字列の場合は終了
    if (res.length() == 0) {
        return;
    }

    // TXにデータを転送する
    swSerial->println(res);
    debugPrint("WiFiClientProcess::processQuery: TX[" + res + "]");
}

void WiFiClientProcess::process_on_quarter_second() {
    // 前回処理から250秒経過していない場合は無視
    if (current_ms - previous_ms < 250) {
        return;
    }
    previous_ms = current_ms;

    // サーバーに接続
    WiFiClient client;
    if (connectToServer(&client) == false) {
        return;
    }

    // サーバーに対してデータを送信し、
    // サーバーからの送信データを受領
    processQuery(&client);
}

void WiFiClientProcess::doProcess() {
    // RXからデータを受領
    getStringFromSerial();

    // 周期的に処理を行う
    current_ms = millis();
    process_on_quarter_second();
}
