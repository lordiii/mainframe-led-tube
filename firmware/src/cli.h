#ifndef CLI_H
#define CLI_H

#include <embedded_cli.h>

void initCLI();

void processCLI();

void writeCmdOutChar(EmbeddedCli *embeddedCli, char c);

//
// Sensor Values
//
void commandPrintTemperature(EmbeddedCli *cli, char *args, void *context);

void commandPrintCurrent(EmbeddedCli *cli, char *args, void *context);

//
// Device Control
//
void commandReboot(EmbeddedCli *cli, char *args, void *context);

void commandTogglePowerSupply(EmbeddedCli *cli, char *args, void *context);

//
// Effect Control
//
void commandSetEffect(EmbeddedCli *cli, char *args, void *context);

void commandSetBrightness(EmbeddedCli *cli, char *args, void *context);

void commandToggleHalt(EmbeddedCli *cli, char *args, void *context);

void commandExecuteNext(EmbeddedCli *cli, char *args, void *context);

void commandSlowExecution(EmbeddedCli *cli, char *args, void *context);

void commandPrintEffectList(EmbeddedCli *cli, char *args, void *context);

void commandSetRing(EmbeddedCli *cli, char *args, void *context);

void commandSetAll(EmbeddedCli *cli, char *args, void *context);

//
// Gamepad control
//
void commandClearGamepads(EmbeddedCli *cli, char *args, void *context);

void commandToggleGamepadRegister(EmbeddedCli *cli, char *args, void *context);

//
// Display
//
void commandButtonNext(EmbeddedCli *cli, char *args, void *context);

void commandButtonPrev(EmbeddedCli *cli, char *args, void *context);

void commandButtonSelect(EmbeddedCli *cli, char *args, void *context);

#endif
