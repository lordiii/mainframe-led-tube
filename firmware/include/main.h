#include <SD.h>
#include <QNEthernet.h>
#include <INA226.h>

enum LED_EFFECT {
    OFF,
    LED_TEST,
    STROBE,
    RAINBOW_STROBE,
    POLICE
};

// Temperature Sensor Values
struct SensorValues {
    float temperatureTop;
    float temperatureCenter;
    float temperatureBottom;
};

bool initializeCurrentSensor(INA226 *sensor);
void renderFrame();
void calculateFrame();
void setCurrentEffect(LED_EFFECT effect);
File getFileContents(char *fileName);
SensorValues* getSensorValues();