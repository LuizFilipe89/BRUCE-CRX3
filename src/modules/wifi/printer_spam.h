#ifndef __PRINTER_SPAM_H__
#define __PRINTER_SPAM_H__

#include <Arduino.h>
#include <WiFi.h>
#include <vector>

enum PrintContent {
    PRINT_SKULL = 0,
    PRINT_CUSTOM,
};

struct PrinterInfo {
    String ip;
    uint16_t port;
};

void printerSpamMenu();

#endif
