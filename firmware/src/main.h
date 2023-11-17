#include <SPI.h>
#include <Wire.h>
#include <NativeEthernet.h>
#include <INA226.h>

bool initializeDHCP();
void checkDHCPStatus();
void initializeSensors();
bool initializeCurrentSensor(INA226 *sensor);
