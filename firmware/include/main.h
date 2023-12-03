#include <SD.h>
#include <QNEthernet.h>
#include <INA226.h>

#include "console.h"

// Temperature Sensor Values
struct SensorValues {
    float temperatureTop;
    float temperatureCenter;
    float temperatureBottom;

    float currentLine1;
    float currentLine2;
    float currentLine3;
};

File getFileContents(String fileName);
INA226 getCurrentSensor(int id);
SensorValues *getSensorValues();
