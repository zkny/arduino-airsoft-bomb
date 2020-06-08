#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>
#include <Keypad.h>

LiquidCrystal_PCF8574 lcd(0x27); // set the LCD address to 0x27 for a 16 chars and 2 line display
// sda -> analouge 4 (a4)
// scl -> analouge 5 (a5)

const byte ROWS = 4;
const byte COLS = 4;

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


#define buzzer 3
#define endbuzzer 2

#define red 5
#define green 4

// always the current timer
int minutes = 10;
int seconds = 1;

int codeLength = 8;

char code[12];
char pin[4];

char defusePin[4];

char input[2];
char input2[3];
char input4[12];


unsigned long time;
unsigned long interval;
unsigned long difference;

int counter = 0;
void setup()
{
  delay(1001);
  interval = millis();

  pinMode(buzzer, OUTPUT);
  pinMode(endbuzzer, OUTPUT);

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  analogWrite(green, 255);
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


  setBombTimer();
  setBombCode();
  
  checkCode();
  lcd.clear();
  setPin();
  
}


void checkCode() {

  memset(input4, 0, sizeof(input4));
  

  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,1);
  

  char key4 = kpd.getKey();
  while(key4 != '*') {
    key4 = kpd.getKey();
    if (key4) {
      strncat(input4, &key4, 1);
      lcd.print(key4);
      delay(250);
    }
  }

  
  for (int x = 0; x < codeLength; x++) {
    if ( code[x] != input4[x] ) {
      checkCode();
    }
  }
  
}

void setBombTimer() {
  lcd.setCursor(0,0);
  lcd.print("Bomb Time: ");

  char key1 = kpd.getKey();

  while(key1 != '*') {
    key1 = kpd.getKey();
    if (key1) {
      strncat(input, &key1, 1);
      lcd.print(key1);
      delay(250);
    }
  }  
  minutes = atoi(input);

  lcd.clear();
  delay(500);
}


void setBombCode() {
  lcd.setCursor(0,0);
  lcd.print("Code lenght: <13");

  char keyx = kpd.getKey();
  lcd.setCursor(0,1);
  while(keyx != '*') {
    keyx = kpd.getKey();
    if (keyx) {
      strncat(input2, &keyx, 1);
      lcd.print(keyx);
      delay(250);
    }
  }  
  codeLength = atoi(input2);


  randomSeed(analogRead(0));
  char temp[1];
  for (int i = 0; i < codeLength; i++) {
    
    itoa(random(9), temp, 10);
    code[i] = temp[0];
  }

  lcd.clear();

  lcd.setCursor(0,0);
  for (int j = 0; j < codeLength; j++) {
    lcd.print(code[j]);
  }
  lcd.print('*');
  lcd.setCursor(0,1);
  lcd.print("type code to arm");
}


void setPin() {
  
  char tempx[4];
  
  randomSeed(analogRead(0));
  
  for (int i = 0; i < 4; i++) {
  
    itoa(random(9), tempx, 10);
    pin[i] = tempx[0];
  }

  lcd.setCursor(0,0);
  lcd.print("disarm: *");
  for (int f = 0; f < 4; f++) {
    lcd.print(pin[f]);
  }
  
}


void writeTime() {
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,1);

  seconds--;
  if (seconds == 0 && minutes == 0) {
    explode();
  }
  
  if (seconds == 0) {
    seconds = 60;
    minutes--;
  }
  
  lcd.print(minutes);
  lcd.print(":");
  lcd.print(seconds);

}
 
void explode() {
  analogWrite(buzzer, 250);
  analogWrite(endbuzzer, 255);
  analogWrite(red, 255);

  lcd.clear();
  lcd.print("Bomb explodes");
  while(1);
}

void disarm() {
  analogWrite(red, 0);
  analogWrite(green, 255);
  analogWrite(buzzer, 0);
  analogWrite(endbuzzer, 0);
  
  lcd.clear();
  lcd.print("Bomb Disarmed");
   while(1);
}


void loop() {
  analogWrite(green, 0);
  time = millis();
  unsigned long currentDifference = time / interval;

  if (currentDifference != difference) {
    if ( (currentDifference % 2) == 0) {
      analogWrite(buzzer, 255);
      analogWrite(red, 255);
    } else {
      analogWrite(buzzer, 0);
      analogWrite(red, 0);
    }
    
    difference = currentDifference;
    writeTime();
  }
  
  char key = kpd.getKey();
  if(key == '*') {
    memset(defusePin, 0, sizeof(defusePin));
    counter = 0;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("*");
  } else if (key) {

    counter++;    
    lcd.setCursor(counter, 0);
    lcd.print(key);
    strncat(defusePin, &key, 1);
  }

  if (counter == 4) {

    for (int x = 0; x < 4; x++) {
      if ( pin[x] != defusePin[x] ) {
        explode();
      }
    }
    disarm();
  }
  
}
