#include <TinyGPSPlus.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "OneButton.h"

#define SCREEN_ADDRESS 0x3C  // OLED Display I2C Address
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

#define BUTTON_PIN 15  // GPIO for Button
#define LED_PIN 2      // Built-in LED on ESP32

HardwareSerial gpsSerial(2); // Use UART2 (GPIO 16=RX, GPIO 17=TX)
TinyGPSPlus gps;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
OneButton button(BUTTON_PIN, true);

bool ledState = false;

void setup() {
    Serial.begin(115200);
    gpsSerial.begin(9600, SERIAL_8N1, 16, 17);  // GPS RX=GPIO16, TX=GPIO17
    Wire.begin(21, 22);  // SDA=21, SCL=22 (ESP32 I2C)
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    display.clearDisplay();
    
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, ledState);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    button.attachClick(toggleLED);
    button.attachDoubleClick(resetESP);

    displayWelcome();
}

void loop() {
    button.tick();
    updateGPS();
}

void toggleLED() {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    Serial.println(ledState ? "LED ON" : "LED OFF");
}

void resetESP() {
    Serial.println("Resetting ESP32...");
    delay(1000);
    ESP.restart();
}

void updateGPS() {
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 10);

    if (gps.location.isValid()) {
        display.print("Lat: "); display.println(gps.location.lat(), 6);
        display.print("Lon: "); display.println(gps.location.lng(), 6);
    } else {
        display.println("Waiting for GPS...");
    }

    display.display();
}

void displayWelcome() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 10);
    display.println("ESP32 GPS Tracker");
    display.display();
    delay(2000);
}
