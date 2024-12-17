#include "display.h"
#include "effects/_effects.h"

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_ST7735.h>

#define RST 40
#define CS1 35
#define CS2 36

Adafruit_ST7735 tft = Adafruit_ST7735(&SPI1, CS1, CS2, RST); // SPI, CS, DC, RST

DSP_Page pageEffects = {{"Select Effect", DSP_WHITE}, {}, 0};
DSP_Btn btnBack = {false, {"Back", DSP_WHITE}, &DSP_onButtonBackClick};
DSP_Btn btnReboot = {false, {"Reboot", DSP_WHITE}, &DSP_onButtonRebootClick};
DSP_Element effectMenuBtn = {
        (DSP_Element_Data) {
                .btn = {true, {"Select Effect", DSP_WHITE}, &DSP_onEffectMenuClick}
        },
        BTN
};
DSP_Page pageMainMenu = {{"Main Menu", DSP_WHITE}, &effectMenuBtn, 1};

// State Information
DSP_State displayState;

void DSP_init() {
    tft.initR(INITR_BLACKTAB);
    tft.fillScreen(DSP_BLACK);
    tft.setTextWrap(false);

    displayState.height = tft.height();
    displayState.width = tft.width();

    pageEffects.count = FX_getCount();
    pageEffects.elements = (DSP_Element *) calloc(pageEffects.count * 2, sizeof(DSP_Element));

    FX **effects = FX_getEffects();
    for (int i = 0; i < pageEffects.count; i++) {
        DSP_Element *element = &pageEffects.elements[i];
        element->type = BTN;

        DSP_Btn *btn = &element->data.btn;

        btn->active = i == 0;
        btn->text = {effects[i]->name, DSP_WHITE};
        btn->onClick = DSP_onEffectBtnClick;
    }

    displayState.currentButtons = (DSP_Btn **) calloc(16, sizeof(DSP_Btn **));

    DSP_renderPage(&pageMainMenu);
}

void DSP_nextButton(signed char dir) {
    if (displayState.btnCount == 0) {
        return;
    }

    bool found = false;
    if (dir == 1 && displayState.currentButtons[displayState.btnCount - 1]->active) {

        displayState.currentButtons[displayState.btnCount - 1]->active = false;
        displayState.currentButtons[0]->active = true;

        found = true;
    } else if (dir == -1 && displayState.currentButtons[0]->active) {
        displayState.currentButtons[displayState.btnCount - 1]->active = true;
        displayState.currentButtons[0]->active = false;

        found = true;
    } else {
        for (int i = 0; i < displayState.btnCount; i++) {
            DSP_Btn *btn = displayState.currentButtons[dir == 1 ? i : (displayState.btnCount - 1) - i];
            if (found) {
                btn->active = true;
                break;
            } else {
                found = btn->active;
                btn->active = false;
            }
        }
    }

    if (found) {
        DSP_renderPage(displayState.currentPage);
    }
}

void DSP_selectButton() {
    for (int i = 0; i < displayState.btnCount; i++) {
        if (displayState.currentButtons[i]->active) {
            displayState.currentButtons[i]->onClick(&displayState.currentPage->elements[i].data.btn);
            return;
        }
    }

    DSP_renderPage(&pageMainMenu);
}

void DSP_onButtonRebootClick(DSP_Btn *btn) {
    _reboot_Teensyduino_();
}

void DSP_onButtonBackClick(DSP_Btn *btn) {
    DSP_renderPage(&pageMainMenu);
}

void DSP_onEffectMenuClick(DSP_Btn *btn) {
    DSP_renderPage(&pageEffects);
}

void DSP_onEffectBtnClick(DSP_Btn *btn) {
    FX_setEffect(btn->text.text);
    DSP_onButtonBackClick(btn);
}

#define TEXT_SPACING 8

void DSP_renderText(DSP_Text *text) {
    displayState.currentY += 2;

    tft.setCursor(displayState.currentX, displayState.currentY);
    tft.setTextSize(1);
    tft.setTextColor(text->color);
    tft.print(text->text);

    displayState.currentY += TEXT_SPACING;
}

void DSP_renderButton(DSP_Btn *btn) {
    displayState.currentY += 2;
    displayState.currentX += 8;

    if (btn->active) {
        tft.fillRoundRect(displayState.currentX - 4, displayState.currentY,
                          displayState.width - 4 - 2 - displayState.currentX, TEXT_SPACING + 3,
                          4, DSP_WHITE);

        btn->text.color = DSP_BLACK;
        DSP_renderText(&btn->text);
        displayState.currentY += 2;
    } else {
        tft.drawRoundRect(displayState.currentX - 4, displayState.currentY,
                          displayState.width - 4 - 2 - displayState.currentX, TEXT_SPACING + 3,
                          4, DSP_WHITE);

        btn->text.color = DSP_WHITE;
        DSP_renderText(&btn->text);
        displayState.currentY += 2;
    }

    displayState.currentX -= 8;
}

void DSP_renderPage(DSP_Page *page) {
    displayState.currentX = 4;
    displayState.currentY = 4;

    tft.fillScreen(DSP_BLACK);
    tft.drawRect(displayState.currentX, displayState.currentY, displayState.width - 4, displayState.height - 4,
                 DSP_WHITE);

    displayState.currentX += 4;
    displayState.currentY += 2;
    DSP_renderText(&page->title);
    displayState.currentX -= 4;

    displayState.currentY += 2;
    tft.drawLine(displayState.currentX, displayState.currentY, displayState.width - 4, displayState.currentY,
                 DSP_WHITE);
    displayState.currentY += 4;

    displayState.btnCount = 0;
    for (int i = 0; i < page->count; i++) {
        DSP_Element *element = &page->elements[i];
        DSP_Btn *btn = &element->data.btn;

        switch (element->type) {
            case TEXT:
                DSP_renderText(&element->data.text);
                break;
            case BTN:
                displayState.currentButtons[displayState.btnCount++] = btn;
                DSP_renderButton(btn);
                break;
            default:
                continue;
        }
    }

    displayState.currentPage = page;

    displayState.currentY = displayState.height - 4 - 12;
    if (displayState.currentPage != &pageMainMenu) {
        displayState.currentButtons[displayState.btnCount++] = &btnBack;
        DSP_renderButton(&btnBack);
    } else {
        displayState.currentButtons[displayState.btnCount++] = &btnReboot;
        DSP_renderButton(&btnReboot);
    }
}
