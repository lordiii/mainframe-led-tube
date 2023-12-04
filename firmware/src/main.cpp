#include "main.h"
#include "globals.h"
#include "console.h"
#include "effects.h"
#include "web.h"

#include <Wire.h>
#include <INA226.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SD.h>

// Create Current Sensor Objects
const unsigned char currentSensor1Address = CURRENT_SENSOR_1_ADDRESS;
INA226 *currentSensor1 = new INA226(currentSensor1Address);

const unsigned char currentSensor2Address = CURRENT_SENSOR_2_ADDRESS;
INA226 *currentSensor2 = new INA226(currentSensor2Address);

const unsigned char currentSensor3Address = CURRENT_SENSOR_3_ADDRESS;
INA226 *currentSensor3 = new INA226(currentSensor3Address);

OneWire oneWire(24);
DallasTemperature sensors(&oneWire);
const unsigned char tempProbeTop[8] = TEMPERATURE_SENSOR_TOP_ADDRESS;
const unsigned char tempProbeCenter[8] = TEMPERATURE_SENSOR_CENTER_ADDRESS;
const unsigned char tempProbeBottom[8] = TEMPERATURE_SENSOR_BOTTOM_ADDRESS;

// LED Render Tasks
IntervalTimer taskRenderLeds;

// Scheduled Tasks
unsigned long taskReadSensors = 0;
unsigned long taskRenderScreen = 0;
unsigned long taskActivityLed = 0;

void setup()
{
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);

    Wire.begin();
    Serial.begin(115200);

    qindesign::network::Ethernet.begin();

    taskRenderLeds.begin(renderFrame, 10000);

    sensorValues = (SensorValues *)malloc(sizeof(SensorValues));
    sensorValues->temperatureTop = 0.0f;
    sensorValues->temperatureCenter = 0.0f;
    sensorValues->temperatureBottom = 0.0f;

    sensorValues->currentLine1 = 0.0f;
    sensorValues->currentLine2 = 0.0f;
    sensorValues->currentLine3 = 0.0f;

    initializeCurrentSensor(currentSensor1);
    initializeCurrentSensor(currentSensor2);
    initializeCurrentSensor(currentSensor3);

    initConsole();
    initWebServer();
    initOctoWS2811();

    registerEffect("led-test", &effectTestLEDs);
    registerEffect("strobe", &effectStrobe);
    registerEffect("rainbow-strobe", &effectRainbowStrobe);
    registerEffect("police", &effectPolice);
    registerEffect("solid-white", &effectSolidWhite);
}

bool toggleTemperatureReadWrite = false;
bool activityLedState = false;
void loop()
{
    unsigned long time = millis();

    if ((time - taskReadSensors) > 1000)
    {
        taskReadSensors = time;

        // Read Temperature Values
        if (toggleTemperatureReadWrite)
        {
            sensors.requestTemperatures();
            toggleTemperatureReadWrite = !toggleTemperatureReadWrite;
        }
        else
        {
            sensorValues->temperatureTop = sensors.getTempC(tempProbeTop);
            sensorValues->temperatureCenter = sensors.getTempC(tempProbeCenter);
            sensorValues->temperatureBottom = sensors.getTempC(tempProbeBottom);

            toggleTemperatureReadWrite = !toggleTemperatureReadWrite;
        }

        sensorValues->currentLine1 = currentSensor1->getCurrent();
        sensorValues->currentLine2 = currentSensor2->getCurrent();
        sensorValues->currentLine3 = currentSensor3->getCurrent();
    }

    if ((time - taskRenderScreen) > 2000)
    {
        taskRenderScreen = time;
    }

    if (
        ((time - taskActivityLed) > 125 && !qindesign::network::Ethernet.linkState()) || ((time - taskActivityLed) > 250 && qindesign::network::Ethernet.localIP()[0] == 0) || (time - taskActivityLed > 500))
    {
        taskActivityLed = time;

        digitalWrite(13, activityLedState ? LOW : HIGH);
        activityLedState = !activityLedState;
    }

    processConsoleData();
    handleWebClient();
}

File getFileContents(String fileName)
{
    if (SD.begin(BUILTIN_SDCARD))
    {

        File dataFile = SD.open(fileName.c_str(), FILE_READ);

        if (dataFile)
        {
            return dataFile;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

bool initializeCurrentSensor(INA226 *sensor)
{
    if (sensor->begin())
    {
        return false;
    }

    switch (sensor->setMaxCurrentShunt(12.0F, 0.006F))
    {
    case INA226_ERR_NONE:
        return true;
    default:
        return false;
    }
}