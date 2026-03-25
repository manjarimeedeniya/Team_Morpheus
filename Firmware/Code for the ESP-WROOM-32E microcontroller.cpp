#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int ANALOG_PIN = A0;

const int CAL_POINTS = 11;
float calibrationWeights[CAL_POINTS] =
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
float calibrationVolts[CAL_POINTS];

const int NUM_SAMPLES = 20;

const int EEPROM_MAGIC_ADDR = 0;
const int EEPROM_DATA_START = 1;
const byte EEPROM_MAGIC_VALUE = 0x5A;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  if (EEPROM.read(EEPROM_MAGIC_ADDR) == EEPROM_MAGIC_VALUE) {
    loadCalibrationFromEEPROM();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Loaded calib");
    lcd.setCursor(0, 1);
    lcd.print("from EEPROM");
    delay(2000);
  } else {
    calibrateAllPoints();
    saveCalibrationToEEPROM();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Calib saved");
    lcd.setCursor(0, 1);
    lcd.print("to EEPROM");
    delay(2000);
  }
  lcd.clear();
}

void loop() {
  float v = readVoltage();
  float w = calculateWeight(v);

  lcd.setCursor(0, 0);
  lcd.print("W: ");
  lcd.print(w, 2);
  lcd.print(" kg   ");

  lcd.setCursor(0, 1);
  lcd.print("V: ");
  lcd.print(v, 3);
  lcd.print(" V   ");

  Serial.print("Weight: ");
  Serial.print(w, 2);
  Serial.print(" kg, V=");
  Serial.println(v, 3);

  delay(500);
}

float readVoltage() {
  long total = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    total += analogRead(ANALOG_PIN);
    delay(5);
  }
  float avg = total / (float)NUM_SAMPLES;
  float voltage = avg * (5.0 / 1023.0);
  return voltage;
}

void calibrateAllPoints() {
  for (int i = 0; i < CAL_POINTS; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Place ");
    lcd.print(calibrationWeights[i], 0);
    lcd.print(" kg");

    lcd.setCursor(0, 1);
    lcd.print("Press key PC");
    waitForSerial();

    delay(1000);
    calibrationVolts[i] = readVoltage();

    Serial.print("Cal ");
    Serial.print(calibrationWeights[i], 0);
    Serial.print(" kg -> ");
    Serial.print(calibrationVolts[i], 4);
    Serial.println(" V");
  }
}

void saveCalibrationToEEPROM() {
  int addr = EEPROM_DATA_START;
  for (int i = 0; i < CAL_POINTS; i++) {
    float v = calibrationVolts[i];
    byte *p = (byte*)&v;
    for (int b = 0; b < 4; b++) {
      EEPROM.update(addr++, p[b]);
    }
  }
  EEPROM.update(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);
}

void loadCalibrationFromEEPROM() {
  int addr = EEPROM_DATA_START;
  for (int i = 0; i < CAL_POINTS; i++) {
    float v;
    byte *p = (byte*)&v;
    for (int b = 0; b < 4; b++) {
      p[b] = EEPROM.read(addr++);
    }
    calibrationVolts[i] = v;
  }
}

float calculateWeight(float v) {
  if (v <= calibrationVolts[0]) return 0.0;

  for (int i = 1; i < CAL_POINTS; i++) {
    if (v <= calibrationVolts[i]) {
      return mapFloat(v,
                      calibrationVolts[i - 1], calibrationVolts[i],
                      calibrationWeights[i - 1], calibrationWeights[i]);
    }
  }

  int last = CAL_POINTS - 1;
  return mapFloat(v,
                  calibrationVolts[last - 1], calibrationVolts[last],
                  calibrationWeights[last - 1], calibrationWeights[last]);
}

void waitForSerial() {
  while (!Serial.available()) {}
  while (Serial.available()) Serial.read();
}

float mapFloat(float x,
               float in_min, float in_max,
               float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) /
         (in_max - in_min) + out_min;
}