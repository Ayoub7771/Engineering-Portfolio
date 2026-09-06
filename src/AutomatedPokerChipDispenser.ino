src/AutomatedPokerChipDispenser.ino

#include <LiquidCrystal.h>
#include <Servo.h>
#include <Keypad.h>

// LCD settings (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 10, 9, 8, 7); // Initialize the LCD object

// Servo motor setup
Servo servo1, servo2, servo3, servo4;
const int servoPins[4] = {6, 5, 4, 3}; // Servo pins

// Keypad setup for Digi-Key 419 (3x4 keypad)
const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

// Define the row and column pins connected to the keypad
byte rowPins[ROWS] = {A3, A2, A1, A0}; // Row pins of the keypad
byte colPins[COLS] = {13, 2, 1};       // Column pins of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Variables
long chipValues[4] = {0, 0, 0, 0};
long requestedAmount = 0;
int currentChipIndex = 0;
bool chipsSet = false;
bool dispensingComplete = false;

void setup() {
  // Initialize LCD display
  lcd.begin(16, 2); // Set up the LCD with 16 columns and 2 rows

  // Attach servos to their respective pins
  servo1.attach(servoPins[0]);
  servo2.attach(servoPins[1]);
  servo3.attach(servoPins[2]);
  servo4.attach(servoPins[3]);
  resetServos(); // Set initial servo positions to closed

  lcd.print("Welcome to Poker!");
  delay(2000);
  lcd.clear();
  promptChipValue();
}

void loop() {
  // Handle chip setup
  if (!chipsSet) {
    handleChipSetup();
  }
  // Handle dispensing chips
  else if (!dispensingComplete) {
    handleChipDispensing();
  }

  // Allow reset
  char key = keypad.getKey();
  if (key == '*') {
    resetMachine();
  }
}

void promptChipValue() {
  lcd.clear();
  switch (currentChipIndex) {
    case 0:
      lcd.print("Enter Max Chip:");
      break;
    case 1:
      lcd.print("Enter Next Largest:");
      break;
    case 2:
      lcd.print("Enter Following:");
      break;
    case 3:
      lcd.print("Enter Min Chip:");
      break;
  }
}

void handleChipSetup() {
  char key = keypad.getKey();
  if (key && key != '#' && key != '*') { // Handle numeric input
    chipValues[currentChipIndex] = chipValues[currentChipIndex] * 10 + (key - '0');
    lcd.setCursor(0, 1); // Move to second row
    lcd.print(chipValues[currentChipIndex]);
  }
  if (key == '#') { // Confirm current chip value
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
  if (key && key != '#' && key != '*') { // Handle numeric input
    requestedAmount = requestedAmount * 10 + (key - '0');
    lcd.setCursor(0, 1); // Display amount on second row
    lcd.print(requestedAmount);
  }
  if (key == '#') { // Confirm amount
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
    int chipCount = remainingAmount / chipValues[i]; // Calculate number of chips
    remainingAmount %= chipValues[i];               // Update remaining amount

    for (int j = 0; j < chipCount; j++) {
      dispenseChip(i);
    }
  }
}

void dispenseChip(int servoIndex) {
  Servo &servo = (servoIndex == 0) ? servo1 :
                 (servoIndex == 1) ? servo2 :
                 (servoIndex == 2) ? servo3 : servo4;

  servo.write(90);  // Open gate
  delay(500);       // Wait
  servo.write(0);   // Close gate
  delay(500);       // Wait before next
}

void resetServos() {
  servo1.write(0); // Closed position
  servo2.write(0); // Closed position
  servo3.write(0); // Closed position
  servo4.write(0); // Closed position
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
  
  
