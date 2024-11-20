#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

#define BUZZER_PIN 7
#define MOTOR_IN1 8
#define MOTOR_IN2 9

// Initialize LCD (change address if necessary)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Keypad configuration
const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns
char keys[ROWS][COLS] = {
  {'A', 'B', 'C', 'D'},
  {'3', '6', '9', '#'},
  {'2', '5', '8', '0'},
  {'1', '4', '7', '*'}
};
byte rowPins[ROWS] = {10, 11, 12, 13};
byte colPins[COLS] = {A0, A1, A2, A3};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// User passwords (initialize to "0000")
String passwords[4] = {"0000", "0000", "0000", "0000"};
int selectedUser = -1;
String inputPassword = "";
int attempts = 0;

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);

  lcd.begin();
  lcd.backlight();
  displaySelectUser();
}

void loop() {
  char key = keypad.getKey();
  
  if (key) {
    beep();
    if (selectedUser == -1) {
      // Select user
      if (key >= 'A' && key <= 'D') {
        selectedUser = key - 'A';
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter Password");
        lcd.setCursor(0, 1);
        inputPassword = "";
      }
    } else if (inputPassword.length() < 4) {
      // Enter password
      inputPassword += key;
      lcd.setCursor(inputPassword.length() - 1, 1);
      lcd.print(key);
      if (inputPassword.length() == 4) {
        delay(500);
        if (inputPassword == passwords[selectedUser]) {
          lcd.clear();
          lcd.print("A:newPSW B:openGate");
          lcd.setCursor(0, 1);
          lcd.print("A        B");
        } else {
          attempts++;
          if (attempts >= 3) {
            lcd.clear();
            lcd.print("Alarm");
            buzzLong();
            resetSystem();
          } else {
            lcd.clear();
            lcd.print("Try Again");
            delay(1000);
            lcd.clear();
            lcd.print("Enter Password");
            lcd.setCursor(0, 1);
            inputPassword = "";
          }
        }
      }
    } else {
      // Handle newPSW or openGate
      if (key == 'A') {
        lcd.clear();
        lcd.print("Enter New Passwo");
        lcd.setCursor(0, 1);
        inputPassword = "";
      } else if (key == 'B') {
        openGate();
        resetSystem();
      } else if (inputPassword.length() < 4) {
        inputPassword += key;
        lcd.setCursor(inputPassword.length() - 1, 1);
        lcd.print(key);
        if (inputPassword.length() == 4) {
          passwords[selectedUser] = inputPassword;
          resetSystem();
        }
      }
    }
  }
}

void beep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
}

void buzzLong() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(1000);
  digitalWrite(BUZZER_PIN, LOW);
}

void openGate() {
  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);
  delay(2000); // Adjust as needed for one rotation
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, HIGH);
  delay(2000);
  digitalWrite(MOTOR_IN2, LOW);
}

void resetSystem() {
  selectedUser = -1;
  inputPassword = "";
  attempts = 0;
  displaySelectUser();
}

void displaySelectUser() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select User");
  lcd.setCursor(0, 1);
  lcd.print("A B C D");
}
