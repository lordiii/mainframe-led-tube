#ifndef FIRMWARE_CLI_H
#define FIRMWARE_CLI_H

#include <embedded_cli.h>
#include <Arduino.h>

void initCLI();

void processCLI();

void writeCmdOutChar(EmbeddedCli *embeddedCli, char c);

//
// Sensor Values
//
void commandPrintTemperature(EmbeddedCli *cli, char *args, void *context);

void commandPrintCurrent(EmbeddedCli *cli, char *args, void *context);

//
// Network Configuration
//
void commandPrintNetworkInfo(EmbeddedCli *cli, char *args, void *context);

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


#endif //FIRMWARE_CLI_H
