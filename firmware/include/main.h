#include <SD.h>
#include <QNEthernet.h>
#include <INA226.h>

#include "console.h"

// Temperature Sensor Values
struct SensorValues {
    float temperatureTop;
    float temperatureCenter;
    float temperatureBottom;
};

bool initializeCurrentSensor(INA226 *sensor);
File getFileContents(String fileName);
SensorValues* getSensorValues();