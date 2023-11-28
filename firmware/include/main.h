#include <SD.h>
#include <QNEthernet.h>
#include <INA226.h>

using namespace qindesign::network;

bool initializeCurrentSensor(INA226 *sensor);
void testLEDColors();
void colorWipe(int color, int wait);
File getFileContents(char *fileName);