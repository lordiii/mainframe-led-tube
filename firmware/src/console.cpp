#include "console.h"
#include "main.h"
#include "effects.h"
#include "globals.h"

bool debug = false;

const int BUFFER_SIZE = 65;
String consoleBuffer;

void initConsole()
{
    consoleBuffer = String("");
}

void processConsoleData()
{
    bool change = false;

    while (Serial.available())
    {
        size_t bufferLength = consoleBuffer.length();
        char character = Serial.read();

        switch (character)
        {
        case 0xD:
            Serial.print("\n\r");
            processCommand();
            consoleBuffer = "";
            change = true;
            break;
        case 0x7F:
            if (bufferLength > 0)
            {
                consoleBuffer = consoleBuffer.substring(0, consoleBuffer.length() - 1);
                change = true;
            }
            break;
        default:
            if (bufferLength < (BUFFER_SIZE - 1) && ((character >= 0x20 && character <= 0x7E) || debug))
            {
                consoleBuffer.append(character);
                change = true;
            }
            break;
        }
    }

    if (change)
    {
        size_t bufferLength = consoleBuffer.length();

        Serial.print("\33[2K\r#> ");
        for (size_t i = 0; i < bufferLength; i++)
        {
            char character = consoleBuffer[i];

            if (!debug)
            {
                Serial.write(character);
            }
            else
            {
                Serial.print(character, 16);
                Serial.print(' ');
            }
        }
    }
}

void processCommand()
{
    if (consoleBuffer.equals("temperature"))
    {
        commandPrintTemperatures();
    }
    else if (consoleBuffer.equals("network"))
    {
        commandPrintNetworkInfo();
    }
    else if (consoleBuffer.equals("reboot"))
    {
        commandReboot();
    }
    else if (consoleBuffer.startsWith("effect "))
    {
        commandSetEffect();
    }
    else if (consoleBuffer.startsWith("brightness "))
    {
        commandSetBrightness();
    }
    else if (consoleBuffer.equals("current"))
    {
        commandPrintCurrent();
    }
    else if (consoleBuffer.equals("debug"))
    {
        debug = !debug;
    } else if(consoleBuffer.equals("effect-list"))
    {
        commandPrintEffectList();
    }
}

void commandPrintTemperatures()
{
    Serial.print("Top: ");
    Serial.print(sensorValues->temperatureTop);
    Serial.println("°C");

    Serial.print("Center: ");
    Serial.print(sensorValues->temperatureCenter);
    Serial.println("°C");

    Serial.print("Bottom: ");
    Serial.print(sensorValues->temperatureBottom);
    Serial.println("°C");
}

void commandPrintCurrent()
{
    Serial.print("Top: ");
    Serial.print(sensorValues->currentLine1);
    Serial.println("mA");

    Serial.print("Center: ");
    Serial.print(sensorValues->currentLine2);
    Serial.println("mA");

    Serial.print("Bottom: ");
    Serial.print(sensorValues->currentLine3);
    Serial.println("mA");
}

void commandPrintNetworkInfo()
{
    Serial.print("IP: ");
    Serial.println(qindesign::network::Ethernet.localIP());

    Serial.print("Gateway: ");
    Serial.println(qindesign::network::Ethernet.gatewayIP());

    Serial.print("Broadcast: ");
    Serial.println(qindesign::network::Ethernet.broadcastIP());

    Serial.print("DNS: ");
    Serial.println(qindesign::network::Ethernet.dnsServerIP());
}

void commandReboot()
{
    _reboot_Teensyduino_();
}

void commandSetEffect()
{
    String effectName = consoleBuffer.substring(strlen("effect "));
    Effect *effect = nullptr;

    if(!effectName.equals("off")) {
        for(int i = 0; i < effects->length; i++)
        {
            effect = i == 0 ? effects->first : effect->next;

            if(effect != nullptr && effect->name.equals(effectName))
            {
                break;
            }
        }
    }

    setCurrentEffect(effect);

    if(effect == nullptr)
    {
        Serial.println("Effect '" + effectName + "' not found!");
    } else
    {
        Serial.print("Effect set to '" + effectName + "'!");
    }
}

void commandSetBrightness()
{
    int brightness = consoleBuffer.substring(strlen("brightness ")).toInt();
    Serial.println(brightness);

    if (brightness)
    {
        if (brightness < 0)
        {
            brightness = 0;
        }

        if (brightness > 100)
        {
            brightness = 100;
        }
    }
    else
    {
        brightness = 0;
    }

    float value = brightness / 100.0f;
    setBrightness(value);
    Serial.print("Brightness changed to: ");
    Serial.print(brightness);
    Serial.println('%');
}

void commandPrintEffectList()
{
    Serial.println("Known effects: ");

    Effect *effect = nullptr;
    for(int i = 0; i < effects->length; i++)
    {
        effect = i == 0 ? effects->first : effect->next;

        if(effect != nullptr)
        {
            Serial.println("\t> " + effect->name);
        }
    }
}