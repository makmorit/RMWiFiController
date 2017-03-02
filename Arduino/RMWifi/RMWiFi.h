#ifndef __RMWIFI_H
#define __RMWIFI_H

#define BAUD_RATE	19200

#define WIFI_SSID	"xxxxxxxx"
#define WIFI_PSWD	"xxxxxxxx"

#define WIFI_SVR_IPADDR_FN	(IPAddress(192,168,xxx,101))
#define WIFI_SVR_IPADDR	"192.168.xxx.101"
#define WIFI_SVR_PORTNO	80
#define WIFI_SVR_ACC_INTMS	5000
#define WIFI_SVR_ACC_TMOUT	1000

#define WIFI_SVR_BEGIN_INTMS	250
#define WIFI_SVR_POLLING_INTMS	500

#define WIFI_CLI_BEGIN_INTMS	250
#define WIFI_CLI_POLLING_INTMS	500

#define WIFI_URL_QUERY "/query/"
#define WIFI_URL_GPIO	"/gpio/"

#define PIN_LED_HCHECK 2
#define PIN_LED_CONNECTING 4
#define PIN_SWSERIAL_RX 14
#define PIN_SWSERIAL_TX 12
#define PIN_SWSERIAL_GND 13

#define GPIO_HIGH 1
#define GPIO_LOW 0

#endif // __RMWIFI_H
