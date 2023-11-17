/**
 * 
 * Heavily Based on the ArtnetOctoWS2811 Example of https://github.com/natcl/Artnet/
 * 
*/

#include "led_control.h"
#include "globals.h"

// OctoWS2811 settings
const byte pinList[LED_STRIP_AMOUNT] = LED_PINS;

const int numLeds = LED_PER_STRIP * LED_STRIP_AMOUNT;

const int numberOfChannels = numLeds * 3; // Total number of channels you want to receive (1 led = 3 channels)

DMAMEM int displayMemory[LED_PER_STRIP * 6];
int drawingMemory[LED_PER_STRIP * 6];

const int config = LED_CONFIGURATION;
OctoWS2811 leds(LED_PER_STRIP, displayMemory, drawingMemory, config, LED_STRIP_AMOUNT, pinList);

// Check if we got all universes
const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
bool universesReceived[maxUniverses];
bool sendFrame = 1;
int previousDataLength = 0;

Artnet initializeLEDs(byte mac[4], byte ipAddress[4], byte broadcast[4])
{
    Artnet artnet;

    artnet.setBroadcast(broadcast);
    artnet.begin(mac, ipAddress);

    // Init LEDS
    leds.begin();
    initTest();

    // this will be called for each packet received
    artnet.setArtDmxCallback(onDmxFrame);

    return artnet;
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t *data, IPAddress remoteIP)
{
    sendFrame = 1;

    // Store which universe has got in
    if ((universe - ARTNET_START_UNIVERSE) < maxUniverses)
        universesReceived[universe - ARTNET_START_UNIVERSE] = 1;

    for (int i = 0; i < maxUniverses; i++)
    {
        if (universesReceived[i] == 0)
        {
            sendFrame = 0;
            break;
        }
    }

    // read universe and put into the right part of the display buffer
    for (int i = 0; i < length / 3; i++)
    {
        int led = i + (universe - ARTNET_START_UNIVERSE) * (previousDataLength / 3);
        if (led < numLeds)
            leds.setPixel(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    previousDataLength = length;

    if (sendFrame)
    {
        leds.show();
        // Reset universeReceived to 0
        memset(universesReceived, 0, maxUniverses);
    }
}

void initTest()
{
    // Try Red LEDs
    for (int i = 0; i < numLeds; i++)
    {
        leds.setPixel(i, 127, 0, 0);
    }
    leds.show();

    delay(500);

    // Try Green LEDs
    for (int i = 0; i < numLeds; i++)
    {
        leds.setPixel(i, 0, 127, 0);
    }
    leds.show();

    delay(500);

    // Try Blue LEDs
    for (int i = 0; i < numLeds; i++)
    {
        leds.setPixel(i, 0, 0, 127);
    }
    leds.show();

    delay(500);

    for (int i = 0; i < numLeds; i++)
    {
        leds.setPixel(i, 0, 0, 0);
    }
    leds.show();
}