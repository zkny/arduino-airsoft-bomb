#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>
#include <Keypad.h>

LiquidCrystal_PCF8574 lcd(0x27); // set the LCD address to 0x27 for a 16 chars and 2 line display
// sda -> analouge 4 (a4)
// scl -> analouge 5 (a5)

const byte ROWS = 4; // Four rows
const byte COLS = 4; // Three columns

char keys[ROWS][COLS] = {
  {'1','4','7', '*'}, 
  {'2','5','8', '0'},
  {'3','6','9', '#'},
  {'A','B','C', 'D'}
};

byte rowPins[ROWS] = { 9, 8, 7, 6 }; // Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte colPins[COLS] = { 13, 12, 11, 10 }; // Connect keypad COL0, COL1 and COL2 to these Arduino pins.

// Create the Keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


int minutes = 30;
int seconds = 1;
char code[12];
char input[2];

unsigned long time;
unsigned long interval;
unsigned long difference;

void setup()
{
  delay(1001);
  interval = millis();

  Serial.begin(115200);
  // LCD setup
  Wire.begin();
  Wire.beginTransmission(0x27);

  if (Wire.endTransmission() == 0) {
    lcd.begin(16, 2);
    lcd.setBacklight(true);
    lcd.setCursor(0, 0);
    lcd.clear();
  } else {
    lcd.setBacklight(false);
  }
  Serial.begin(9600);
  
  difference = interval / interval;

  lcd.print("Time config");

  char key1 = kpd.getKey();
  lcd.setCursor(0,1);
  while(key1 != '*') {
    key1 = kpd.getKey();
    if (key1) {
      strncat(input, &key1, 1);
      lcd.print(key1);
      delay(500);
    }
  }  
  minutes = atoi(input);

  lcd.clear();
  delay(500);
  
  lcd.print("Random seed: ");
  char key = kpd.getKey();
  
  while(!key) {
    key = kpd.getKey();
    if (key) {
      lcd.print(key);
      randomSeed(atoi(input));
      delay(500);
    }
  }
  
  lcd.clear();
  char temp[1];
  for (int i = 0; i < 12; i++) {
    itoa(random(9), temp, 10);
    code[i] = temp[0];
  }
  for (int i = 0; i < 12; i++) {
    lcd.print(code[i]);;
  }
  
  lcd.setCursor(0,1);
  lcd.print("write note and *");

  char key3 = kpd.getKey();
  
  while(key3 != '*') {
    key3 = kpd.getKey();
  }

  lcd.clear();
  lcd.print("Bomb armed");
  
}

void writeTime() {
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,1);

  seconds--;
  if (seconds == 0) {
    seconds = 60;
    minutes--;
  }
  
  lcd.print(minutes);
  lcd.print(":");
  lcd.print(seconds);

}



void loop() {
  time = millis();
  unsigned long currentDifference = time / interval;

  
  //Serial.println(time / interval);
  if (currentDifference != difference) {
    difference = currentDifference;
    writeTime();
  }
  
  char key = kpd.getKey();
  if(key == '*') {
    lcd.clear();
    lcd.print(key);
  }
  if (key) {
    lcd.setCursor(0,0);
    lcd.print(key);
  }
  
}
  
