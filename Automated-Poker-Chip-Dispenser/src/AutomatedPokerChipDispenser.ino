#include <LiquidCrystal.h>
#include <Servo.h>
#include <Keypad.h>

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

const int servoPins[4] = {6, 5, 4, 3};

const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {A3, A2, A1, A0};
byte colPins[COLS] = {13, 2, 1};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

long chipValues[4] = {0, 0, 0, 0};
long requestedAmount = 0;
int currentChipIndex = 0;
bool chipsSet = false;
bool dispensingComplete = false;

void setup() {
  lcd.begin(16, 2);
  servo1.attach(servoPins[0]);
  servo2.attach(servoPins[1]);
  servo3.attach(servoPins[2]);
  servo4.attach(servoPins[3]);
  resetServos();
  lcd.print("Welcome to Poker!");
  delay(2000);
  lcd.clear();
  promptChipValue();
}

void loop() {
  if (!chipsSet) {
    handleChipSetup();
  } else if (!dispensingComplete) {
    handleChipDispensing();
  }

  char key = keypad.getKey();
  if (key == '*') {
    resetMachine();
  }
}

void promptChipValue() {
  lcd.clear();
  switch (currentChipIndex) {
    case 0: lcd.print("Enter Max Chip:"); break;
    case 1: lcd.print("Enter Next Largest:"); break;
    case 2: lcd.print("Enter Following:"); break;
    case 3: lcd.print("Enter Min Chip:"); break;
  }
}

void handleChipSetup() {
  char key = keypad.getKey();

  if (key && key != '#' && key != '*') {
    chipValues[currentChipIndex] =
      chipValues[currentChipIndex] * 10 + (key - '0');
    lcd.setCursor(0, 1);
    lcd.print(chipValues[currentChipIndex]);
  }

  if (key == '#') {
    currentChipIndex++;
    if (currentChipIndex < 4) {
      promptChipValue();
    } else {
      chipsSet = true;
      lcd.clear();
      lcd.print("Chips Set!");
      delay(2000);
      promptAmountRequest();
    }
  }
}

void promptAmountRequest() {
  lcd.clear();
  lcd.print("How much do you");
  lcd.setCursor(0, 1);
  lcd.print("want in chips?");
}

void handleChipDispensing() {
  char key = keypad.getKey();

  if (key && key != '#' && key != '*') {
    requestedAmount =
      requestedAmount * 10 + (key - '0');
    lcd.setCursor(0, 1);
    lcd.print(requestedAmount);
  }

  if (key == '#') {
    lcd.clear();
    lcd.print("Dispensing...");
    dispenseChips(requestedAmount);
    dispensingComplete = true;
    lcd.clear();
    lcd.print("Done! Press (*)");
  }
}

void dispenseChips(long amount) {
  long remainingAmount = amount;

  for (int i = 0; i < 4; i++) {
    int chipCount = remainingAmount / chipValues[i];
    remainingAmount %= chipValues[i];

    for (int j = 0; j < chipCount; j++) {
      dispenseChip(i);
    }
  }
}

void dispenseChip(int servoIndex) {
  Servo &servo =
    (servoIndex == 0) ? servo1 :
    (servoIndex == 1) ? servo2 :
    (servoIndex == 2) ? servo3 : servo4;

  servo.write(90);
  delay(500);
  servo.write(0);
  delay(500);
}

void resetServos() {
  servo1.write(0);
  servo2.write(0);
  servo3.write(0);
  servo4.write(0);
}

void resetMachine() {
  for (int i = 0; i < 4; i++) {
    chipValues[i] = 0;
  }

  requestedAmount = 0;
  currentChipIndex = 0;
  chipsSet = false;
  dispensingComplete = false;
  resetServos();

  lcd.clear();
  lcd.print("Machine Reset!");
  delay(2000);
  promptChipValue();
}
