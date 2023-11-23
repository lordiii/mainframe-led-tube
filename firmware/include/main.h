#include <SPI.h>
#include <Wire.h>
#include <INA226.h>
#include <QNEthernet.h>
#include <OctoWS2811.h>

using namespace qindesign::network;

bool initializeCurrentSensor(INA226 *sensor);
void testLEDColors();

void colorWipe(int color, int wait);