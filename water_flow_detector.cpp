#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define sensorPin 2
#define buzzerPin 8

LiquidCrystal_I2C lcd(0x27, 16, 2);

volatile int pulseCount = 0;
float flowRate;
unsigned long oldTime = 0;

float calibrationFactor = 4.5;

float threshold = 10.0;   // main limit
float margin = 1.0;       // buffer to avoid flickering

bool buzzerState = false;

// Interrupt function
void countPulse() {
  pulseCount++;
}

void setup() {
  pinMode(sensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  attachInterrupt(digitalPinToInterrupt(sensorPin), countPulse, RISING);
}

void loop() {
  if ((millis() - oldTime) > 1000) {
    detachInterrupt(digitalPinToInterrupt(sensorPin));

    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;

    oldTime = millis();
    pulseCount = 0;

    // Serial output
    Serial.print("Flow Rate: ");
    Serial.print(flowRate);
    Serial.println(" L/min");

    // LCD display
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Flow Rate:");
    lcd.setCursor(0, 1);
    lcd.print(flowRate);
    lcd.print(" L/min");

    if (!buzzerState && flowRate > (threshold + margin)) {
      buzzerState = true;
      digitalWrite(buzzerPin, HIGH);
    }
    else if (buzzerState && flowRate < (threshold - margin)) {
      buzzerState = false;
      digitalWrite(buzzerPin, LOW);
    }

    attachInterrupt(digitalPinToInterrupt(sensorPin), countPulse, RISING);
  }
}