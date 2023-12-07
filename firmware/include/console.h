#ifndef FIRMWARE_CONSOLE_H
#define FIRMWARE_CONSOLE_H

#include <OctoWS2811.h>

void processConsoleData();
void initConsole();
void processCommand();


void commandPrintTemperatures();
void commandPrintNetworkInfo();
void commandReboot();
void commandSetEffect();
void commandSetBrightness();
void commandPrintCurrent();
void commandSensorStatus();
void commandTogglePowerSupply();
void commandPrintEffectList();
void commandToggleHalt();
void commandExecuteNext();
void commandSlowExecution();

#endif