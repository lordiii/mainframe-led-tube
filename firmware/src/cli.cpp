#include "cli.h"
#include "globals.h"
#include "main.h"
#include "effects/_effects.h"
#include "tft.h"
#include "led.h"
#include "display.h"
#include "gamepad.h"

#include <Wire.h>
#include <embedded_cli.h>
#include <Arduino.h>

EmbeddedCli *embeddedCli;
unsigned long lastPowerToggle = 0;

void initCLI() {
    EmbeddedCliConfig *config = embeddedCliDefaultConfig();
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
            "gp-clear",
            "Tell Gamepad Controller to clear all known controllers",
            true,
            &Serial,
            commandClearGamepads
    });

    embeddedCliAddBinding(embeddedCli, {
            "gp-register",
            "Toggle registration of new controller on gp controller. Usage: gp-register: [on|off]",
            true,
            &Serial,
            commandToggleGamepadRegister
    });

    embeddedCliAddBinding(embeddedCli, {
            "button-next",
            "Select next display button",
            true,
            &Serial,
            commandButtonNext
    });

    embeddedCliAddBinding(embeddedCli, {
            "button-prev",
            "Select previous display button",
            true,
            &Serial,
            commandButtonPrev
    });

    embeddedCliAddBinding(embeddedCli, {
            "button-select",
            "Select current display button",
            true,
            &Serial,
            commandButtonSelect
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

    //auto sensor = getSensorValues();

    out->print(0, 2);
    out->print(" °C\t");

    out->print(0, 2);
    out->print(" °C\t");

    out->print(0, 2);
    out->println(" °C\t");
}

void commandPrintCurrent(EmbeddedCli *cli, char *args, void *context) {
    auto *out = (Print *) context;
    out->println("TOP\t\tCENTER\t\tBOTTOM\r\n");

    //auto sensor = getSensorValues();

    out->print(0, 2);
    out->print(" A\t");

    out->print(0, 2);
    out->print(" A\t");

    out->print(0, 2);
    out->println(" A\t");
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

        if (FX_setEffect(effectName)) {
            out->print("Effect set to '");
            out->print(effectName);
            out->println("'!");
        } else {
            out->print("Effect '");
            out->print(effectName);
            out->println("' not found!");
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

        //float value = ((float) brightness) / 100.0f;

        //setBrightness(value);


        out->print("Brightness changed to: ");
        out->print(brightness);
        out->println("%");
    }
}


void commandPrintEffectList(EmbeddedCli *cli, char *args, void *context) {
    auto *out = (Print *) context;
    FX **effects = FX_getEffects();
    int effectCount = FX_getCount();

    out->println("Known effects: ");

    for (int i = 0; i < effectCount; i++) {
        out->print("\t* ");
        out->println(effects[i]->name);
    }

    out->println();
}

void commandToggleHalt(EmbeddedCli *cli, char *args, void *context) {
    auto *out = (Print *) context;
    EffectState *state = FX_getState();

    if (!state->halt) {
        state->halt = true;
        out->println("Halt Enabled!");
    } else {
        state->halt = false;
        out->println("Halt Disabled!");
    }
}

void commandExecuteNext(EmbeddedCli *cli, char *args, void *context) {
    EffectState *state = FX_getState();
    state->singleStep = true;
}

void commandSlowExecution(EmbeddedCli *cli, char *args, void *context) {
    auto *out = (Print *) context;
    EffectState *state = FX_getState();

    if (embeddedCliGetTokenCount(args) == 0) {
        out->println("Missing argument: slow [0-100]");
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

        LED_RGB pixel = {
                .G = (unsigned char) ((color >> 16) & 0xFF),
                .R = (unsigned char) ((color >> 8) & 0xFF),
                .B = (unsigned char) ((color >> 0) & 0xFF)
        };

        LED_Ring *ring_p = LED_getRing(ring);
        if (ring_p != nullptr) {
            LED_fillRing(&pixel, ring_p);
        }
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

        LED_RGB pixel = {
                .G = (unsigned char) ((color >> 16) & 0xFF),
                .R = (unsigned char) ((color >> 8) & 0xFF),
                .B = (unsigned char) ((color >> 0) & 0xFF)
        };

        LED_fillSection(
                &pixel,
                LED_getPixel(LED_getRing(0), 0),
                LED_getPixel(LED_getRing(LED_TOTAL_RINGS - 1), LED_PER_RING - 1)
        );
    }
}

//
// Gamepad control
//
void commandClearGamepads(EmbeddedCli *cli, char *args, void *context) {
    auto *out = (Print *) context;

    GP_clear();

    out->println("Gamepads cleared!");
}

void commandToggleGamepadRegister(EmbeddedCli *cli, char *args, void *context) {
    auto *out = (Print *) context;

    if (embeddedCliGetTokenCount(args) == 0) {
        out->println("Missing argument: gp-register [on|off]");
    } else {
        const char *command = embeddedCliGetToken(args, 1);

        bool turnOn = strcmp(command, "on") == 0;
        if (turnOn) {
            GP_enablePairing();
        } else {
            GP_disablePairing();
        }

        out->print("Controller registration turned ");
        out->print(turnOn ? "on" : "off");
    }
}

//
// Display
//
void commandButtonPrev(EmbeddedCli *cli, char *args, void *context) {
    DSP_nextButton(-1);
}

void commandButtonNext(EmbeddedCli *cli, char *args, void *context) {
    DSP_nextButton(1);
}

void commandButtonSelect(EmbeddedCli *cli, char *args, void *context) {
    DSP_selectButton();
}