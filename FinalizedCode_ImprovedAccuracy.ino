// Capacitance Meter Project - EE 425 Microprocessor System Design
// Team member: Angelica Quinto, Forrest Zhang
// Schematic, Arduino code and breadboard prototypes- designed by Forrest Zhang
// PCB and 3D printed Box- designed by Angelica Quinto
// Last edited: 6/7/2024

#include <LiquidCrystal.h>

// LCD connection
const int rs = 51, en = 49, d4 = 47, d5 = 45, d6 = 43, d7 = 41;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Pin assignments
const int analogPin = A0; // Pin to measure capacitor voltage
const int relayPin = 10; // Pin to control relay for discharging
const int resistorPins[] = {1, 2, 3, 4, 5, 6}; // Pins to control resistors

// Resistor values in ohms
const long resistorValues[] = {100, 1000, 10000, 100000, 1000000, 50000000};

// Constants
const float VCC = 5.0; // Supply voltage
const float thresholdVoltage = VCC * 0.632; // 63.2% of VCC

void setup() {
  // Initialize the LCD
  lcd.begin(16, 2);
  lcd.print("Connect device");
  delay(3000);

  // Set pin modes
  pinMode(analogPin, INPUT);
  pinMode(relayPin, OUTPUT);
  for (int i = 0; i < 6; i++) {
    pinMode(resistorPins[i], OUTPUT);
    digitalWrite(resistorPins[i], HIGH); // Turn off all resistors
  }

  // Start discharging the capacitor
  dischargeCapacitor();
}

void loop() {
  int selectedResistorIndex = autoRangeResistor();
  float capacitance = measureCapacitance(selectedResistorIndex);
  displayCapacitance(capacitance);
  while (true); // Infinite loop to halt the program
}

//--------------------------------------------------------------------------------- 
//start of program after initializing
void dischargeCapacitor() {
  lcd.clear();
  lcd.print("Discharging...");
  digitalWrite(relayPin, HIGH); // Activate relay to discharge capacitor
  delay(1000); // Wait for the capacitor to discharge
  while (analogRead(analogPin) > 10); // Wait until the capacitor voltage is close to 0V
  digitalWrite(relayPin, LOW); // Deactivate relay
}

//--------------------------------------------------------------------------------- 
int autoRangeResistor() {
  lcd.clear();
  lcd.print("Auto Ranging...");
  int i;
  for (i = 5; i >= 0; i--) { //Auto ranging loop
    lcd.setCursor(0, 1);
    lcd.print("Testing R");
    lcd.print(i + 1);
    lcd.print("   ");
    digitalWrite(resistorPins[i], LOW); // Turn on the resistor
    delay(2000); // Allow some time for charging
    if (analogRead(analogPin) > thresholdVoltage / VCC * 1023) {
      break;
    }
    digitalWrite(resistorPins[i], HIGH); // Turn off the resistor
  }
  return i;
}

//--------------------------------------------------------------------------------- 
//Phase 2: Measure capacitance
float measureCapacitance(int resistorIndex) {

  dischargeCapacitor();
  lcd.clear();
  lcd.print("Measuring...");

  digitalWrite(resistorPins[resistorIndex], LOW); // Turn on the selected resistor
  unsigned long startTime = micros();

  while (analogRead(analogPin) < thresholdVoltage / VCC * 1023);
  unsigned long elapsedTime = micros() - startTime;
  digitalWrite(resistorPins[resistorIndex], HIGH); // Turn off the resistor

  float resistance = resistorValues[resistorIndex];
  float capacitance = (float)elapsedTime / resistance;

  return capacitance / 1000000.0; // Convert from microfarads to farads
}

//--------------------------------------------------------------------------------- 
//Phase 3: Display capacitance on LCD
void displayCapacitance(float capacitance) {
  lcd.clear();

  if (capacitance >= 1e-6) {
    lcd.print(capacitance * 1e6, 2);
    lcd.print(" uF");

  } else if (capacitance >= 1e-9) {
    lcd.print(capacitance * 1e9, 2);
    lcd.print(" nF");

  } else {
    lcd.print(capacitance * 1e12, 2);
    lcd.print(" pF");
  }
}

//debugging
