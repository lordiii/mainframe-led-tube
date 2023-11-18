#include "main.h"
#include "globals.h"

byte mac[] = MAC_ADDRESS;

Artnet artnet;

void setup()
{
    Wire.begin();
    Serial.begin(115200);

    Serial.println("Fetching IP Address via DHCP");
    bool hasIP = initializeDHCP();
    if (!hasIP)
    {
        while (!hasIP)
        {
            delay(1000);
            hasIP = initializeDHCP();
            if (hasIP)
            {
                Serial.println("IP Address assigned....continuing!");
            }
        }
    }

    IPAddress address = Ethernet.localIP();
    byte addressBytes[] = {address[0], address[1], address[2], address[3]};
    artnet = initializeLEDs(mac, addressBytes, addressBytes);

    initializeSensors();
}

long lastMillis = 0;
void loop()
{
    // Perform other tasks every 2.5 second
    if ((millis() - lastMillis) < 2500)
    {
        artnet.read();
    }
    else
    {
        lastMillis = millis();
        checkDHCPStatus();
    }
}

#pragma region Network

void checkDHCPStatus()
{
    switch (Ethernet.maintain())
    {
    case 1:
        // renewed fail
        Serial.println("Error: renewed fail");
        break;

    case 2:
        // renewed success
        Serial.println("Renewed success");
        // print your local IP address:
        Serial.print("My IP address: ");
        Serial.println(Ethernet.localIP());
        break;

    case 3:
        // rebind fail
        Serial.println("Error: rebind fail");
        break;

    case 4:
        // rebind success
        Serial.println("Rebind success");
        // print your local IP address:
        Serial.print("My IP address: ");
        Serial.println(Ethernet.localIP());
        break;

    default:
        // nothing happened
        break;
    }
}

bool initializeDHCP()
{
    if (Ethernet.begin(mac) == 0)
    {
        if (Ethernet.linkStatus() == LinkOFF)
        {
            Serial.println("Ethernet cable is not connected. Retrying every 1 second...");
            return false;
        }

        Serial.println("Unknown Ethernet error... Sorry, can't run without hardware. :(");
        while (true)
        {
            delay(1);
        }
    }

    return true;
}

#pragma endregion

#pragma region Sensors

const uint8_t currentSensor1Address = CURRENT_SENSOR_1_ADDRESS;
INA226 *currentSensor1 = new INA226(currentSensor1Address);

const uint8_t currentSensor2Address = CURRENT_SENSOR_2_ADDRESS;
INA226 *currentSensor2 = new INA226(currentSensor2Address);

const uint8_t currentSensor3Address = CURRENT_SENSOR_3_ADDRESS;
INA226 *currentSensor3 = new INA226(currentSensor3Address);

void initializeSensors()
{
    initializeCurrentSensor(currentSensor1);
    initializeCurrentSensor(currentSensor2);
    initializeCurrentSensor(currentSensor3);
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

#pragma endregion

#pragma region LED Control

/**
 *
 * Heavily Based on the ArtnetOctoWS2811 Example of https://github.com/natcl/Artnet/
 *
 */

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
    testLEDColors();

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

void testLEDColors()
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

#pragma endregions