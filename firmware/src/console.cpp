#include <console.h>
#include <main.h>

#include <effects.h>
#include <globals.h>

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
    else if (consoleBuffer.startsWith("power "))
    {
        commandTogglePowerSupply();
    } else if (consoleBuffer.equals("halt") || consoleBuffer.equals("h"))
    {
        commandToggleHalt();
    } else if (consoleBuffer.equals("next") || consoleBuffer.equals("n"))
    {
        commandExecuteNext();
    } else if (consoleBuffer.startsWith("slow ")) 
    {
        state->slowRate = consoleBuffer.substring(strlen("slow ")).toInt();
    } else {
        Serial.println("Command not found...");
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
    Serial.print(sensorValues->currentLine1, 4);
    Serial.println("A");

    Serial.print("Center: ");
    Serial.print(sensorValues->currentLine2, 4);
    Serial.println("A");

    Serial.print("Bottom: ");
    Serial.print(sensorValues->currentLine3, 4);
    Serial.println("A");
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
    for(int i = 0; i < effectCount; i++)
    {
        effect = &effects[i];

        if(effect->name.equals(effectName))
        {
            break;
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

void commandSensorStatus()
{
    int id = consoleBuffer.substring(strlen("sensor-status ")).toInt();

    INA226 sensor = getCurrentSensor(id);

    Serial.print("Die-ID:");
    Serial.println(sensor.getDieID());

    Serial.println("\nBUS\tSHUNT\tCURRENT\tPOWER");
    Serial.print(sensor.getBusVoltage(), 3);
    Serial.print("\t");
    Serial.print(sensor.getShuntVoltage_mV(), 3);
    Serial.print("\t");
    Serial.print(sensor.getCurrent(), 3);
    Serial.print("\t");
    Serial.print(sensor.getPower(), 3);
    Serial.println();
}

long lastToggle = 0;
void commandTogglePowerSupply()
{
    if ((millis() - lastToggle) > 5000)
    {
        String sub = consoleBuffer.substring(strlen("power "));
        if (sub.equals("on"))
        {
            digitalWrite(PIN_PW_ON, HIGH);
            Serial.println("Power Supply Enabled");
        }
        else if (sub.equals("off"))
        {
            digitalWrite(PIN_PW_ON, LOW);
            Serial.println("Power Supply Disabled");
        }

        lastToggle = millis();
    }
    else
    {
        Serial.println("Power supply recentry toggled...");
    }
}

void commandPrintEffectList()
{
    Serial.println("Known effects: ");

    for(int i = 0; i < effectCount; i++)
    {
        Serial.println("\t> " + effects[i].name);
    }
}

void commandToggleHalt()
{
    state->halt = !state->halt;

    if(state->halt)
    {
        Serial.println("Halt Enabled!");
    } else {
        Serial.println("Halt Disabled!");
    }
}

void commandExecuteNext()
{
    state->singleStep = true;
}

void commandSlowExecution()
{
    if(state->slowRate != 0)
    {
        state->slowRate = 0;
        Serial.println("Disabled animation slowing!");
    } else {
        ;
        Serial.print("Set animation slowing to ");
        Serial.print(state->slowRate);
        Serial.println("ms");
    }
}