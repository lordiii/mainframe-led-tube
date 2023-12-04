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
#include <Adafruit_ST7735.h>

// Create Current Sensor Objects
INA226 currentSensor1(CURRENT_SENSOR_1_ADDRESS);
INA226 currentSensor2(CURRENT_SENSOR_2_ADDRESS);
INA226 currentSensor3(CURRENT_SENSOR_3_ADDRESS);

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
unsigned long taskReadCurrent = 0;

// Sensor Values
SensorValues *sensorValues;

// Screen
#define RST 40
#define CS1 35
#define CS2 36
Adafruit_ST7735 tft = Adafruit_ST7735(&SPI1, CS1, CS2, RST); // SPI, CS, DC, RST

void setup()
{
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);

    Serial.begin(115200);

    tft.initR(INITR_GREENTAB);

    pinMode(PIN_PW_ON, OUTPUT);
    pinMode(PIN_PS_GOOD, INPUT);

    digitalWrite(PIN_PW_ON, LOW);

    qindesign::network::Ethernet.begin();

    taskRenderLeds.begin(renderFrame, 10000);

    sensorValues = (SensorValues *)malloc(sizeof(SensorValues));
    sensorValues->temperatureTop = 0.0f;
    sensorValues->temperatureCenter = 0.0f;
    sensorValues->temperatureBottom = 0.0f;

    sensorValues->currentLine1 = 0.0f;
    sensorValues->currentLine2 = 0.0f;
    sensorValues->currentLine3 = 0.0f;

    Wire.begin();
    currentSensor1.begin();
    currentSensor1.setMaxCurrentShunt(10, 0.006);
    currentSensor1.setAverage(2);

    currentSensor2.begin();
    currentSensor2.setMaxCurrentShunt(10, 0.006);
    currentSensor2.setAverage(2);

    currentSensor3.begin();
    currentSensor3.setMaxCurrentShunt(10, 0.006);
    currentSensor3.setAverage(2);

    initConsole();
    initWebServer();
    initOctoWS2811();

    registerEffect("test-led", &effectTestLEDs);
    registerEffect("test-segment", &effectSegmentTest);
    registerEffect("strobe", &effectStrobe);
    registerEffect("rainbow-strobe", &effectRainbowStrobe);
    registerEffect("police", &effectPolice);
    registerEffect("solid-white", &effectSolidWhite);
    registerEffect("beam", &effectBeam);
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
    }

    if ((time - taskRenderScreen) > 100)
    {
        tft.fillScreen(ST7735_BLACK);

        tft.setCursor(0, 0);
        tft.setTextColor(ST7735_WHITE);
        tft.setTextWrap(true);

        tft.print("Top: ");
        tft.print(sensorValues->temperatureTop);
        tft.println("°C");

        tft.print("Center: ");
        tft.print(sensorValues->temperatureCenter);
        tft.println("°C");

        tft.print("Bottom: ");
        tft.print(sensorValues->temperatureBottom);
        tft.println("°C");

        tft.println();
        tft.println();

        tft.print("Top: ");
        tft.print(sensorValues->currentLine1, 4);
        tft.println("A");

        tft.print("Center: ");
        tft.print(sensorValues->currentLine2, 4);
        tft.println("A");

        tft.print("Bottom: ");
        tft.print(sensorValues->currentLine3, 4);
        tft.println("A");

        taskRenderScreen = millis();
    }

    if (
        ((time - taskActivityLed) > 125 && !qindesign::network::Ethernet.linkState()) || ((time - taskActivityLed) > 250 && qindesign::network::Ethernet.localIP()[0] == 0) || (time - taskActivityLed > 500))
    {
        taskActivityLed = time;

        digitalWrite(13, activityLedState ? LOW : HIGH);
        activityLedState = !activityLedState;
    }

    if ((time - taskReadCurrent) > 100)
    {
        sensorValues->currentLine1 = currentSensor1.getCurrent();
        sensorValues->currentLine2 = currentSensor2.getCurrent();
        sensorValues->currentLine3 = currentSensor3.getCurrent();

        taskReadCurrent = 0;
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

INA226 getCurrentSensor(int id)
{
    switch (id)
    {
    case 1:
        return currentSensor1;
    case 2:
        return currentSensor2;
    case 3:
    default:
        return currentSensor3;
    }
}