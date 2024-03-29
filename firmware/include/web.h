#ifndef FIRMWARE_WEB_H
#define FIRMWARE_WEB_H

#define USE_NATIVE_ETHERNET false
#define USE_QN_ETHERNET true

#define USE_UIP_ETHERNET false
#define USE_ETHERNET_GENERIC false
#define USE_ETHERNET_ESP8266 false
#define USE_ETHERNET_ENC false
#define USE_CUSTOM_ETHERNET false

#define _ETHERNET_WEBSERVER_LOGLEVEL_ 0
#define DEBUG_ETHERNET_WEBSERVER_PORT Serial

#include <Arduino.h>
#include <SD.h>

void initWebServer();

void handleWebClient();

bool sendFile(const char *fileName);

const char *getContentType(const char *filename);

void sendLEDs();

File getFileContents(const char *fileName);

bool strEndsWith(const char *str, const char *suffix);

#endif