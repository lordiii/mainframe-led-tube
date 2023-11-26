#include "main.h"
#include "globals.h"

// Setup LEDs
const byte pinList[LED_STRIP_AMOUNT] = LED_PINS;
const int numLeds = LED_PER_STRIP * LED_STRIP_AMOUNT;
const int numberOfChannels = numLeds * 3; // Total number of channels you want to receive (1 led = 3 channels)

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
const unsigned char tempProbeUpper[8] = TEMPERATURE_SENSOR_TOP_ADDRESS;
const unsigned char tempProbeMiddle[8] = TEMPERATURE_SENSOR_MIDDLE_ADDRESS;
const unsigned char tempProbeLower[8] = TEMPERATURE_SENSOR_BOTTOM_ADDRESS;

void setup()
{
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);

    Wire.begin();
    Serial.begin(115200);

    Ethernet.begin();
    //while (!Ethernet.waitForLocalIP(15000))
    //{
    //    Serial.println("Waiting for IP Address");
    //}

    Serial.println("Current IP: ");
    Serial.print(Ethernet.localIP()[0]);
    Serial.print(".");
    Serial.print(Ethernet.localIP()[1]);
    Serial.print(".");
    Serial.print(Ethernet.localIP()[2]);
    Serial.print(".");
    Serial.print(Ethernet.localIP()[3]);

    leds.begin();
    leds.show();
    testLEDColors();


    Serial.println("Initializing Current Sensors");
    initializeCurrentSensor(currentSensor1);
    initializeCurrentSensor(currentSensor2);
    initializeCurrentSensor(currentSensor3);
}

#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF

long last = 0;
void loop()
{
    testLEDColors();

    if (millis() - last > 2500)
    {
        sensors.requestTemperatures();
        float tempCTop = sensors.getTempC(tempProbeUpper);
        float tempCMiddle = sensors.getTempC(tempProbeMiddle);
        float tempCBottom = sensors.getTempC(tempProbeLower);

        Serial.print("Temperatures: ");
        Serial.print(tempCTop);
        Serial.print(" \xC2\xB0");
        Serial.print("C  Top |  ");
        Serial.print(tempCMiddle);
        Serial.print(" \xC2\xB0");
        Serial.print("C  Middle |  ");
        Serial.print(tempCBottom);
        Serial.print(" \xC2\xB0");
        Serial.print("C Bottom");
        Serial.println();

        last = millis();
    }
}

void colorWipe(int color, int wait)
{
    for (int i = 0; i < leds.numPixels(); i++)
    {
        leds.setPixel(i, color);
    }
    leds.show();
    delay(wait);
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

// OctoWS2811 settings

// Check if we got all universes
void testLEDColors()
{
    Serial.println("Testing LEDs");

    for (int i = 0; i < numLeds; i++)
    {
        leds.setPixel(i, 127, 0, 0);
    }
    Serial.println("Testing Red LEDs");
    leds.show();

    delay(500);

    for (int i = 0; i < numLeds; i++)
    {
        leds.setPixel(i, 0, 127, 0);
    }
    Serial.println("Testing Green LEDs");
    leds.show();

    delay(500);

    for (int i = 0; i < numLeds; i++)
    {
        leds.setPixel(i, 0, 0, 127);
    }
    Serial.println("Testing Blue LEDs");
    leds.show();

    delay(500);

    for (int i = 0; i < numLeds; i++)
    {
        leds.setPixel(i, 0, 0, 0);
    }
    leds.show();
}