#include "display.h"
#include "effects/_effects.h"
#include "globals.h"

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_ST7735.h>

#define RST 40
#define CS1 35
#define CS2 36

Adafruit_ST7735 tft = Adafruit_ST7735(&SPI1, CS1, CS2, RST); // SPI, CS, DC, RST

const char *textPowerOff = "Power: OFF";
const char *textPowerOn = "Power: ON";
const char *textPowerLock = "Power: LOCK";
const char *textPowerSupply = "Power: PWR FAIL";

IntervalTimer powerUnlockTimer;
bool powerToggleLocked = true;

DSP_Page pageEffects = {{"Select Effect", DSP_WHITE}, {}, 0};
DSP_Btn btnBack = {false, {"Back", DSP_WHITE}, &DSP_onButtonMainMenuClick};

DSP_Btn btnPower = {false, {"", DSP_WHITE}, &DSP_onButtonCyclePowerClick, true, false, DSP_BLACK, DSP_BLACK, DSP_GREEN,
                    DSP_RED};

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
    pageEffects.elements = (DSP_Element *) calloc(pageEffects.count + 1, sizeof(DSP_Element));

    FX **effects = FX_getEffects();
    for (int i = 0; i < pageEffects.count; i++) {
        DSP_Element *element = &pageEffects.elements[i];
        element->type = BTN;

        DSP_Btn *btn = &element->data.btn;

        btn->selected = i == 0;
        btn->text = {effects[i]->name, DSP_WHITE};
        btn->onClick = DSP_onEffectBtnClick;

        btn->active = false;
        btn->activeColor = DSP_WHITE;
        btn->inactiveColor = DSP_BLACK;
    }

    // Configure "none" effect
    DSP_Element *element = &pageEffects.elements[pageEffects.count++];
    element->type = BTN;
    DSP_Btn *btn = &element->data.btn;

    btn->selected = false;
    btn->text = {"none", DSP_WHITE};
    btn->onClick = DSP_onEffectBtnClick;

    btn->active = false;
    btn->activeColor = DSP_WHITE;
    btn->inactiveColor = DSP_BLACK;

    displayState.currentButtons = (DSP_Btn **) calloc(16, sizeof(DSP_Btn **));

    // Configure power button
    btnPower.active = false;
    btnPower.text.text = textPowerLock;
    powerUnlockTimer.begin(DSP_reenablePower, PWR_LOCK_TIME);

    // Render
    DSP_renderPage(&pageMainMenu);
}

void DSP_reenablePower() {
    powerUnlockTimer.end();
    bool pwGood = digitalRead(PIN_PS_GOOD);

    // PW Good is inverted due to hardware error
    if (!pwGood || !btnPower.active) {
        btnPower.text.text = btnPower.active ? textPowerOn : textPowerOff;
        powerToggleLocked = false;
    } else {
        powerUnlockTimer.begin(DSP_reenablePower, PWR_LOCK_TIME);
        btnPower.text.text = textPowerSupply;
        powerToggleLocked = true;
    }

    shouldRerenderDisplay = true;
}

void DSP_nextButton(signed char dir) {
    if (displayState.btnCount == 0) {
        return;
    }

    bool found = false;
    if (dir == 1 && displayState.currentButtons[displayState.btnCount - 1]->selected) {

        displayState.currentButtons[displayState.btnCount - 1]->selected = false;
        displayState.currentButtons[0]->selected = true;

        found = true;
    } else if (dir == -1 && displayState.currentButtons[0]->selected) {
        displayState.currentButtons[displayState.btnCount - 1]->selected = true;
        displayState.currentButtons[0]->selected = false;

        found = true;
    } else {
        for (int i = 0; i < displayState.btnCount; i++) {
            DSP_Btn *btn = displayState.currentButtons[dir == 1 ? i : (displayState.btnCount - 1) - i];
            if (found) {
                btn->selected = true;
                break;
            } else {
                found = btn->selected;
                btn->selected = false;
            }
        }
    }

    if (found) {
        DSP_renderPage(displayState.currentPage);
    }
}

void DSP_selectButton() {
    for (int i = 0; i < displayState.btnCount; i++) {
        if (displayState.currentButtons[i]->selected) {
            displayState.currentButtons[i]->onClick(&displayState.currentPage->elements[i].data.btn);
            return;
        }
    }
    DSP_renderPage(&pageMainMenu);
}

void DSP_onButtonCyclePowerClick(DSP_Btn *btn) {
    if (powerToggleLocked) {
        return;
    }

    bool pwOn = digitalRead(PIN_PW_ON);

    if (pwOn) {
        btnPower.active = false;
        digitalWrite(PIN_PW_ON, LOW);
    } else {
        btnPower.active = true;
        digitalWrite(PIN_PW_ON, HIGH);
    }

    btnPower.text.text = textPowerLock;
    powerToggleLocked = true;

    powerUnlockTimer.begin(DSP_reenablePower, PWR_LOCK_TIME);

    shouldRerenderDisplay = true;
}

void DSP_onButtonMainMenuClick(DSP_Btn *btn) {
    DSP_renderPage(&pageMainMenu);
}

void DSP_onEffectMenuClick(DSP_Btn *btn) {
    DSP_renderPage(&pageEffects);
}

void DSP_onEffectBtnClick(DSP_Btn *btn) {
    FX_setEffect(btn->text.text);
    DSP_onButtonMainMenuClick(btn);
}

#define TEXT_SPACING 8

void DSP_renderText(DSP_Text *text, unsigned short color) {
    displayState.currentY += 2;

    tft.setCursor(displayState.currentX, displayState.currentY);
    tft.setTextSize(1);
    tft.setTextColor(color);
    tft.print(text->text);

    displayState.currentY += TEXT_SPACING;
}

void DSP_renderButton(DSP_Btn *btn) {
    displayState.currentY += 2;
    displayState.currentX += 8;

    if (btn->selected) {
        tft.fillRoundRect(displayState.currentX - 4, displayState.currentY,
                          displayState.width - 4 - 2 - displayState.currentX, TEXT_SPACING + 3,
                          4, DSP_WHITE);

        DSP_renderText(&btn->text, DSP_BLACK);
        displayState.currentY += 2;
    } else {
        unsigned short textColor = 0;

        if (btn->hasActiveMode) {
            tft.fillRoundRect(displayState.currentX - 4, displayState.currentY,
                              displayState.width - 4 - 2 - displayState.currentX, TEXT_SPACING + 3,
                              4, btn->active ? btn->activeColor : btn->inactiveColor);

            textColor = btn->active ? btn->activeTextColor : btn->inactiveTextColor;
        } else {
            tft.drawRoundRect(displayState.currentX - 4, displayState.currentY,
                              displayState.width - 4 - 2 - displayState.currentX, TEXT_SPACING + 3,
                              4, DSP_WHITE);
            textColor = DSP_WHITE;
        }

        DSP_renderText(&btn->text, textColor);
        displayState.currentY += 2;
    }

    displayState.currentX -= 8;
}

void DSP_renderPage(DSP_Page *page) {
    if (page == nullptr) {
        if (displayState.currentPage != nullptr) {
            DSP_renderPage(displayState.currentPage);
        }
        return;
    }

    displayState.currentX = 4;
    displayState.currentY = 4;

    tft.fillScreen(DSP_BLACK);
    tft.drawRect(displayState.currentX, displayState.currentY, displayState.width - 4, displayState.height - 4,
                 DSP_WHITE);

    displayState.currentX += 4;
    displayState.currentY += 2;
    DSP_renderText(&page->title, page->title.color);
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
                DSP_renderText(&element->data.text, element->data.text.color);
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
        displayState.currentButtons[displayState.btnCount++] = &btnPower;
        DSP_renderButton(&btnPower);
    }
}
