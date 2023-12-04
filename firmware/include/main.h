#include <SD.h>
#include <QNEthernet.h>
#include <INA226.h>

#include "console.h"

#define PIN_PW_ON 28
#define PIN_PS_GOOD 29

// Temperature Sensor Values
struct SensorValues {
    float temperatureTop;
    float temperatureCenter;
    float temperatureBottom;

    float currentLine1;
    float currentLine2;
    float currentLine3;
};

extern SensorValues *sensorValues;

bool initializeCurrentSensor(INA226 *sensor);
File getFileContents(String fileName);
INA226 getCurrentSensor(int id);