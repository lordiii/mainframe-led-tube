#include <OctoWS2811.h>

typedef bool (*EFFECT_CALLBACK)(OctoWS2811, unsigned long);

void processConsoleData();
void initConsole();
void processCommand();
void printTemperatures();
void printNetworkInfo();
void reboot();
void setEffect();