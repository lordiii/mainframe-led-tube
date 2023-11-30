#include <Wire.h>
#include <INA226.h>
#include <QNEthernet.h>
#include <OctoWS2811.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SD.h>

#include "main.h"
#include "globals.h"
#include "console.h"
#include "effects.h"
#include "web.h"

// Setup LEDs
const byte pinList[LED_STRIP_AMOUNT] = LED_PINS;

DMAMEM int displayMemory[LED_PER_STRIP * 6];
int drawingMemory[LED_PER_STRIP * 6];

const int config = LED_CONFIGURATION;
OctoWS2811 leds(LED_PER_STRIP, displayMemory, drawingMemory, config, LED_STRIP_AMOUNT, pinList);

// Create Current Sensor Objects
const unsigned char currentSensor1Address = CURRENT_SENSOR_1_ADDRESS;
INA226 *currentSensor1 = new INA226(currentSensor1Address);

const unsigned char currentSensor2Address = CURRENT_SENSOR_2_ADDRESS;
INA226 *currentSensor2 = new INA226(currentSensor2Address);

const unsigned char currentSensor3Address = CURRENT_SENSOR_3_ADDRESS;
INA226 *currentSensor3 = new INA226(currentSensor3Address);

OneWire oneWire(16);
DallasTemperature sensors(&oneWire);
const unsigned char tempProbeTop[8] = TEMPERATURE_SENSOR_TOP_ADDRESS;
const unsigned char tempProbeCenter[8] = TEMPERATURE_SENSOR_CENTER_ADDRESS;
const unsigned char tempProbeBottom[8] = TEMPERATURE_SENSOR_BOTTOM_ADDRESS;

// LED Render Tasks
IntervalTimer taskRenderLeds;

// Scheduled Tasks
unsigned long taskHandleWebRequests = 0;
unsigned long taskReadSensors = 0;
unsigned long taskRenderScreen = 0;
unsigned long taskActivityLed = 0;

// Sensor Values
SensorValues *sensorValues;

void setup()
{
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);

    Wire.begin();
    Serial.begin(115200);

    qindesign::network::Ethernet.begin();

    leds.begin();
    leds.show();

    taskRenderLeds.begin(renderFrame, 10000);

    sensorValues = (SensorValues *)malloc(sizeof(SensorValues));
    sensorValues->temperatureTop = 0.0f;
    sensorValues->temperatureCenter = 0.0f;
    sensorValues->temperatureBottom = 0.0f;

    initializeCurrentSensor(currentSensor1);
    initializeCurrentSensor(currentSensor2);
    initializeCurrentSensor(currentSensor3);

    initConsole();
    initWebServer();
}

bool toggleTemperatureReadWrite = false;
bool activityLedState = false;
void loop()
{
    unsigned long time = millis();

    if ((time - taskHandleWebRequests) > 25)
    {
        taskHandleWebRequests = time;
        handleWebClient();
    }

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
    }

    if ((time - taskRenderScreen) > 2000)
    {
        taskRenderScreen = time;
    }

    if ((time - taskActivityLed) > 500)
    {
        taskActivityLed = time;

        digitalWrite(13, activityLedState ? LOW : HIGH);
        activityLedState = !activityLedState;
    }

    processConsoleData();
}

File getFileContents(char *fileName)
{
    File dataFile = SD.open(fileName, FILE_READ);

    if (dataFile)
    {
        return dataFile;
    }
    else if (SD.begin(BUILTIN_SDCARD))
    {
        return NULL;
    }
    else
    {
        while (!SD.begin(BUILTIN_SDCARD))
        {
            Serial.println("ERROR: SD CARD NOT FOUND");
        }

        return getFileContents(fileName);
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

bool (*currentEffect)(OctoWS2811, unsigned long) = nullptr;
unsigned long lastFrameChange = 0;

void renderFrame()
{
    bool updated = false;

    if(currentEffect != nullptr) {
        unsigned long delta = millis() - lastFrameChange;
        updated = currentEffect(leds, delta);
    }

    if (updated)
    {
        lastFrameChange = millis();
    }

    leds.show();
}

void setCurrentEffect(bool (*function)(OctoWS2811,unsigned long))
{
    noInterrupts();
    currentEffect = function;
    lastFrameChange = 0;
    interrupts();
}

SensorValues *getSensorValues()
{
    return sensorValues;
}