#include <SPI.h>
#include <Wire.h>
#include <INA226.h>
#include <Artnet.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <OctoWS2811.h>

#pragma region Network

bool initializeDHCP();
void checkDHCPStatus();

#pragma endregion

#pragma region Sensors

void initializeSensors();
bool initializeCurrentSensor(INA226 *sensor);

#pragma endregion

#pragma region LED Control

Artnet initializeLEDs(byte mac[4], byte ipAddress[4], byte broadcast[4]);
void testLEDColors();
void onArtnetReadTimer();
void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data, IPAddress remoteIP);

#pragma endregion