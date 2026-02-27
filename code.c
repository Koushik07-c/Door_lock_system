#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <EEPROM.h>

// LCD I2C configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Servo motor
Servo doorLock;

// Pins
#define BUZZER 10
#define SERVO_PIN 11
#define GREEN_LED 12
#define RED_LED 13

// Servo positions
#define LOCKED_POS 0
#define UNLOCKED_POS 90

// EEPROM address to store password
#define PASS_ADDR 0

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
 {'1','2','3','A'},
 {'4','5','6','B'},
 {'7','8','9','C'},
 {'*','0','#','D'}
};
byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Variables
String input = "";
String savedPass = "";

void setup() {
  lcd.init();
  lcd.backlight();
  doorLock.attach(SERVO_PIN);
                
  pinMode(BUZZER, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  digitalWrite(BUZZER, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  loadPassword();

  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    if (key == '#') {
      checkPassword();
    } else if (key == '*') {
      input = "";
      lcd.setCursor(0, 1);
      lcd.print("Cleared         ");
    } else if (key == 'A') {
      enterMasterMode();
    } else if (input.length() < 4) {
      input += key;
      lcd.setCursor(0, 1);
      lcd.print("Code: " + input + "    ");
    }
  }
}

void checkPassword() {
  if (input == savedPass) {
    lcd.setCursor(0, 1);
    lcd.print("Access Granted  ");
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    doorLock.write(UNLOCKED_POS);
    delay(2000);
    doorLock.write(LOCKED_POS);
    lcd.setCursor(0, 1);
    lcd.print("Locked           ");
    digitalWrite(GREEN_LED, LOW);
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Wrong Password! ");
    tone(BUZZER, 1000, 500);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    delay(1500);
    digitalWrite(RED_LED, LOW);
  }
  input = "";
}

void enterMasterMode() {
  lcd.clear();
  lcd.print("Master Mode");
  lcd.setCursor(0, 1);
  lcd.print("New Pass:");
  input = "";
  while (input.length() < 4) {
    char key = keypad.getKey();
    if (key && isDigit(key)) {
      input += key;
      lcd.setCursor(10, 1);
      lcd.print(input + " ");
    }
  }
  savedPass = input;
  savePassword();
  lcd.clear();
  lcd.print("Password Saved!");
  delay(2000);
  lcd.clear();
  lcd.print("Enter Password:");
  input = "";
}

void loadPassword() {
  savedPass = "";
  for (int i = 0; i < 4; i++) {
    char c = EEPROM.read(PASS_ADDR + i);
    if (isDigit(c)) {
      savedPass += c;
    } else {
      savedPass = "1234"; // fallback default
      savePassword();
      return;
    }
  }
}

void savePassword() {
    for (int i = 0; i < 4; i++) {
        EEPROM.write(PASS_ADDR + i, savedPass[i]);
    }
}
