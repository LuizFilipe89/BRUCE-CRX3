#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include "soc/soc_caps.h"
#include <stdint.h>

#define USB_VID 0x303a
#define USB_PID 0x1001

static const uint8_t TX = 43;
static const uint8_t RX = 44;

static const uint8_t TXD2 = 1;
static const uint8_t RXD2 = 2;

static const uint8_t SDA = 13;
static const uint8_t SCL = 15;  // Default I2C - NOT used (GROVE_SCL=26 is used instead)

// Modified elsewhere
static const uint8_t SS = 1;
static const uint8_t MOSI = 23;
static const uint8_t MISO = 19;
static const uint8_t SCK = 18;

static const uint8_t G0 = 0;
static const uint8_t G1 = 1;
static const uint8_t G2 = 2;
static const uint8_t G3 = 3;
static const uint8_t G4 = 4;
static const uint8_t G5 = 5;
static const uint8_t G6 = 6;
static const uint8_t G7 = 7;
static const uint8_t G8 = 8;
static const uint8_t G9 = 9;
static const uint8_t G10 = 10;
static const uint8_t G11 = 11;
static const uint8_t G12 = 12;
static const uint8_t G13 = 13;
static const uint8_t G14 = 14;
static const uint8_t G15 = 15;
static const uint8_t G39 = 39;
static const uint8_t G40 = 40;
static const uint8_t G41 = 41;
static const uint8_t G42 = 42;
static const uint8_t G43 = 43;
static const uint8_t G44 = 44;
static const uint8_t G46 = 46;

static const uint8_t ADC1 = 7;
static const uint8_t ADC2 = 8;

#define RGB_LED -1  // No RGB LED on this board

#define BAD_TX 26   // Not wired - spare GPIO
#define BAD_RX 27   // Not wired - spare GPIO

// No GPS on this board - SERIAL pins unused (GPIO1/GPIO3 are used for K3/K2)
#define SERIAL_TX -1
#define SERIAL_RX -1
#define GPS_SERIAL_TX -1
#define GPS_SERIAL_RX -1

// 4 physical buttons (K1-K4), see dispositivo-display-esp32.pdf.
// Only 4 of Bruce's 5 logical roles are wired; L_BTN points at a spare
// GPIO with internal pull-up so it always reads "not pressed" (see
// interface.cpp).
#define HAS_BTN 1
// K1=Up, K2=Down, K3=Select, K4=Escape/Cancel
// Correspondência física pode estar trocada por erro de solda -
// testar cada botão e ajustar aqui se necessário.
#define SEL_BTN 1   // K3 (TX0 - cuidado durante flash)
#define UP_BTN 21   // K1
#define DW_BTN 3    // K2 (RX0 - cuidado durante flash)
#define R_BTN 22    // K4
#define L_BTN 32    // Not physically wired - spare pin, always "not pressed"
// Only 4 real buttons, no dedicated Left/Right - NOT a true 5-button board.
// Uses a dedicated short/long-press keyboard nav scheme (see mykeyboard.cpp)
// instead of HAS_5_BUTTONS, which assumes independent Left/Right buttons.
#define DEVKIT_CUSTOM_KB 1
#define BTN_ALIAS "\"Ok\""
#define BTN_ACT LOW

#define TXLED -1
#define LED_ON HIGH
#define LED_OFF LOW

// CC1101 (Ebyte E07-M1101D-SMA V2.0, 433MHz)
// EXCLUSIVE SPI bus - does NOT share SCK/MOSI with display.
// GDO2 not connected => single-pin mode (GDO0 used for both Tx and Rx).
#define CC1101_SCK_PIN 19
#define CC1101_MOSI_PIN 17
#define CC1101_MISO_PIN 16
#define CC1101_SS_PIN 25
#define CC1101_GDO0_PIN 15

#define NRF24_CE_PIN -1
#define NRF24_SS_PIN -1
#define NRF24_MOSI_PIN SPI_MOSI_PIN
#define NRF24_SCK_PIN SPI_SCK_PIN
#define NRF24_MISO_PIN SPI_MISO_PIN

#define FP 1
#define FM 1
#define FG 2

#define HAS_SCREEN 1
// Bruce's UI assumes landscape (width > height) throughout. Our panel is
// natively portrait (128x160); rotate 90 degrees so it reports as
// landscape (160x128) to match. Earlier attempt at ROTATION=1 looked like
// it "didn't work" because Bruce persists rotation to flash keyed by MAC
// address - the stale saved value (0) from earlier boots silently
// overrode this compile-time default. Must flash with a full chip erase
// for this to actually take effect.
// ROTATION 3 = landscape flipped 180 from ROTATION 1, so the buttons end
// up next to the bottom of the menu instead of the top (much easier to
// reach while reading the screen).
#define ROTATION 3
#define MINBRIGHT 160

// No SD card on this board
#define SDCARD_CS -1
#define SDCARD_SCK -1
#define SDCARD_MISO -1
#define SDCARD_MOSI -1

#define GROVE_SDA 33
#define GROVE_SCL 26

#define SPI_SCK_PIN 18
#define SPI_MISO_PIN 19
#define SPI_MOSI_PIN 23
#define SPI_SS_PIN -1  // Not used - display is write-only (no MISO/SS)

#endif /* Pins_Arduino_h */
