#include <SD.h>
#include <QNEthernet.h>
#include <INA226.h>

// Temperature Sensor Values
struct SensorValues {
    float temperatureTop;
    float temperatureCenter;
    float temperatureBottom;
};

bool initializeCurrentSensor(INA226 *sensor);
void testLEDColor(unsigned int color);
void colorWipe(int color, int wait);
void renderFrame();
File getFileContents(char *fileName);
SensorValues* getSensorValues();