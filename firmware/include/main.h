#include <SPI.h>
#include <Wire.h>
#include <INA226.h>
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

void testLEDColors();

#pragma endregion