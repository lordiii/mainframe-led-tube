#include <SD.h>
#include <QNEthernet.h>
#include <INA226.h>

// Temperature Sensor Values
struct SensorValues {
    float temperatureTop;
    float temperatureCenter;
    float temperatureBottom;
};

enum LED_EFFECT {
    OFF,
    LED_TEST,
    STROBE,
    RAINBOW_STROBE
};

bool initializeCurrentSensor(INA226 *sensor);
void renderFrame();
void setCurrentEffect(LED_EFFECT effect);
File getFileContents(char *fileName);
SensorValues* getSensorValues();