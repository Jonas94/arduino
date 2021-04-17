/*
 *
 *
 * Automated watering system by Jonas Fredriksson
 * Arduino controlling a water pump via a relay
 * Input is taken from push and flip switches
 * Status is shown by LEDs
 *
 *
 */

const int manualWateringButtonPin = 2; // DIGITAL PIN 2
const int waterFlowButtonPin = 3;      // DIGITAL PIN 3
const int timeButtonPin = 4;           // DIGITAL PIN 4

const int ledPinFlowLow = 5;  // DIGITAL PIN 5
const int ledPinFlowMid = 6;  // DIGITAL PIN 6
const int ledPinFlowHigh = 7; // DIGITAL PIN 7

const int ledPinTimeLow = 8;   // DIGITAL PIN 8
const int ledPinTimeMid = 9;   // DIGITAL PIN 9
const int ledPinTimeHigh = 10; // DIGITAL PIN 10

int allLeds[] = {ledPinFlowLow, ledPinFlowMid, ledPinFlowHigh,
                 ledPinTimeLow, ledPinTimeMid, ledPinTimeHigh};

const int autoModeButtonPin = 11;      // DIGITAL PIN 11
const int autoModeResetButtonPin = 12; // DIGITAL PIN 12
const int statusButtonPin = 13;        // DIGITAL PIN 13
const int relayPin = A0;               // ANALOG PIN 0

const long MINUTE_IN_MS = 60 * 1000;
const long HOUR_IN_MS = MINUTE_IN_MS * 60;

const int numberOfStates = 4;
const int numberOfLeds = 6;
const int statusDelay = 1500;
const int noFlow = 0;
const unsigned long lowFlow = 5 * MINUTE_IN_MS; // Set good values for low mid high
const unsigned long midFlow = 15 * MINUTE_IN_MS;
const unsigned long highFlow = 30 * MINUTE_IN_MS;

const long noInterval = 0;
const long lowInterval = 6 * HOUR_IN_MS; 
const long midInterval = 12 * HOUR_IN_MS;
const long highInterval = 24 * HOUR_IN_MS; 

bool manualWateringButtonState = false;
bool waterFlowButtonState = false;
bool timeButtonState = false;
bool autoMode = false;

int waterFlowState = 1; // Default value to handle interrupts when on vacation
int waterFlow[] = {noFlow, lowFlow, midFlow, highFlow};

int timeState = 1; // Default value to handle interrupts
unsigned long timeValues[] = {noInterval, lowInterval, midInterval,highInterval};

unsigned long previousMillis = millis(); //default value to avoid instant start
long interval = 0;

void setup() {
  pinMode(manualWateringButtonPin, INPUT_PULLUP);
  pinMode(waterFlowButtonPin, INPUT_PULLUP);
  pinMode(statusButtonPin, INPUT_PULLUP);
  pinMode(timeButtonPin, INPUT_PULLUP);
  pinMode(autoModeButtonPin, INPUT_PULLUP);
  pinMode(autoModeResetButtonPin, INPUT_PULLUP);

  pinMode(relayPin, OUTPUT);
  pinMode(ledPinFlowLow, OUTPUT);
  pinMode(ledPinFlowMid, OUTPUT);
  pinMode(ledPinFlowHigh, OUTPUT);
  pinMode(ledPinTimeLow, OUTPUT);
  pinMode(ledPinTimeMid, OUTPUT);
  pinMode(ledPinTimeHigh, OUTPUT);
}

void loop() {
  if (digitalRead(statusButtonPin) == LOW) {
    showAllStatus();
  }

  // Handle water level
  waterFlowButtonState = digitalRead(waterFlowButtonPin);
  if (waterFlowButtonState == LOW) {
    waterFlowState++;

    if (waterFlowState >= numberOfStates) {
      waterFlowState = 0;
    }
    showWaterFlowStatus();
  }

  // Handle time setting
  timeButtonState = digitalRead(timeButtonPin);
  if (timeButtonState == LOW) {
    timeState++;

    if (timeState >= numberOfStates) {
      timeState = 0;
    }
    showTimeStatus();
  }

  autoMode = digitalRead(autoModeButtonPin);
  unsigned long currentMillis = millis();
  interval = timeValues[timeState];

  // If interval has passed, do watering
  if (timeState > 0 && autoMode == LOW &&
      currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    water(waterFlow[waterFlowState]);
  }

  // If button is pressed do manual watering
  manualWateringButtonState = digitalRead(manualWateringButtonPin);

  if (manualWateringButtonState == LOW) {
    water(waterFlow[waterFlowState]);
  }
}

void water(int waterAmount) {
  // Depending on water amount, maybe seconds to run the pump
  digitalWrite(relayPin, HIGH);
  delay(waterAmount);
  digitalWrite(relayPin, LOW);
}

void writeWaterFlowStateToLeds() {
  if (waterFlowState > 0) {
    digitalWrite(ledPinFlowLow, HIGH);
  }

  if (waterFlowState > 1) {
    digitalWrite(ledPinFlowMid, HIGH);
  }

  if (waterFlowState > 2) {
    digitalWrite(ledPinFlowHigh, HIGH);
  }
}

void writeTimeStateToLeds() {
  if (timeState > 0) {
    digitalWrite(ledPinTimeLow, HIGH);
  }

  if (timeState > 1) {
    digitalWrite(ledPinTimeMid, HIGH);
  }

  if (timeState > 2) {
    digitalWrite(ledPinTimeHigh, HIGH);
  }
}

void writeAllLedLow() {
  for (int currentPin = 0; currentPin < numberOfLeds; currentPin++) {
      digitalWrite(allLeds[currentPin], LOW);
  }
}

void showAllStatus() {
  writeWaterFlowStateToLeds();
  writeTimeStateToLeds();
  delay(statusDelay);
  writeAllLedLow();
}

void showWaterFlowStatus() {
  writeWaterFlowStateToLeds();
  delay(statusDelay);
  writeAllLedLow();
}

void showTimeStatus() {
  writeTimeStateToLeds();
  delay(statusDelay);
  writeAllLedLow();
}
