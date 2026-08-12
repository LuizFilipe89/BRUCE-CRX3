#include "printer_spam.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/utils.h"

namespace {
constexpr uint16_t RAW_PRINT_PORT = 9100;
constexpr int MAX_PRINTERS = 10;
constexpr int MAX_COPIES = 20;
constexpr int SCAN_CONNECT_TIMEOUT_MS = 70;
constexpr int SEND_CONNECT_TIMEOUT_MS = 2500;
constexpr size_t MAX_TEXT_LENGTH = 240;

std::vector<PrinterInfo> foundPrinters;
int selectedPrinter = -1;
int printCopies = 1;
PrintContent selectedContent = PRINT_SKULL;
String customText;

const char SKULL_ART[] PROGMEM = R"(
             _______
          .-'       '-.
         /   O     O   \
        |             |
        |     ___     |
         \   /___\   /
          '-._____.-'
            /| |\
           /_| |_\
)";

const char *contentName() { return selectedContent == PRINT_SKULL ? "ASCII Skull" : "Custom Text"; }

bool validPrinterSelection() {
    return selectedPrinter >= 0 && selectedPrinter < (int)foundPrinters.size();
}

void drawScanStatus(const String &ip, int host, bool canFinish) {
    drawMainBorderWithTitle("Printer Scan");
    const int x = BORDER_PAD_X + BORDER_OFFSET_FROM_SCREEN_EDGE;
    int y = BORDER_PAD_Y;
    tft.setTextSize(FP);
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    tft.setCursor(x, y);
    tft.println("Scanning port 9100");
    tft.setCursor(x, y + 16);
    tft.println(ip);
    tft.setCursor(x, y + 32);
    tft.println("Progress: " + String((host * 100) / 254) + "%");
    tft.setCursor(x, y + 48);
    tft.println("Found: " + String(foundPrinters.size()));
    tft.setCursor(x, y + 68);
    tft.println(canFinish ? "K1: use found" : "ESC: cancel");
}

void scanPrinters() {
    foundPrinters.clear();
    selectedPrinter = -1;
    UpPress = false; // K1 on DevKit-Custom; discard a stale menu press.

    const IPAddress local = WiFi.localIP();
    if (local[0] == 0 || WiFi.status() != WL_CONNECTED) {
        displayError("WiFi is not connected");
        return;
    }

    bool stoppedEarly = false;
    bool canceled = false;
    drawScanStatus(local.toString(), 0, false);

    for (int host = 1; host < 255 && foundPrinters.size() < MAX_PRINTERS; host++) {
        if (check(EscPress)) {
            canceled = true;
            break;
        }
        if (!foundPrinters.empty() && check(UpPress)) {
            stoppedEarly = true;
            break;
        }

        IPAddress candidate(local[0], local[1], local[2], host);
        if (candidate == local) continue;

        const String ip = candidate.toString();
        if (host == 1 || host % 4 == 0) drawScanStatus(ip, host, !foundPrinters.empty());

        WiFiClient client;
        if (client.connect(candidate, RAW_PRINT_PORT, SCAN_CONNECT_TIMEOUT_MS)) {
            client.stop();
            foundPrinters.push_back({ip, RAW_PRINT_PORT});
            if (selectedPrinter < 0) selectedPrinter = 0;
            drawScanStatus(ip, host, true);
        }
        delay(1);
    }

    UpPress = false;
    if (foundPrinters.empty()) {
        if (!canceled) displayError("No RAW printers found");
        return;
    }

    String message = String(foundPrinters.size()) + " printer(s) found";
    if (stoppedEarly) message += " - scan stopped";
    displaySuccess(message);
}

void selectPrinter() {
    if (foundPrinters.empty()) return;

    options.clear();
    for (size_t i = 0; i < foundPrinters.size(); i++) {
        const String label = String(i == (size_t)selectedPrinter ? "> " : "") + foundPrinters[i].ip;
        options.push_back({label, [i]() { selectedPrinter = (int)i; }});
    }
    loopOptions(options, MENU_TYPE_SUBMENU, "Select Printer", max(selectedPrinter, 0));
}

void selectPrintContent() {
    options = {
        {"ASCII Skull", []() { selectedContent = PRINT_SKULL; }},
        {"Custom Text", []() {
             String value = keyboard(customText, MAX_TEXT_LENGTH, "Text to print:");
             if (value != "\x1B" && !value.isEmpty()) {
                 customText = value;
                 selectedContent = PRINT_CUSTOM;
             }
         }},
    };
    loopOptions(options, MENU_TYPE_SUBMENU, "Print Content", (int)selectedContent);
}

void selectCopies() {
    const String value = num_keyboard(String(printCopies), 2, "Copies (1-20):");
    if (value == "\x1B" || value.isEmpty()) return;
    const int copies = value.toInt();
    if (copies < 1 || copies > MAX_COPIES) {
        displayError("Copies must be 1-20");
        return;
    }
    printCopies = copies;
}

String normalizedContent() {
    String content = selectedContent == PRINT_SKULL ? String(SKULL_ART) : customText;
    content.replace("\r\n", "\n");
    content.replace("\r", "\n");
    content.replace("\n", "\r\n");
    return content;
}

bool sendPrintJob() {
    if (!validPrinterSelection() || WiFi.status() != WL_CONNECTED) return false;
    const String content = normalizedContent();
    if (content.isEmpty()) return false;

    WiFiClient client;
    const PrinterInfo &printer = foundPrinters[selectedPrinter];
    if (!client.connect(printer.ip.c_str(), printer.port, SEND_CONNECT_TIMEOUT_MS)) return false;

    // One explicit PCL connection/job for every requested page. Repeating the
    // page inside the same job is compatible with printers that ignore the PCL
    // hardware-copy command, while still avoiding one TCP handshake per copy.
    client.print("\x1B%-12345X@PJL JOB NAME=\"BRUCE PRINT\"\r\n");
    client.print("@PJL ENTER LANGUAGE=PCL\r\n");
    client.print("\x1B" "E"); // PCL reset
    client.print("\x1B&l0O");   // portrait orientation
    for (int copy = 0; copy < printCopies; copy++) {
        if (client.print(content) != content.length()) {
            client.stop();
            return false;
        }
        client.print("\r\n\x0C"); // finish this page
    }
    client.print("\x1B" "E");
    client.print("\x1B%-12345X@PJL EOJ\r\n\x1B%-12345X");
    client.flush();
    delay(30);
    client.stop();
    return true;
}

bool confirmPrint() {
    if (!validPrinterSelection()) {
        displayError("Select a printer first");
        return false;
    }
    if (selectedContent == PRINT_CUSTOM && customText.isEmpty()) {
        displayError("Enter custom text first");
        return false;
    }

    bool confirmed = false;
    options = {
        {"Printer: " + foundPrinters[selectedPrinter].ip, []() {}},
        {"Content: " + String(contentName()), []() {}},
        {"Copies: " + String(printCopies), []() {}},
        {"SEND NOW", [&confirmed]() { confirmed = true; }},
        {"Cancel", []() {}},
    };
    loopOptions(options, MENU_TYPE_SUBMENU, "Confirm Print", 3);
    return confirmed;
}

void reviewAndSend() {
    if (!confirmPrint()) return;
    displayInfo("Sending print job...");
    if (sendPrintJob()) displaySuccess("Print job sent");
    else displayError("Printer did not accept job");
}
} // namespace

void printerSpamMenu() {
    if (WiFi.status() != WL_CONNECTED) {
        displayError("Connect WiFi first");
        return;
    }

    bool leave = false;
    while (!leave && !returnToMenu) {
        const String printerLabel = validPrinterSelection() ? foundPrinters[selectedPrinter].ip : "Not selected";
        const String contentLabel = selectedContent == PRINT_CUSTOM && !customText.isEmpty()
                                        ? "Custom: " + customText.substring(0, 12)
                                        : String(contentName());

        options = {
            {"1. Scan Printers", []() { scanPrinters(); }},
            {"2. Printer: " + printerLabel, []() { selectPrinter(); }},
            {"3. Content: " + contentLabel, []() { selectPrintContent(); }},
            {"4. Copies: " + String(printCopies), []() { selectCopies(); }},
            {"5. Review & Send", []() { reviewAndSend(); }},
            {"Back", [&leave]() { leave = true; }},
        };
        addOptionToMainMenu();
        loopOptions(options, MENU_TYPE_SUBMENU, "Printer Tool");
    }
}
