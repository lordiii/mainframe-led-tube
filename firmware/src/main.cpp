#include <main.h>
#include <globals.h>
#include <effects.h>
#include <web.h>
#include <cli.h>
#include <Wire.h>
#include <INA226.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SD.h>
#include <Entropy.h>

// Create Sensor Objects
INA226 currentSensorTop(CURRENT_SENSOR_TOP_ADDRESS);
INA226 currentSensorCenter(CURRENT_SENSOR_CENTER_ADDRESS);
INA226 currentSensorBottom(CURRENT_SENSOR_BOTTOM_ADDRESS);

OneWire oneWire(24);
DallasTemperature tempSensors(&oneWire);
const unsigned char tempProbeTop[8] = TEMPERATURE_SENSOR_TOP_ADDRESS;
const unsigned char tempProbeCenter[8] = TEMPERATURE_SENSOR_CENTER_ADDRESS;
const unsigned char tempProbeBottom[8] = TEMPERATURE_SENSOR_BOTTOM_ADDRESS;

// Scheduled Tasks
unsigned long taskReadSensors = 0;
unsigned long taskActivityLed = 0;
unsigned long taskReadCurrent = 0;
unsigned long taskReadControllerInput = 0;

// Sensor Values
SensorValues *sensorValues = new SensorValues;

ControllerStatus *controller = new ControllerStatus;

void setup() {
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);

    SD.begin(BUILTIN_SDCARD);

    Serial.begin(115200);

    pinMode(PIN_PW_ON, OUTPUT);
    pinMode(PIN_PS_GOOD, INPUT);

    digitalWrite(PIN_PW_ON, LOW);

    qindesign::network::Ethernet.begin();

    sensorValues->temperatureTop = 0.0f;
    sensorValues->temperatureCenter = 0.0f;
    sensorValues->temperatureBottom = 0.0f;

    sensorValues->currentLine1 = 0.0f;
    sensorValues->currentLine2 = 0.0f;
    sensorValues->currentLine3 = 0.0f;

    Wire.begin();
    currentSensorBottom.reset();
    currentSensorBottom.begin();
    currentSensorBottom.setMaxCurrentShunt(10, 0.006);
    currentSensorBottom.setAverage(3);

    currentSensorCenter.reset();
    currentSensorCenter.begin();
    currentSensorCenter.setMaxCurrentShunt(10, 0.006);
    currentSensorCenter.setAverage(3);

    currentSensorTop.reset();
    currentSensorTop.begin();
    currentSensorTop.setMaxCurrentShunt(10, 0.006);
    currentSensorTop.setAverage(3);

    initCLI();
    initWebServer();
    initOctoWS2811();
    initTFT();

    Entropy.Initialize();
}

uint8_t controllerBuffer[27] = {};
bool toggleTemperatureReadWrite = false;
bool activityLedState = false;

void loop() {
    unsigned long time = millis();

    if ((time - taskReadSensors) > 1000 && false) {
        taskReadSensors = time;

        // Read Temperature Values
        if (toggleTemperatureReadWrite) {
            tempSensors.requestTemperatures();
            toggleTemperatureReadWrite = false;
        } else {
            fetchTemperatureValue(tempProbeTop, TOP, &sensorValues->temperatureTop);
            fetchTemperatureValue(tempProbeCenter, CENTER, &sensorValues->temperatureCenter);
            fetchTemperatureValue(tempProbeBottom, BOTTOM, &sensorValues->temperatureBottom);

            toggleTemperatureReadWrite = true;
        }
    }

    if (
            ((time - taskActivityLed) > 125 && !qindesign::network::Ethernet.linkState()) ||
            ((time - taskActivityLed) > 250 && qindesign::network::Ethernet.localIP()[0] == 0) ||
            (time - taskActivityLed > 500)) {
        taskActivityLed = time;

        digitalWrite(13, activityLedState ? LOW : HIGH);
        activityLedState = !activityLedState;
    }

    if ((time - taskReadCurrent) > 100 && false) {
        fetchCurrentValue(currentSensorTop, TOP, &sensorValues->currentLine3);
        fetchCurrentValue(currentSensorCenter, CENTER, &sensorValues->currentLine2);
        fetchCurrentValue(currentSensorBottom, BOTTOM, &sensorValues->currentLine1);

        fetchBusVoltageValue(currentSensorTop, TOP, &sensorValues->busVoltageLine3);
        fetchBusVoltageValue(currentSensorCenter, CENTER, &sensorValues->busVoltageLine2);
        fetchBusVoltageValue(currentSensorBottom, BOTTOM, &sensorValues->busVoltageLine1);

        taskReadCurrent = time;
    }

    if ((time - taskReadControllerInput) > 20 && false) {
        memset(controllerBuffer, 0, sizeof(controllerBuffer));
        uint8_t quantity = Wire.requestFrom(0x55, sizeof(controllerBuffer));
        Wire.readBytes(controllerBuffer, quantity);
        processControllerInputs();
        Wire.begin();

        taskReadControllerInput = time;
    }

    processCLI();
    handleWebClient();

    if (Wire.getReadError()) {
        Wire.clearReadError();
    }

    if (Wire.getWriteError()) {
        Wire.clearWriteError();
    }
}

void processButton(bool *value, Button type, uint8_t mask, uint8_t source) {
    bool pressed = (source & mask) > 0;

    if (pressed && !*value) {
        if (state->current != nullptr) {
            state->current->onButtonPress(type);
        }
    }
    *value = pressed;
}

void processAnalogValue(size_t offset, int *value, Button type) {
    *value = ((int) (controllerBuffer[offset + 0])) << 24 | ((int) controllerBuffer[offset + 1]) << 16 |
             ((int) controllerBuffer[offset + 2]) << 8 | ((int) controllerBuffer[offset + 3]);

    if (*value != 0 && state->current != nullptr) {
        state->current->onAnalogButton(type, *value);
    }
}

void processControllerInputs() {
    processButton(&controller->dpadLeft, DPAD_LEFT, 0b00001000, controllerBuffer[0]);
    processButton(&controller->dpadRight, DPAD_RIGHT, 0b00000100, controllerBuffer[0]);
    processButton(&controller->dpadDown, DPAD_DOWN, 0b00000010, controllerBuffer[0]);
    processButton(&controller->dpadUp, DPAD_UP, 0b00000001, controllerBuffer[0]);

    processButton(&controller->buttonY, BUTTON_Y, 0b10000000, controllerBuffer[1]);
    processButton(&controller->buttonB, BUTTON_B, 0b01000000, controllerBuffer[1]);
    processButton(&controller->buttonA, BUTTON_A, 0b00100000, controllerBuffer[1]);
    processButton(&controller->buttonX, BUTTON_X, 0b00010000, controllerBuffer[1]);
    processButton(&controller->shoulderL1, SHOULDER_L1, 0b00001000, controllerBuffer[1]);
    processButton(&controller->shoulderR2, SHOULDER_R2, 0b00000100, controllerBuffer[1]);
    processButton(&controller->shoulderR1, SHOULDER_R1, 0b00000010, controllerBuffer[1]);
    processButton(&controller->shoulderL2, SHOULDER_L2, 0b00000001, controllerBuffer[1]);

    processButton(&controller->miscHome, MISC_HOME, 0b10000000, controllerBuffer[2]);
    processButton(&controller->miscStart, MISC_START, 0b01000000, controllerBuffer[2]);
    processButton(&controller->miscSelect, MISC_SELECT, 0b00100000, controllerBuffer[2]);
    processButton(&controller->miscSystem, MISC_SYSTEM, 0b00010000, controllerBuffer[2]);
    processButton(&controller->miscBack, MISC_BACK, 0b00001000, controllerBuffer[2]);
    processButton(&controller->miscCapture, MISC_CAPTURE, 0b00000100, controllerBuffer[2]);
    processButton(&controller->buttonTR, BUTTON_TR, 0b00000010, controllerBuffer[2]);
    processButton(&controller->buttonTL, BUTTON_TL, 0b00000001, controllerBuffer[2]);

    processAnalogValue(3, &controller->breakForce, BREAK);
    processAnalogValue(7, &controller->throttleForce, THROTTLE);
    processAnalogValue(11, &controller->stickLX, STICK_L_X);
    processAnalogValue(15, &controller->stickLY, STICK_L_Y);
    processAnalogValue(19, &controller->stickRX, STICK_R_X);
    processAnalogValue(23, &controller->stickRY, STICK_R_Y);
}

void fetchBusVoltageValue(INA226 sensor, TUBE_SECTION section, float *oldvalue) {
    float value = round(sensor.getBusVoltage() * 100.0f) / 100.0f;

    if (*oldvalue != value) {
        *oldvalue = value;
        renderVoltageValue(section, value);
    }
}

void fetchCurrentValue(INA226 sensor, TUBE_SECTION section, float *oldvalue) {
    float value = round((sensor.getShuntVoltage() / 0.006f) * 100.0f) / 100.0f;

    if (*oldvalue != value) {
        *oldvalue = value;
        renderCurrentValue(section, value);
    }
}

void fetchTemperatureValue(const uint8_t *sensor, TUBE_SECTION section, float *oldvalue) {
    float value = round(tempSensors.getTempC(sensor) * 100.0f) / 100.0f;

    if (*oldvalue != value) {
        *oldvalue = value;
        renderTemperatureValue(section, value);
    }
}

File getFileContents(const String &fileName) {
    if (SD.begin(BUILTIN_SDCARD)) {

        File dataFile = SD.open(fileName.c_str(), FILE_READ);

        if (dataFile) {
            return dataFile;
        } else {
            return nullptr;
        }
    } else {
        return nullptr;
    }
}

INA226 getCurrentSensor(int id) {
    switch (id) {
        case 1:
            return currentSensorBottom;
        case 2:
            return currentSensorCenter;
        case 3:
        default:
            return currentSensorTop;
    }
}