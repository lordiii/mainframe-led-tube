#include <main.h>
#include <globals.h>
#include <effects.h>
#include <web.h>
#include <console.h>

#include <Wire.h>
#include <INA226.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SD.h>
#include <Adafruit_ST7735.h>

// Create Current Sensor Objects
INA226 sensor1(CURRENT_SENSOR_1_ADDRESS);
INA226 sensor2(CURRENT_SENSOR_2_ADDRESS);
INA226 sensor3(CURRENT_SENSOR_3_ADDRESS);

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
SensorValues *sensorValues = new SensorValues;

void setup()
{
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);

    Serial.begin(115200);

    pinMode(PIN_PW_ON, OUTPUT);
    pinMode(PIN_PS_GOOD, INPUT);

    digitalWrite(PIN_PW_ON, LOW);

    qindesign::network::Ethernet.begin();

    taskRenderLeds.begin(renderFrame, 10000);

    sensorValues->temperatureTop = 0.0f;
    sensorValues->temperatureCenter = 0.0f;
    sensorValues->temperatureBottom = 0.0f;

    sensorValues->currentLine1 = 0.0f;
    sensorValues->currentLine2 = 0.0f;
    sensorValues->currentLine3 = 0.0f;

    Wire.begin();
    sensor1.begin();
    sensor1.setMaxCurrentShunt(10, 0.006);
    sensor1.setAverage(3);

    sensor2.begin();
    sensor2.setMaxCurrentShunt(10, 0.006);
    sensor2.setAverage(3);

    sensor3.begin();
    sensor3.setMaxCurrentShunt(10, 0.006);
    sensor3.setAverage(3);

    initConsole();
    initWebServer();
    initOctoWS2811();
    initTFT();

    registerEffect("test-led", &effectTestLEDs);
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

    if (
        ((time - taskActivityLed) > 125 && !qindesign::network::Ethernet.linkState()) || ((time - taskActivityLed) > 250 && qindesign::network::Ethernet.localIP()[0] == 0) || (time - taskActivityLed > 500))
    {
        taskActivityLed = time;

        digitalWrite(13, activityLedState ? LOW : HIGH);
        activityLedState = !activityLedState;
    }

    if ((time - taskReadCurrent) > 100)
    {
        fetchCurrentValue(sensor1, TOP, &sensorValues->currentLine1);
        fetchCurrentValue(sensor2, CENTER, &sensorValues->currentLine2);
        fetchCurrentValue(sensor3, BOTTOM, &sensorValues->currentLine3);

        fetchBusVoltageValue(sensor1, TOP, &sensorValues->busVoltageLine1);
        fetchBusVoltageValue(sensor2, CENTER, &sensorValues->busVoltageLine2);
        fetchBusVoltageValue(sensor3, BOTTOM, &sensorValues->busVoltageLine3);

        taskReadCurrent = 0;
    }

    processConsoleData();
    handleWebClient();
}

void fetchBusVoltageValue(INA226 sensor, TUBE_SECTION section, float *oldvalue)
{
    float value = round(sensor.getBusVoltage() * 100.0f) / 100.0f;

    if (*oldvalue != value)
    {
        *oldvalue = value;
        renderVoltageValue(section, value);
    }
}

void fetchCurrentValue(INA226 sensor, TUBE_SECTION section, float *oldvalue)
{
    float value = round((sensor.getShuntVoltage() / 0.006f) * 100.0f) / 100.0f;

    if (*oldvalue != value)
    {
        *oldvalue = value;
        renderCurrentValue(section, value);
    }
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
        return sensor1;
    case 2:
        return sensor2;
    case 3:
    default:
        return sensor3;
    }
}