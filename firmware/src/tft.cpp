#include "tft.h"
#include "globals.h"

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
    tft.setTextColor(ST7735_GREEN);
    tft.setTextWrap(false);

    tft.drawRect(RECT_OFFSET, RECT_OFFSET, RECT_WIDTH, RECT_HEIGHT, ST7735_WHITE);

    tft.drawLine(RECT_OFFSET, RECT_TOP_START_Y + (RECT_HEIGHT / 3), (RECT_TOP_START_X + RECT_WIDTH) - 1,
                 RECT_TOP_START_Y + (RECT_HEIGHT / 3), ST7735_WHITE);
    tft.drawLine(RECT_OFFSET, RECT_CENTER_START_Y + (RECT_HEIGHT / 3), (RECT_CENTER_START_X + RECT_WIDTH) - 1,
                 RECT_CENTER_START_Y + (RECT_HEIGHT / 3), ST7735_WHITE);

    renderCurrentValue(TOP_SECTION, 0);
    renderVoltageValue(TOP_SECTION, 0);
    renderTemperatureValue(TOP_SECTION, 0);

    renderCurrentValue(CENTER_SECTION, 0);
    renderVoltageValue(CENTER_SECTION, 0);
    renderTemperatureValue(CENTER_SECTION, 0);

    renderCurrentValue(BOTTOM_SECTION, 0);
    renderVoltageValue(BOTTOM_SECTION, 0);
    renderTemperatureValue(BOTTOM_SECTION, 0);

    XY xy = getStartPosition(TOP_SECTION);
    tft.setCursor(xy.X + RECT_WIDTH + 8, xy.Y + 10);
    tft.setTextColor(ST7735_WHITE);
    tft.print("Effect:");
}

XY getStartPosition(TUBE_SECTION section)
{
    switch (section)
    {
    case TOP_SECTION:
        return {RECT_TOP_START_X, RECT_TOP_START_Y};
    case CENTER_SECTION:
        return {RECT_CENTER_START_X, RECT_CENTER_START_Y};
    case BOTTOM_SECTION:
        return {RECT_BOTTOM_START_X, RECT_BOTTOM_START_Y};
    }

    return {0, 0};
}

void displayEffect(const char* effectName)
{
    XY xy = getStartPosition(TOP_SECTION);
    tft.fillRect(xy.X + RECT_WIDTH + 8, xy.Y + 20, RECT_WIDTH, 10, ST7735_BLACK);
    tft.setCursor(xy.X + RECT_WIDTH + 8, xy.Y + 20);
    tft.setTextColor(ST7735_GREEN);
    tft.print(effectName);
}

void renderCurrentValue(TUBE_SECTION section, float value)
{
    XY xy = getStartPosition(section);
    tft.fillRect(xy.X + 10, xy.Y + 10, RECT_WIDTH - 20, 10, ST7735_BLACK);

    tft.setCursor(xy.X + 10, xy.Y + 10);

    tft.setFont();
    tft.setTextColor(ST7735_YELLOW);
    tft.print(value, 2);
    tft.print("A");
}

void renderVoltageValue(TUBE_SECTION section, float value)
{
    XY xy = getStartPosition(section);
    tft.fillRect(xy.X + 10, xy.Y + 20, RECT_WIDTH - 20, 10, ST7735_BLACK);

    tft.setCursor(xy.X + 10, xy.Y + 20);

    tft.setTextColor(ST7735_CYAN);
    tft.print(value, 2);
    tft.print("V");
}

void renderTemperatureValue(TUBE_SECTION section, float value)
{
    XY xy = getStartPosition(section);
    tft.fillRect(xy.X + 10, xy.Y + 30, RECT_WIDTH - 20, 10, ST7735_BLACK);

    tft.setCursor(xy.X + 10, xy.Y + 30);

    tft.setTextColor(ST7735_RED);
    tft.print(value, 2);
    tft.drawCircle(tft.getCursorX() + 1, tft.getCursorY() + 1, 1, ST7735_RED);
    tft.setCursor(tft.getCursorX() + 4, tft.getCursorY());
    tft.print("C");
}
