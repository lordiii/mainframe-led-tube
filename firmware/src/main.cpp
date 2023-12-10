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

// Create Sensor Objects
INA226 currentSensorTop(CURRENT_SENSOR_TOP_ADDRESS);
INA226 currentSensorCenter(CURRENT_SENSOR_CENTER_ADDRESS);
INA226 currentSensorBottom(CURRENT_SENSOR_BOTTOM_ADDRESS);

OneWire oneWire(24);
DallasTemperature tempSensors(&oneWire);
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
unsigned long taskReadControllerInput = 0;

// Sensor Values
SensorValues *sensorValues = new SensorValues;

ControllerStatus *controller = new ControllerStatus;

void setup()
{
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);

    SD.begin(BUILTIN_SDCARD);

    Serial.begin(115200);

    pinMode(PIN_PW_ON, OUTPUT);
    pinMode(PIN_PS_GOOD, INPUT);

    digitalWrite(PIN_PW_ON, LOW);

    qindesign::network::Ethernet.begin();

    taskRenderLeds.begin(renderFrame, 12621);

    sensorValues->temperatureTop = 0.0f;
    sensorValues->temperatureCenter = 0.0f;
    sensorValues->temperatureBottom = 0.0f;

    sensorValues->currentLine1 = 0.0f;
    sensorValues->currentLine2 = 0.0f;
    sensorValues->currentLine3 = 0.0f;

    Wire.begin();
    currentSensorBottom.begin();
    currentSensorBottom.setMaxCurrentShunt(10, 0.006);
    currentSensorBottom.setAverage(3);

    currentSensorCenter.begin();
    currentSensorCenter.setMaxCurrentShunt(10, 0.006);
    currentSensorCenter.setAverage(3);

    currentSensorTop.begin();
    currentSensorTop.setMaxCurrentShunt(10, 0.006);
    currentSensorTop.setAverage(3);

    initConsole();
    initWebServer();
    initOctoWS2811();
    initTFT();
}

char controllerBuffer[2] = {};
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
            tempSensors.requestTemperatures();
            toggleTemperatureReadWrite = !toggleTemperatureReadWrite;
        }
        else
        {
            fetchTemperatureValue(tempProbeTop, TOP, &sensorValues->temperatureTop);
            fetchTemperatureValue(tempProbeCenter, CENTER, &sensorValues->temperatureCenter);
            fetchTemperatureValue(tempProbeBottom, BOTTOM, &sensorValues->temperatureBottom);

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
        fetchCurrentValue(currentSensorTop, TOP, &sensorValues->currentLine3);
        fetchCurrentValue(currentSensorCenter, CENTER, &sensorValues->currentLine2);
        fetchCurrentValue(currentSensorBottom, BOTTOM, &sensorValues->currentLine1);

        fetchBusVoltageValue(currentSensorTop, TOP, &sensorValues->busVoltageLine3);
        fetchBusVoltageValue(currentSensorCenter, CENTER, &sensorValues->busVoltageLine2);
        fetchBusVoltageValue(currentSensorBottom, BOTTOM, &sensorValues->busVoltageLine1);

        taskReadCurrent = time;
    }

    if((time - taskReadControllerInput) > 15)
    {
        memset(controllerBuffer, 0, sizeof(controllerBuffer));
        uint8_t quantity = Wire.requestFrom(0x55, sizeof(controllerBuffer));
        size_t read = Wire.readBytes(controllerBuffer, quantity);

        processControllerInputs();
        Wire.begin();

        taskReadControllerInput = time;
    }
    
    processConsoleData();
    handleWebClient();

    if(Wire.getReadError())
    {
        Wire.clearReadError();
    }

    if(Wire.getWriteError())
    {
        Wire.clearWriteError();
    }
}

void processControllerInputs()
{
    controller->dpadLeft = controllerBuffer[0] & 0b001000;
    controller->dpadRight = controllerBuffer[0] & 0b000100;
    controller->dpadUp = controllerBuffer[0] & 0b000001;
    controller->dpadDown = controllerBuffer[0] & 0b000010;

    controller->buttonY = (controllerBuffer[1] & 0b10000000) > 0;
    controller->buttonB = (controllerBuffer[1] & 0b01000000) > 0;
    controller->buttonA = (controllerBuffer[1] & 0b00100000) > 0;
    controller->buttonX = (controllerBuffer[1] & 0b00010000) > 0;

    controller->shoulderL1 = controllerBuffer[1] & 0b00001000;
    controller->shoulderL2 = controllerBuffer[1] & 0b00000100;
    controller->shoulderR1 = controllerBuffer[1] & 0b00000010;
    controller->shoulderR2 = controllerBuffer[1] & 0b00000001;
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

void fetchTemperatureValue(const uint8_t * sensor, TUBE_SECTION section, float *oldvalue)
{
    float value = round(tempSensors.getTempC(sensor) * 100.0f) / 100.0f;

    if (*oldvalue != value)
    {
        *oldvalue = value;
        renderTemperatureValue(section, value);
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
        return currentSensorBottom;
    case 2:
        return currentSensorCenter;
    case 3:
    default:
        return currentSensorTop;
    }
}