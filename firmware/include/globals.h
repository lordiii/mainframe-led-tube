#define LED_TOTAL_RINGS 60
#define LED_PER_RING 32
#define LED_RINGS_PER_SEGMENT 10
#define LED_PINS {2, 3, 4, 6, 7, 8}
#define LED_CONFIGURATION (WS2811_GRB | WS2811_800kHz)

#define CURRENT_SENSOR_1_ADDRESS 0b1000000
#define CURRENT_SENSOR_2_ADDRESS 0b1000101
#define CURRENT_SENSOR_3_ADDRESS 0b1000001

#define TEMPERATURE_SENSOR_TOP_ADDRESS      {0x28, 0xFF, 0x64, 0x1E, 0x30, 0x79, 0x79, 0xD5}
#define TEMPERATURE_SENSOR_CENTER_ADDRESS   {0x28, 0xFF, 0x64, 0x1E, 0x31, 0x90, 0xC5, 0x28}
#define TEMPERATURE_SENSOR_BOTTOM_ADDRESS   {0x28, 0xFF, 0x64, 0x1E, 0x30, 0x7F, 0x39, 0x39}

#include <QNEthernet.h>
using namespace qindesign::network;