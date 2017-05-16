#include "WiFiProcess.h"

#include "WiFiInfo.h"

WiFiProcess process;

WiFiServer server(WIFI_SVR_PORTNO);

#define PIN_LED_HCHECK 2

#define GPIO_HIGH 1
#define GPIO_LOW 0

void setup() {
    //process.setup();

    pinMode(PIN_LED_HCHECK, OUTPUT);
    digitalWrite(PIN_LED_HCHECK, GPIO_LOW);
    
    Serial.begin(115200);
    Serial.println();
    Serial.print("Configuring access point...");
    WiFi.begin();
    delay(250);

    WiFi.softAP(WIFI_SSID, WIFI_PSWD);
    WiFi.softAPConfig(WIFI_SVR_IPADDR_FN, WiFi.gatewayIP(), WiFi.subnetMask());
    delay(250);
  
    IPAddress myIP = WiFi.softAPIP();
    delay(250);
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    delay(250);

    server.begin();
    delay(500);
    
    Serial.println("HTTP server started");
}


String readRequest(WiFiClient *client) {
    // 子機からリクエストが来るまで待つ
    while (!client->available()) {
        delay(1);
    }

    // リクエストの先頭行だけを読む
    // （他の行は無視）
    String req = client->readStringUntil('\r');
    client->flush();
    return req;
}

bool processHealthCheck(WiFiClient *client, String req) {
    int val;
    // ヘルスチェックリクエストを参照し、
    // ヘルスチェック用LEDを点灯／消灯させる
    if (req.indexOf("/hcheck/0") != -1) {
        val = GPIO_LOW;
        digitalWrite(PIN_LED_HCHECK, val);
    } else if (req.indexOf("/hcheck/1") != -1) {
        val = GPIO_HIGH;
        digitalWrite(PIN_LED_HCHECK, val);
    } else {
        return false;
    }

    // レスポンス文字列（HTML）を生成
    String s = "HTTP/1.1 200 OK\r\n";
    s += "Content-Type: text/html\r\n\r\n";
    s += "<!DOCTYPE HTML>\r\n";
    s += "<html>\r\n";
    s += "Pin IO2 is now ";
    s += (val == GPIO_HIGH) ? "HIGH" : "LOW";
    s += "</html>\n";

    // ヘルスチェックリクエストの送信元に、
    // レスポンス文字列（HTML）を返送
    client->print(s);
    delay(1);
    
    return true;
}

void loop() {
    //process.doProcess();

    WiFiClient client = server.available();
    if (client) {
        String req = readRequest(&client);
        Serial.println("Your request is: " + req);
        if (processHealthCheck(&client, req) == true) {
            return;
        }
    }
}
