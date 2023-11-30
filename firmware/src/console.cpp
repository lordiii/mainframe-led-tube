#include "console.h"
#include "main.h"
#include "globals.h"

bool debug = false;

const int BUFFER_SIZE = 65;
char *consoleBuffer;

void initConsole()
{
    consoleBuffer = (char *)malloc(BUFFER_SIZE);
    resetConsoleBuffer(true);
}

void processConsoleData()
{
    while (Serial.available())
    {
        char character = Serial.read();

        switch (character)
        {
        case 0xD:
            Serial.print("\n\r");
            processCommand();
            resetConsoleBuffer(false);
            break;
        case 0x7F:
            if (strlen(consoleBuffer) > 0)
            {
                consoleBuffer[strlen(consoleBuffer) - 1] = '\0';
            }
            break;
        default:
            if (strlen(consoleBuffer) < (BUFFER_SIZE - 1) && ((character >= 0x20 && character <= 0x7E) || debug))
            {
                consoleBuffer[strlen(consoleBuffer)] = character;
                consoleBuffer[strlen(consoleBuffer) + 1] = '\0';
            }
            break;
        }

        Serial.print("\33[2K\r#> ");
        for (size_t i = 0; i < strlen(consoleBuffer); i++)
        {
            char character = consoleBuffer[i];

            if (character != 0x1B && character != 0x5B && character != 0x41 && character != 0x42)
            {
                if (!debug)
                {
                    Serial.write(consoleBuffer[i]);
                }
                else
                {
                    Serial.print(consoleBuffer[i], 16);
                    Serial.print(' ');
                }
            }
        }
    }
}

void resetConsoleBuffer(bool skipHistory)
{
    memset(consoleBuffer, 0, BUFFER_SIZE);
    consoleBuffer[0] = '\0';
}

void processCommand()
{
    if (strcmp(consoleBuffer, "temperature") == 0)
    {
        printTemperatures();
    }
    else if (strcmp(consoleBuffer, "network") == 0)
    {
        printNetworkInfo();
    }
    else if (strcmp(consoleBuffer, "reboot") == 0)
    {
        reboot();
    }
    else if (startsWith("effect ", consoleBuffer))
    {
        setEffect();
    }
    else if (strcmp(consoleBuffer, "debug") == 0)
    {
        debug = !debug;
    }
}

void printTemperatures()
{
    SensorValues *sensorValues = getSensorValues();

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

void printNetworkInfo()
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

void reboot()
{
    _reboot_Teensyduino_();
}

struct EffectName
{
    char *name;
    LED_EFFECT type;
};

EffectName EFFECT_NAMES[] = {
    (EffectName){"led-test", LED_EFFECT::LED_TEST},
    (EffectName){"strobe", LED_EFFECT::STROBE},
    (EffectName){"rainbow-strobe", LED_EFFECT::RAINBOW_STROBE},
    (EffectName){"police", LED_EFFECT::POLICE},
    (EffectName){"off", LED_EFFECT::OFF},

};

void setEffect()
{
    size_t offset = strlen("effect ");
    size_t total = strlen(consoleBuffer);
    size_t new_str = total - offset + 1;

    char *effectName = (char *)malloc(new_str);
    memcpy(effectName, &consoleBuffer[offset], new_str);

    bool found = false;
    for (size_t i = 0; i < sizeof(EFFECT_NAMES); i++)
    {
        if (strcmp(EFFECT_NAMES[i].name, effectName) == 0)
        {
            setCurrentEffect(EFFECT_NAMES[i].type);
            found = true;
            break;
        }
    }

    if (!found)
    {
        Serial.print("Effect '");
        Serial.print(effectName);
        Serial.println("' not found!");
    }
    else
    {
        Serial.print("Effect set to '");
        Serial.print(effectName);
        Serial.println("'!");
    }
}

bool startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre);
    size_t lenstr = strlen(str);
    return lenstr < lenpre ? false : memcmp(pre, str, lenpre) == 0;
}