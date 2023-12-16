#include <cli.h>
#include <embedded_cli.h>
#include <Arduino.h>
#include <main.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnusedParameter"

EmbeddedCli *embeddedCli;
unsigned long lastPowerToggle = 0;

void initCLI() {
    auto config = embeddedCliDefaultConfig();
    config->maxBindingCount = 20;
    embeddedCli = embeddedCliNew(config);

    if (embeddedCli == nullptr) {
        while (true) {
            Serial.println(F("Cli was not created. Check sizes!"));
        }
    }

    embeddedCliAddBinding(embeddedCli, {
            "temperature",
            "Display last temperature values",
            true,
            &Serial,
            commandPrintTemperature
    });

    embeddedCliAddBinding(embeddedCli, {
            "current",
            "Display last current values",
            true,
            &Serial,
            commandPrintCurrent
    });

    embeddedCliAddBinding(embeddedCli, {
            "network",
            "Display current network information",
            true,
            &Serial,
            commandPrintNetworkInfo
    });

    embeddedCliAddBinding(embeddedCli, {
            "reboot",
            "Reboot device",
            true,
            &Serial,
            commandReboot
    });

    embeddedCliAddBinding(embeddedCli, {
            "power",
            "Toggle power supply 12V rail. Usage: power [on|off]",
            true,
            &Serial,
            commandTogglePowerSupply
    });

    embeddedCliAddBinding(embeddedCli, {
            "effect",
            "Change current effect. Usage: power [effect name]",
            true,
            &Serial,
            commandSetEffect
    });

    embeddedCliAddBinding(embeddedCli, {
            "brightness",
            "Change current brightness. Usage: brightness [0-100]",
            true,
            &Serial,
            commandSetBrightness
    });

    embeddedCliAddBinding(embeddedCli, {
            "effect-list",
            "List known effects",
            true,
            &Serial,
            commandPrintEffectList
    });

    embeddedCliAddBinding(embeddedCli, {
            "halt",
            "Halt frame calculations",
            true,
            &Serial,
            commandToggleHalt
    });

    embeddedCliAddBinding(embeddedCli, {
            "next",
            "Execute a single frame calculation",
            true,
            &Serial,
            commandExecuteNext
    });

    embeddedCliAddBinding(embeddedCli, {
            "slow",
            "Slow down frame calculations to given interval. Usage: slow [amount]",
            true,
            &Serial,
            commandSlowExecution
    });

    embeddedCli->writeChar = writeCmdOutChar;
}

void processCLI() {
    embeddedCliProcess(embeddedCli);
}

void serialEvent() {
    while (Serial.available()) {
        embeddedCliReceiveChar(embeddedCli, (char) Serial.read());
    }
}

void writeCmdOutChar(EmbeddedCli *cli, char c) {
    Serial.write(c);
}

//
// Sensor Values
//
void commandPrintTemperature(EmbeddedCli *cli, char *args, void *context) {
    auto *out = (Print *) context;
    out->println("TOP\t\tCENTER\t\tBOTTOM\r\n");

    out->print(sensorValues->temperatureTop, 2);
    out->print(" °C\t");

    out->print(sensorValues->temperatureCenter, 2);
    out->print(" °C\t");

    out->print(sensorValues->temperatureBottom, 2);
    out->println(" °C\t");
}

void commandPrintCurrent(EmbeddedCli *cli, char *args, void *context) {
    auto *out = (Print *) context;
    out->println("TOP\t\tCENTER\t\tBOTTOM\r\n");

    out->print(sensorValues->currentLine1, 2);
    out->print(" A\t");

    out->print(sensorValues->currentLine2, 2);
    out->print(" A\t");

    out->print(sensorValues->currentLine3, 2);
    out->println(" A\t");
}

//
// Network Configuration
//
void commandPrintNetworkInfo(EmbeddedCli *cli, char *args, void *context) {
    auto *out = (Print *) context;
    out->println("IP\t\tGATEWAY\t\tBROADCAST\tDNS");

    out->print(qindesign::network::Ethernet.localIP());
    out->print("\t");

    out->print(qindesign::network::Ethernet.gatewayIP());
    out->print("\t");

    out->print(qindesign::network::Ethernet.broadcastIP());
    out->print("\t");

    out->print(qindesign::network::Ethernet.dnsServerIP());
    out->print("\t");
}

//
// Device Control
//
void commandReboot(EmbeddedCli *cli, char *args, void *context) {
    _reboot_Teensyduino_();
}

void commandTogglePowerSupply(EmbeddedCli *cli, char *args, void *context) {
    auto *out = (Print *) context;

    if ((millis() - lastPowerToggle) > 5000) {
        if (embeddedCliGetTokenCount(args) == 0) {
            out->println("Missing argument: power [on|off]");
        } else {
            const char *action = embeddedCliGetToken(args, 1);
            if (strcmp(action, "on") == 0) {
                digitalWrite(PIN_PW_ON, HIGH);
                lastPowerToggle = millis();
                out->println("Power Supply Enabled");
            } else if (strcmp(action, "off") == 0) {
                digitalWrite(PIN_PW_ON, LOW);
                lastPowerToggle = millis();
                out->println("Power Supply Disabled");
            } else {
                out->print("Invalid action: ");
                out->println(action);
            }
        }
    } else {
        out->println("Power supply recently toggled...");
    }
}

//
// Effect Control
//
void commandSetEffect(EmbeddedCli *cli, char *args, void *context) {
    auto *out = (Print *) context;

    if (embeddedCliGetTokenCount(args) == 0) {
        out->println("Missing argument: effect [effect name]");
    } else {
        const char *effectName = embeddedCliGetToken(args, 1);

        Effect *effect = nullptr;
        for (int i = 0; i < effectCount; i++) {
            effect = &effects[i];

            if (strcmp(effect->name, effectName) == 0) {
                break;
            } else {
                effect = nullptr;
            }
        }

        setCurrentEffect(effect);

        if (effect == nullptr) {
            displayEffect("");

            out->print("Effect '");
            out->print(effectName);
            out->println("' not found!");
        } else {
            displayEffect(effectName);

            out->println("Effect set to '");
            out->print(effectName);
            out->println("'!");
        }
    }
}

void commandSetBrightness(EmbeddedCli *cli, char *args, void *context) {
    auto *out = (Print *) context;

    if (embeddedCliGetTokenCount(args) == 0) {
        out->println("Missing argument: brightness [0-100]");
    } else {
        long brightness = strtol(embeddedCliGetToken(args, 1), nullptr, 10);

        if (brightness < 0) {
            brightness = 0;
        } else if (brightness > 100) {
            brightness = 100;
        }

        float value = ((float) brightness) / 100.0f;
        setBrightness(value);
        out->print("Brightness changed to: ");
        out->print(brightness);
        out->println("%");
    }
}

void commandPrintEffectList(EmbeddedCli *cli, char *args, void *context) {
    auto *out = (Print *) context;

    out->println("Known effects: ");

    for (int i = 0; i < effectCount; i++) {
        out->print("\t* ");
        out->print(effects[i].name);
    }

    out->println();
}

void commandToggleHalt(EmbeddedCli *cli, char *args, void *context) {
    auto *out = (Print *) context;
    state->halt = !state->halt;

    if (state->halt) {
        out->println("Halt Enabled!");
    } else {
        out->println("Halt Disabled!");
    }
}

void commandExecuteNext(EmbeddedCli *cli, char *args, void *context) {
    state->singleStep = true;
}

void commandSlowExecution(EmbeddedCli *cli, char *args, void *context) {
    auto *out = (Print *) context;

    if (embeddedCliGetTokenCount(args) == 0) {
        out->println("Missing argument: brightness [0-100]");
    } else {
        state->slowRate = String(embeddedCliGetToken(args, 1)).toInt();

        out->print("Set animation slowing to ");
        out->print(state->slowRate);
        out->println("ms");
    }
}

#pragma clang diagnostic pop