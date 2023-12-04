#include <tft.h>

#include <Adafruit_ST7735.h>

// Screen
#define RST 40
#define CS1 35
#define CS2 36
Adafruit_ST7735 tft = Adafruit_ST7735(&SPI1, CS1, CS2, RST); // SPI, CS, DC, RST

#define RECT_OFFSET 5
#define RECT_HEIGHT 150
#define RECT_WIDTH 60

#define RECT_TOP_START_Y (RECT_OFFSET)
#define RECT_TOP_START_X (RECT_OFFSET)

#define RECT_CENTER_START_Y (RECT_TOP_START_Y + (RECT_HEIGHT / 3))
#define RECT_CENTER_START_X (RECT_OFFSET)

#define RECT_BOTTOM_START_Y (RECT_CENTER_START_Y + (RECT_HEIGHT / 3))
#define RECT_BOTTOM_START_X (RECT_OFFSET)

void initTFT()
{
    tft.initR(INITR_BLACKTAB);
    tft.fillScreen(ST7735_BLACK);

    tft.drawRect(RECT_OFFSET, RECT_OFFSET, RECT_WIDTH, RECT_HEIGHT, 0xFFFF);

    tft.drawLine(RECT_OFFSET, RECT_TOP_START_Y + (RECT_HEIGHT / 3), (RECT_TOP_START_X + RECT_WIDTH) - 1, RECT_TOP_START_Y+ (RECT_HEIGHT / 3), 0xFFFF);
    tft.drawLine(RECT_OFFSET, RECT_CENTER_START_Y + (RECT_HEIGHT / 3), (RECT_CENTER_START_X + RECT_WIDTH) - 1, RECT_CENTER_START_Y + (RECT_HEIGHT / 3), 0xFFFF);

    renderCurrentValue(TOP, 0);
    renderVoltageValue(TOP, 0);
    renderTemperatureValue(TOP, 0);

    renderCurrentValue(CENTER, 0);
    renderVoltageValue(CENTER, 0);
    renderTemperatureValue(CENTER, 0);

    renderCurrentValue(BOTTOM, 0);
    renderVoltageValue(BOTTOM, 0);
    renderTemperatureValue(BOTTOM, 0);
}

XY getStartPosition(TUBE_SECTION section)
{
    switch (section)
    {
    case TOP:
        return {RECT_TOP_START_X, RECT_TOP_START_Y};
    case CENTER:
        return {RECT_CENTER_START_X, RECT_CENTER_START_Y};
    case BOTTOM:
        return {RECT_BOTTOM_START_X, RECT_BOTTOM_START_Y};
    }

    return {0, 0};
}

void renderCurrentValue(TUBE_SECTION section, float value)
{
    XY xy = getStartPosition(section);
    tft.fillRect(xy.X + 10, xy.Y + 10, RECT_WIDTH - 20, 10, ST7735_BLACK);

    tft.setCursor(xy.X + 10, xy.Y + 10);

    tft.print(value, 3);
    tft.println("A");
}

void renderVoltageValue(TUBE_SECTION section, float value)
{
    XY xy = getStartPosition(section);
    tft.fillRect(xy.X + 10, xy.Y + 20, RECT_WIDTH - 20, 10, ST7735_BLACK);

    tft.setCursor(xy.X + 10, xy.Y + 20);
    
    tft.print(value, 3);
    tft.println("V");
}

void renderTemperatureValue(TUBE_SECTION section, float value)
{
    XY xy = getStartPosition(section);
    tft.fillRect(xy.X + 10, xy.Y + 30, RECT_WIDTH - 20, 10, ST7735_BLACK);
    
    tft.setCursor(xy.X + 10, xy.Y + 30);

    tft.print(value, 3);
    tft.println("C");
}