#include <cli.h>
#include <embedded_cli.h>
#include <Arduino.h>
#include <main.h>
#include <effects_lib.h>

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

    embeddedCliAddBinding(embeddedCli, {
            "set-ring",
            "Set given ring to given color. Usage: set-ring [ring] [color]",
            true,
            &Serial,
            commandSetRing
    });

    embeddedCliAddBinding(embeddedCli, {
            "set-all",
            "Set all LEDs to given color. Usage: set-all [color]",
            true,
            &Serial,
            commandSetAll
    });

    embeddedCliAddBinding(embeddedCli, {
            "gamepad-clear",
            "Tell Gamepad Controller to clear all known controllers",
            true,
            &Serial,
            commandClearGamepads
    });

    embeddedCliAddBinding(embeddedCli, {
            "gamepad-register",
            "Toggle registration of new controller on gamepad controller. Usage: gamepad-register: [on|off]",
            true,
            &Serial,
            commandToggleGamepadRegister
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

    out->print(sensorValues->currentSensorTop, 2);
    out->print(" A\t");

    out->print(sensorValues->currentSensorCenter, 2);
    out->print(" A\t");

    out->print(sensorValues->currentSensorBottom, 2);
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
    out->println("\t");
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

            out->print("Effect set to '");
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
        out->println(effects[i].name);
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
        state->slowRate = strtol(embeddedCliGetToken(args, 1), nullptr, 10);

        out->print("Set animation slowing to ");
        out->print(state->slowRate);
        out->println("ms");
    }
}

void commandSetRing(EmbeddedCli *cli, char *args, void *context) {
    auto *out = (Print *) context;

    if (embeddedCliGetTokenCount(args) == 0) {
        out->println("Missing argument: set-ring [ring] [color]");
    } else {
        int ring = strtol(embeddedCliGetToken(args, 1), nullptr, 10);

        int color;
        if (embeddedCliGetTokenCount(args) > 1) {
            color = strtol(embeddedCliGetToken(args, 2), nullptr, 16);
        } else {
            color = 0xFFFFFF;
        }

        out->print("Set ring ");
        out->print(ring);
        out->print(" to color ");
        out->println(color, 16);

        setRingColor(ring, color);
    }
}

void commandSetAll(EmbeddedCli *cli, char *args, void *context) {
    auto *out = (Print *) context;

    if (embeddedCliGetTokenCount(args) == 0) {
        out->println("Missing argument: set-all [color]");
    } else {
        int color = strtol(embeddedCliGetToken(args, 1), nullptr, 16);

        out->print("Setting all to color ");
        out->println(color, 16);

        fillLEDs(color);
    }
}

//
// Gamepad control
//
void commandClearGamepads(EmbeddedCli *cli, char *args, void *context) {
    auto *out = (Print *) context;

    Wire.beginTransmission(I2C_CONTROLLER);
    Wire.write(0x00);
    Wire.endTransmission();

    out->println("Gamepads cleared!");
}

void commandToggleGamepadRegister(EmbeddedCli *cli, char *args, void *context) {
    auto *out = (Print *) context;

    if (embeddedCliGetTokenCount(args) == 0) {
        out->println("Missing argument: gamepad-register [on|off]");
    } else {
        const char *command = embeddedCliGetToken(args, 1);

        bool turnOn = strcmp(command, "on") == 0;
        if (turnOn) {
            Wire.beginTransmission(I2C_CONTROLLER);
            Wire.write(0x01);
            Wire.endTransmission();
        } else {
            Wire.beginTransmission(I2C_CONTROLLER);
            Wire.write(0x02);
            Wire.endTransmission();
        }

        out->print("Controller registration turned ");
        out->print(turnOn ? "on" : "off");
    }
}

#pragma clang diagnostic pop