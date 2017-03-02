#include <arduino.h>

#include "RMWiFi.h"
#include "RMWiFiServer.h"

RMWiFiServer::RMWiFiServer() {
	wiFiServer = new WiFiServer(WIFI_SVR_PORTNO);
	val = 0;
	debugSerial = NULL;
	serialString = String("");
	inputString = String("");
}

void RMWiFiServer::setup() {
	int led = GPIO_LOW;

	// prepare GPIO (IO2, IO4)
	pinMode(PIN_LED_HCHECK, OUTPUT);
	pinMode(PIN_LED_CONNECTING, OUTPUT);
	digitalWrite(PIN_LED_HCHECK, GPIO_LOW);
	digitalWrite(PIN_LED_CONNECTING, GPIO_LOW);

	// prepare GPIO (IO13=GND)
	pinMode(PIN_SWSERIAL_GND, INPUT);

	// Connect to WiFi network
	WiFi.begin(WIFI_SSID, WIFI_PSWD);

	// Wait for connected
	while (WiFi.status() != WL_CONNECTED) {
		led = (led == GPIO_LOW) ? GPIO_HIGH : GPIO_LOW;
		digitalWrite(PIN_LED_CONNECTING, led);
		delay(WIFI_SVR_POLLING_INTMS);
	}

	// Begin server with static IPAddress
	WiFi.config(WIFI_SVR_IPADDR_FN, WiFi.gatewayIP(), WiFi.subnetMask());
	wiFiServer->begin();

	// prepare UART_RX/TX
	Serial.begin(BAUD_RATE);
	delay(WIFI_SVR_BEGIN_INTMS);

	// prepare UART TX for debug
	if (digitalRead(PIN_SWSERIAL_GND) == GPIO_LOW) {
		debugSerial = new SoftwareSerial(PIN_SWSERIAL_RX, PIN_SWSERIAL_TX);
		debugSerial->begin(BAUD_RATE);
		delay(WIFI_SVR_BEGIN_INTMS);
	}

	// end setup
	digitalWrite(PIN_LED_CONNECTING, GPIO_HIGH);
}

String RMWiFiServer::readRequest(WiFiClient *client) {
	// Wait until the client sends some data
	while (!client->available()) {
		delay(1);
	}

	// Read the first line of the request
	// Abondon the rest of the request
	String req = client->readStringUntil('\r');
	client->flush();
	return req;
}

bool RMWiFiServer::processHealthCheck(WiFiClient *client, String req) {
	if (req.indexOf("/gpio/0") != -1) {
		val = GPIO_LOW;
		digitalWrite(PIN_LED_HCHECK, val);
	} else if (req.indexOf("/gpio/1") != -1) {
		val = GPIO_HIGH;
		digitalWrite(PIN_LED_HCHECK, val);
	} else {
		return false;
	}

	// Prepare the response
	String s = "HTTP/1.1 200 OK\r\n";
	s += "Content-Type: text/html\r\n\r\n";
	s += "<!DOCTYPE HTML>\r\n";
	s += "<html>\r\n";
	s += "GPIO is now ";
	s += (val == GPIO_HIGH) ? "high" : "low";
	s += "</html>\n";

	// Send the response to the client
	client->print(s);
	delay(1);
	return true;
}

void RMWiFiServer::debugPrint(String str) {
	if (debugSerial == NULL) {
		return;
	}
	debugSerial->println(str);
}

bool RMWiFiServer::processQuery(WiFiClient *client, String req) {
	String urlQuery = String(WIFI_URL_QUERY);
	int idx = req.indexOf(urlQuery);
	if (idx == -1) {
		return false;
	}

	// Send to UART_TX
	String data = req.substring(idx + urlQuery.length());
	if (0 < data.length()) {
		Serial.println(data);
		debugPrint("RMWiFiServer::processQuery: TX[" + data + "]");
	}

	// Received string from UART_RX
	String res = String(serialString + "\r\n");
	serialString = String("");

	// Send the response to the client
	client->print(res);
	delay(1);
	return true;
}

void RMWiFiServer::getStringFromSerial() {
	if (Serial.available() == 0) {
		return;
	}

	char inChar = (char)Serial.read();
	if (inChar == '\n' || inChar == '\r') {
		if (0 < inputString.length()) {
			serialString = String(inputString);
			inputString = String("");
			debugPrint("RMWiFiServer::getStringFromSerial: RX[" + serialString + "]");
		}
	} else if (31 < inChar && inChar < 126) {
		inputString += inChar;
	}
}

void RMWiFiServer::doProcess() {
	// Get string from RX
	getStringFromSerial();

	// Check if a client has connected
	WiFiClient client = wiFiServer->available();
	if (client) {
		// Read the first line of the request
		// Abondon the rest of the request
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
