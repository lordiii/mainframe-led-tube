#include "main.h"
#include "artnet.h"
#include "globals.h"

byte mac[] = MAC_ADDRESS;

Artnet artnet;

void setup()
{
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
    artnet = initializeArtnet(mac, addressBytes, addressBytes);
}

void loop()
{
    checkDHCPStatus();
    artnet.read();
}

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