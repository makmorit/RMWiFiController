#include <arduino.h>

#include "WiFiInfo.h"
#include "RMWiFi.h"
#include "RMWiFiClient.h"

RMWiFiClient::RMWiFiClient() {
	host = WIFI_SVR_IPADDR;
	port = WIFI_SVR_PORTNO;
	debugSerial = NULL;
	serialString = String("");
	inputString = String("");
	previous_ms = 0;
}

void RMWiFiClient::setup() {
	int led = GPIO_LOW;

	// prepare GPIO (IO2, IO4)
	pinMode(PIN_LED_HCHECK, OUTPUT);
	pinMode(PIN_LED_CONNECTING, OUTPUT);
	digitalWrite(PIN_LED_HCHECK, GPIO_LOW);
	digitalWrite(PIN_LED_CONNECTING, GPIO_LOW);

	// prepare GPIO (IO13=GND)
	pinMode(PIN_SWSERIAL_GND, INPUT);

	// Connect to WiFi network
	WiFiMulti.addAP(WIFI_SSID, WIFI_PSWD);
	while (WiFiMulti.run() != WL_CONNECTED) {
		led = (led == GPIO_LOW) ? GPIO_HIGH : GPIO_LOW;
		digitalWrite(PIN_LED_CONNECTING, led);
		delay(WIFI_CLI_POLLING_INTMS);
	}

	// prepare UART_RX/TX
	Serial.begin(BAUD_RATE);
	delay(WIFI_CLI_BEGIN_INTMS);

	// prepare UART TX for debug
	if (digitalRead(PIN_SWSERIAL_GND) == GPIO_LOW) {
		debugSerial = new SoftwareSerial(PIN_SWSERIAL_RX, PIN_SWSERIAL_TX);
		debugSerial->begin(BAUD_RATE);
		delay(WIFI_CLI_BEGIN_INTMS);
	}

	// end setup
	digitalWrite(PIN_LED_CONNECTING, GPIO_HIGH);
	debugPrint("RMWiFiClient::setup completed");
}

void RMWiFiClient::debugPrint(String str) {
	if (debugSerial == NULL) {
		return;
	}
	debugSerial->println(str);
}

void RMWiFiClient::getStringFromSerial() {
	if (Serial.available() == 0) {
		return;
	}

	char inChar = (char)Serial.read();
	if (inChar == '\n' || inChar == '\r') {
		if (0 < inputString.length()) {
			serialString = String(inputString);
			inputString = String("");
			debugPrint("RMWiFiClient::getStringFromSerial: RX[" + serialString + "]");
		}
	} else if (31 < inChar && inChar < 126) {
		inputString += inChar;
	}
}

bool RMWiFiClient::connectToServer(WiFiClient *client) {
	if (!client->connect(host, port)) {
		debugPrint("RMWiFiClient::connectToServer: connection failed");
		return false;
	}
	return true;
}

void RMWiFiClient::processQuery(WiFiClient *client) {
	// サーバーに対して、RXから受領ずみのデータを送信
	//   ex. /query/<送信データ>
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

	// 受領レスポンスがヘルスチェック文字列の場合
	// 文字列に応じてヘルスチェック用LEDを点灯／消灯
	if (res == "/hchk/1") {
		digitalWrite(PIN_LED_HCHECK, GPIO_HIGH);
		debugPrint("RMWiFiClient::processQuery: Health check LED on");
		return;
	} else if (res == "/hchk/0") {
		digitalWrite(PIN_LED_HCHECK, GPIO_LOW);
		debugPrint("RMWiFiClient::processQuery: Health check LED off");
		return;
	}

	// TXにデータを転送する
	//   ex. /datasend/D11
	Serial.println(res);
	debugPrint("RMWiFiClient::processQuery: TX[" + res + "]");
}

void RMWiFiClient::process_on_quarter_second() {
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

void RMWiFiClient::doProcess() {
	// RXからデータを受領
	getStringFromSerial();

	// 周期的に処理を行う
	current_ms = millis();
	process_on_quarter_second();
}
