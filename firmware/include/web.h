#define USE_NATIVE_ETHERNET false
#define USE_QN_ETHERNET true

#define USE_UIP_ETHERNET false
#define USE_ETHERNET_GENERIC false
#define USE_ETHERNET_ESP8266 false
#define USE_ETHERNET_ENC false
#define USE_CUSTOM_ETHERNET false

#define _ETHERNET_WEBSERVER_LOGLEVEL_ 4
#define DEBUG_ETHERNET_WEBSERVER_PORT Serial

void initWebServer();
void handleWebClient();
void handleRoot();
void handleNotFound();
