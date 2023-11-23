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
const uint8_t currentSensor1Address = CURRENT_SENSOR_1_ADDRESS;
INA226 *currentSensor1 = new INA226(currentSensor1Address);

const uint8_t currentSensor2Address = CURRENT_SENSOR_2_ADDRESS;
INA226 *currentSensor2 = new INA226(currentSensor2Address);

const uint8_t currentSensor3Address = CURRENT_SENSOR_3_ADDRESS;
INA226 *currentSensor3 = new INA226(currentSensor3Address);

void setup()
{

    Wire.begin();
    Serial.begin(115200);

    Ethernet.begin();
    while (!Ethernet.waitForLocalIP(15000))
    {
        Serial.println("Waiting for IP Address");
    }

    Serial.println("Testing LEDs");
    leds.begin();
    leds.show();
    testLEDColors();

    Serial.println("Initializing Current Sensors");
    initializeCurrentSensor(currentSensor1);
    initializeCurrentSensor(currentSensor2);
    initializeCurrentSensor(currentSensor3);
}


#define RED    0xFF0000
#define GREEN  0x00FF00
#define BLUE   0x0000FF

// Less intense...
/*
#define RED    0x160000
#define GREEN  0x001600
#define BLUE   0x000016
#define YELLOW 0x101400
#define PINK   0x120009
#define ORANGE 0x100400
#define WHITE  0x101010
*/

void loop() {
  colorWipe(0, 25);
  colorWipe(RED, 25);
  colorWipe(0, 25);
  colorWipe(RED, 25);
  colorWipe(0, 25);
  colorWipe(BLUE, 25);
  colorWipe(0, 25);
  colorWipe(BLUE, 25);
}

void colorWipe(int color, int wait)
{
  for (int i=0; i < leds.numPixels(); i++) {
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