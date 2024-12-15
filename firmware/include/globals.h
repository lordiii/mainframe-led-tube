#ifndef GLOBALS_H
#define GLOBALS_H

#define LED_TOTAL_RINGS 60
#define LED_PER_RING 32
#define LED_TOTAL_AMOUNT (LED_PER_RING * LED_TOTAL_RINGS)
#define LED_RINGS_PER_SEGMENT 10
#define LED_PINS {2, 3, 4, 6, 7, 8}
#define LED_FRAMES_PER_SECOND ((1000 / 80) * 1000)

#define CURRENT_SENSOR_BOTTOM_ADDRESS 0b1000000
#define CURRENT_SENSOR_CENTER_ADDRESS 0b1000101
#define CURRENT_SENSOR_TOP_ADDRESS 0b1000001

#define TEMPERATURE_SENSOR_TOP_ADDRESS      {0x28, 0xFF, 0x64, 0x1E, 0x30, 0x79, 0x79, 0xD5}
#define TEMPERATURE_SENSOR_CENTER_ADDRESS   {0x28, 0xFF, 0x64, 0x1E, 0x31, 0x90, 0xC5, 0x28}
#define TEMPERATURE_SENSOR_BOTTOM_ADDRESS   {0x28, 0xFF, 0x64, 0x1E, 0x30, 0x7F, 0x39, 0x39}

#define I2C_CONTROLLER 0x55

#define PIN_PW_ON 28
#define PIN_PS_GOOD 29

#define WEB_CHUNK_SIZE 1024

#endif
