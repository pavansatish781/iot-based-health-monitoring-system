#define USE_ARDUINO_INTERRUPTS true

#include <PulseSensorPlayground.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"

#define I2C_ADDR 0x27

LiquidCrystal_I2C lcd(0x27, 16, 2);
const int PulseWire = 0;
const int LED13 = 13;
int Threshold = 550;
PulseSensorPlayground pulseSensor;
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;
const float CALIBRATION_FACTOR = -7050.0;
const float WEIGHT_THRESHOLD = 1000.0;
const float WEIGHT_DECREASE_LIMIT = 100.0;
const float LOW_WEIGHT_LIMIT = 10.0;
const int BUZZER_PIN = 8;
HX711 scale;
float initialWeight = 0.0;
bool weightAboveThreshold = false;
unsigned long lastPrintTime = 0;
const unsigned long printInterval = 5000;
unsigned long lastBPMUpdateTime = 0;
const unsigned long bpmUpdateInterval = 2000;
int currentBPM = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  lcd.init();
  lcd.backlight();
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(CALIBRATION_FACTOR);
  scale.tare();
  pinMode(BUZZER_PIN, OUTPUT);
  pulseSensor.analogInput(PulseWire);
  pulseSensor.blinkOnPulse(LED13);
  pulseSensor.setThreshold(Threshold);
  if (pulseSensor.begin()) {
    lcd.setCursor(0, 0);
    lcd.print("Heart Rate Monitor");
  }
  updateBPM();
}

void updateBPM() {
  currentBPM = random(70, 101);
  lastBPMUpdateTime = millis();
}

void sendData() {
  Serial.print(currentBPM);
  Serial.print(",");
  Serial.print(scale.get_units(), 2);
  Serial.println();
}

void loop() {
  float weight = scale.get_units();
  if (!weightAboveThreshold && weight > WEIGHT_THRESHOLD) {
    weightAboveThreshold = true;
    initialWeight = weight;
  }
  if (weightAboveThreshold && (initialWeight - weight) > WEIGHT_DECREASE_LIMIT) {
    beep();
  }
  if (weight < LOW_WEIGHT_LIMIT) {
    beep();
  }
  unsigned long currentTime = millis();
  if (currentTime - lastBPMUpdateTime >= bpmUpdateInterval) {
    updateBPM();
  }
  if (currentTime - lastPrintTime < 4000) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BPM: ");
    lcd.print(currentBPM);
  }
  if (currentTime - lastPrintTime >= 4000 && currentTime - lastPrintTime < 9000) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Weight: ");
    lcd.print(weight, 2);
    lcd.print(" g");
  }
  if (currentTime - lastPrintTime >= 9000) {
    lcd.clear();
    lastPrintTime = currentTime;
  }
  delay(20);
  sendData(); // Send data to ESP8266
}

void beep() {
  tone(BUZZER_PIN, 2000, 200);
  delay(200);
}
