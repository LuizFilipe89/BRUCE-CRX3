#include "core/powerSave.h"
#include <interface.h>

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    // All 4 physical buttons rely on the ESP32's internal pull-up (no
    // external pull-up resistors on this board), unlike stock Marauder
    // Mini hardware which this target is modeled on.
    pinMode(UP_BTN, INPUT_PULLUP);
    pinMode(SEL_BTN, INPUT_PULLUP);
    pinMode(DW_BTN, INPUT_PULLUP);
    pinMode(R_BTN, INPUT_PULLUP);
    // L_BTN has no physical button wired (only 4 of Bruce's 5 button
    // roles are available) - it points at a spare GPIO with the internal
    // pull-up enabled so it always reads "not pressed" instead of
    // floating/undefined.
    pinMode(L_BTN, INPUT_PULLUP);

    bruceConfig.colorInverted = 0;
    bruceConfigPins.rotation = 3;
}

/***************************************************************************************
** Function name: _post_setup_gpio()
** Location: main.cpp
** Description:   second stage gpio setup to make a few functions work
***************************************************************************************/
void _post_setup_gpio() { pinMode(TFT_BL, OUTPUT); }

/***************************************************************************************
** Function name: getBattery()
** location: display.cpp
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() { return 0; }

/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** set brightness value
** No real backlight control pin (BLK tied straight to 3V3), so this
** is a no-op beyond toggling the disconnected spare GPIO assigned to
** TFT_BL.
**********************************************************************/
void _setBrightness(uint8_t brightval) {
    pinMode(TFT_BL, OUTPUT);
    if (brightval > 5) {
        digitalWrite(TFT_BL, LOW);
        digitalWrite(TFT_BL, HIGH);
    } else {
        digitalWrite(TFT_BL, HIGH);
        digitalWrite(TFT_BL, LOW);
    }
}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {
    static unsigned long tm = millis();
    if (!(millis() - tm > 200 || LongPress)) return;

    bool u = digitalRead(UP_BTN);
    bool d = digitalRead(DW_BTN);
    bool r = digitalRead(R_BTN);
    bool l = digitalRead(L_BTN); // never pressed - no physical K5 wired
    bool s = digitalRead(SEL_BTN);
    if (!s || !u || !d || !r || !l) {
        tm = millis();
        if (!wakeUpScreen()) AnyKeyPress = true;
        else return;
    }

    // Fixed, non-overlapping 1-button-1-role mapping (K1=Up, K2=Down,
    // K3=Select, K4=Escape/Cancel). loopOptions() (display.cpp) already
    // treats Up/Down as equivalent to Prev/Next, but several screens
    // (BLE_Suite.cpp, ble_spam.cpp, ble_sniffer.cpp, and others) check
    // NextPress/PrevPress directly instead of going through loopOptions,
    // and without also setting those two here, K1/K2 do nothing on those
    // specific screens. Up=Prev, Down=Next matches the direction
    // convention already used at display.cpp:660/702.
    if (!u) {
        UpPress = true;
        PrevPress = true;
    }
    if (!d) {
        DownPress = true;
        NextPress = true;
    }
    if (!s) SelPress = true;
    if (!r) EscPress = true;
}

/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() {}

/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to turn off the device (name is odd btw)
**********************************************************************/
void checkReboot() {}
