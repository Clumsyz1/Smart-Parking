/*
 * Project: Smart Parking - Ultimate Edition (UAT Ready)
 * Author: Clumsyz (Senior Dev / Reliable Brother)
 * Features: Auto Gate, Manual Override (V2), Entry Timestamp (V3), LCD Status
 */

#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN    "YOUR_AUTH_TOKEN"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WidgetRTC.h> // ระบบเวลา Real-time
#include <TimeLib.h>   // ระบบจัดการรูปแบบเวลา

// --- Config Pins ---
const int TRIG_PIN  = 5;
const int ECHO_PIN  = 18;
const int SERVO_PIN = 19;
const int SLOT_PINS[] = {4, 13, 14, 27};
const int TOTAL_SLOTS = 4;

// --- Network Credentials ---
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

// --- Global Objects ---
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo gateServo;
BlynkTimer timer;
WidgetRTC rtc;

// --- System States ---
bool isGateOpening = false;

// --- Helper Function: สร้าง Timestamp สำหรับใบเสร็จ ---
// Time Complexity: O(1)
String getTimestamp() {
    char buf[32];
    // รูปแบบ: DD/MM/YYYY HH:MM:SS
    sprintf(buf, "%02d/%02d/%04d %02d:%02d:%02d",
            day(), month(), year(), hour(), minute(), second());
    return String(buf);
}

// --- Function: วัดระยะทาง ---
int getDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH, 25000);
    int distance = duration * 0.034 / 2;
    return (distance <= 0) ? 999 : distance;
}

// --- Function: ปิดไม้กั้น ---
void closeGate() {
    gateServo.write(0);
    isGateOpening = false;
}

// --- Function: เปิดไม้กั้น (รวมระบบบันทึกเวลา) ---
void openGate() {
    if (!isGateOpening) {
        isGateOpening = true;

        // 1. บันทึกเวลาเข้าและส่งไป Blynk V3
        String entryTime = getTimestamp();
        Blynk.virtualWrite(V3, "Last Entry: " + entryTime);
        Serial.println("[LOG] Vehicle Entered: " + entryTime);

        // 2. แสดงผลบน LCD
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("   WELCOME !    ");
        lcd.setCursor(0, 1);
        lcd.print("TIME: ");
        lcd.print(hour() < 10 ? "0" : ""); lcd.print(hour());
        lcd.print(":");
        lcd.print(minute() < 10 ? "0" : ""); lcd.print(minute());

        // 3. สั่ง Servo ทำงาน
        gateServo.write(90);
        timer.setTimeout(3000L, closeGate);
    }
}

// --- Blynk Event: Manual Override (V2) ---
BLYNK_WRITE(V2) {
    if (param.asInt() == 1) {
        Serial.println("[SYSTEM] Manual Override Triggered!");
        openGate();
    }
}

// --- Main System Logic ---
// Time Complexity: O(N) โดย N คือจำนวน Slot
void updateSystem() {
    int freeCount = 0;
    for (int i = 0; i < TOTAL_SLOTS; i++) {
        if (digitalRead(SLOT_PINS[i]) == HIGH) freeCount++;
    }

    Blynk.virtualWrite(V0, freeCount);

    int distance = getDistance();

    if (!isGateOpening) {
        lcd.setCursor(0, 0);
        lcd.print(" SMART PARKING  ");
        lcd.setCursor(0, 1);
        lcd.print("FREE SLOTS: ");
        lcd.print(freeCount);
        lcd.print("   ");

        if (freeCount > 0 && distance < 15 && distance > 0) {
            openGate();
        } else if (freeCount == 0 && distance < 15) {
            lcd.setCursor(0, 0);
            lcd.print(" SORRY, FULL!   ");
            lcd.setCursor(0, 1);
            lcd.print("PLEASE WAIT...  ");
        }
    }
}

void setup() {
    Serial.begin(115200);
    lcd.init();
    lcd.backlight();

    for (int i = 0; i < TOTAL_SLOTS; i++) pinMode(SLOT_PINS[i], INPUT_PULLUP);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    ESP32PWM::allocateTimer(0);
    gateServo.setPeriodHertz(50);
    gateServo.attach(SERVO_PIN, 500, 2400);
    gateServo.write(0);

    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

    // เริ่มทำงานระบบ RTC
    rtc.begin();

    timer.setInterval(500L, updateSystem);
}

void loop() {
    Blynk.run();
    timer.run();
}
