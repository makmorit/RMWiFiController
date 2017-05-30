#include <arduino.h>

#include "WiFiInfo.h"
#include "WiFiHTTPBoard.h"
#include "WiFiServerProcess.h"

WiFiServerProcess::WiFiServerProcess() {
    wiFiServer = new WiFiServer(WIFI_SVR_PORTNO);
    val = 0;
    swSerial = NULL;
    // UART_RXから受信した入力文字列を格納
    serialString = String("");
    // UART_RXからの文字列受信が完了するまでの一時バッファ
    inputString = String("");
}

void WiFiServerProcess::setup(SoftwareSerial *ss) {
    // ソフトウェアシリアルの参照を保持
    swSerial = ss;

    // サーバー起動済みインジケーターを消灯
    digitalWrite(PIN_LED_CONNECTING, GPIO_LOW);

    // ソフトアクセスポイントを設定／起動
    WiFi.begin();
    WiFi.softAP(WIFI_SSID, WIFI_PSWD);
    WiFi.softAPConfig(WIFI_SVR_IPADDR_FN, WiFi.gatewayIP(), WiFi.subnetMask());
    IPAddress myIP = WiFi.softAPIP();
    delay(WIFI_SVR_BEGIN_INTMS);

    // HTTPサーバーの起動
    wiFiServer->begin();
    delay(WIFI_SVR_BEGIN_INTMS);

    // サーバー起動済みインジケーターを点灯
    digitalWrite(PIN_LED_CONNECTING, GPIO_HIGH);

    // ハードウェアUARTに起動情報をプリント
    Serial.println();
    Serial.print("WiFiServerProcess::setup: Access point configured: IP address=");
    Serial.println(myIP);
    Serial.print("WiFiServerProcess::setup: HTTP server started: Port no=");
    Serial.println(WIFI_SVR_PORTNO);
}

String WiFiServerProcess::readRequest(WiFiClient *client) {
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

bool WiFiServerProcess::processHealthCheck(WiFiClient *client, String req) {
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

void WiFiServerProcess::debugPrint(String str) {
    Serial.println(str);
}

bool WiFiServerProcess::processQuery(WiFiClient *client, String req) {
    // 子機から受けたリクエスト文字列に 
    // WIFI_URL_QUERY が含まれていなければ終了
    String urlQuery = String(WIFI_URL_QUERY);
    int idx = req.indexOf(urlQuery);
    if (idx == -1) {
        return false;
    }

    // UART_TXへ文字列を送信
    String data = req.substring(idx + urlQuery.length());
    if (0 < data.length()) {
        swSerial->println(data);
        debugPrint("WiFiServerProcess::processQuery: TX[" + data + "]");
    }

    // UART_RXから受信した文字列
    // (serialString)を子機へ送信
    //   送信後は、serialString をブランクにしておく
    String res = String(serialString + "\r\n");
    serialString = String("");
    client->print(res);
    delay(1);
    return true;
}

void WiFiServerProcess::getStringFromSerial() {
    // 文字入力がない場合は終了
    if (swSerial->available() == 0) {
        return;
    }

    char inChar = (char)swSerial->read();
    if (inChar == '\n' || inChar == '\r') {
        // 復帰改行文字の受信で入力終了
        if (0 < inputString.length()) {
            serialString = String(inputString);
            inputString = String("");
            debugPrint("WiFiServerProcess::getStringFromSerial: RX[" + serialString + "]");
        }
    } else if (31 < inChar && inChar < 126) {
        // 表示可能文字を受信したら、入力文字列バッファに連結
        inputString += inChar;
    }
}

void WiFiServerProcess::doProcess() {
    // UART_RXから文字を受信
    getStringFromSerial();

    WiFiClient client = wiFiServer->available();
    if (client) {
        // 子機と接続がある場合は
        // 子機からのリクエストを受信
        // （先頭行だけが読み込まれる）
        String req = readRequest(&client);
        if (processQuery(&client, req) == true) {
            return;
        }
        if (processHealthCheck(&client, req) == true) {
            return;
        }
        client.stop();
    }
}
