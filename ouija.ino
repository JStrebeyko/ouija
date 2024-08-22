
//#include "Arduino.h"
#include <AccelStepper.h>
#include <SPI.h>
#include <MFRC522.h>

/*------RFID READER PIN LAYOUT

 * Pin layout should be as follows:
 * Signal     Pin              Pin               Pin
 *            Arduino Uno      Arduino Mega      MFRC522 board
 * ------------------------------------------------------------
 * Reset      9                6                 RST
 * SPI SS     10               53                SDA
 * SPI MOSI   11               51                MOSI
 * SPI MISO   12               50                MISO
 * SPI SCK    13               52                SCK
 * voltage 3.3v

 */

String txt[10];

//texts to display
String txt1 = "chuj";
String txt2 = "2 2 2 2";
String txt3 = "3 3 3 3";
String txt4 = "4 4 4 4";
String txt5 = "5 5 5 5";
String txt6 = "6 6 6 6";
String txt7 = "7 7 7 7";
String txt8 = "8 8 8 8";
String txt9 = "9 9 9 9";

String txt0 = " ";

// pins for rfid
/* 
SS pins on the shield: 25,26,27,28,29
default: 53
*/
#define SS_PIN 53
#define RST_PIN 6

#define dirPin 8     // pins for xl motor
#define stepPin 9    // pins for xl motor
#define dir2Pin 10   // pins for xr motor
#define step2Pin 11  // pins for xr motor
#define dir3Pin 12   // pins for y motor
#define step3Pin 13  // pins for y motor
#define motorInterfaceType 1
// Define a stepper and the pins it will use
AccelStepper xl = AccelStepper(motorInterfaceType, stepPin, dirPin);
AccelStepper xr = AccelStepper(motorInterfaceType, step2Pin, dir2Pin);
AccelStepper y = AccelStepper(motorInterfaceType, step3Pin, dir3Pin);

// set contactrons pins
int kontXtPin = 2;
int kontXbPin = 3;
int kontYlPin = 4;
int kontYrPin = 5;

long steps = 5000;
int direction = 1;
int directionY = 1;
long width = 0;
long height = 0;
bool calibratedX = false;
bool calibratedY = false;

float t = 0;
long r = 0;  // radius

long xPos = 0;
long yPos = 0;

// rfid variables
int elock = 39;
MFRC522 rfid(SS_PIN, RST_PIN);  // Create MFRC522 instance.
int bingo = 0;

// decalre functions
void runSpeedX() {
  xl.runSpeed();
  xr.runSpeed();
}

void runSpeedToPositionX() {
  xl.runSpeedToPosition();
  xr.runSpeedToPosition();
}

void moveToX(long x) {
  xl.moveTo(x);
  xr.moveTo(x);
}

void stopX() {
  xl.stop();
  xr.stop();
}

void moveX(long st) {
  xl.move(st);
  xr.move(st);
}

void setCurrentPositionX(long pos) {
  xl.setCurrentPosition(pos);
  xr.setCurrentPosition(pos);
}

void steppersSetup(float maxSpeed, float speed, float accel) {
  xl.setMaxSpeed(maxSpeed);
  xl.setSpeed(speed);
  xl.setAcceleration(accel);

  xr.setMaxSpeed(maxSpeed);
  xr.setSpeed(speed);
  xr.setAcceleration(accel);

  y.setMaxSpeed(maxSpeed);
  y.setSpeed(speed);
  y.setAcceleration(accel);
}

boolean calibrateX() {
  // contactrons X

  while (!calibratedX) {
    int kontaktXt = digitalRead(kontXtPin);
    int kontaktXb = digitalRead(kontXbPin);
    // kontaktorn 1 nie wykrywa nic, jedziemy
    if (kontaktXt == 1 && direction == 1) {
      moveX(steps);
    }

    // kontaktron 1 zlapal - zeruj pozycje i zawroc
    if (kontaktXt == 0 && direction == 1) {
      // set 0 position
      setCurrentPositionX(0);

      // change direction
      direction = 0;
    }

    // jedziemy w druga strone
    if (kontaktXb == 1 && direction == 0) {
      moveX(-steps);
    }

    // dojechalismy na drugi koniec
    if (kontaktXb == 0 && direction == 0) {

      // zatrzymaj
      stopX();

      // set width
      width = xl.currentPosition();
      delay(1000);

      // pojedzmy teraz na srodek
      Serial.print("calibratedX: width = ");
      Serial.println(width);

      moveToX(width / 2);

      calibratedX = true;
    }

    // when not calibratedX, use runSpeed

    runSpeedX();
  }

  // when calibratedX, use runSpeedToPosition
  while (xl.distanceToGo() != 0) {
    // if (calibratedX == true)
    // {
    runSpeedToPositionX();
    // if (xl.distanceToGo() == 0)
    // {
    //   return true;
    // }
    // }
  }
}

boolean calibrateY() {
  // contactrons Y
  while (!calibratedY) {
    int kontaktYl = digitalRead(kontYlPin);
    int kontaktYr = digitalRead(kontYrPin);

    // kontaktorn 1 nie wykrywa nic, jedziemy
    if (kontaktYl == 1 && directionY == 1) {
      y.move(steps);
    }

    // kontaktron 1 zlapal - zeruj pozycje i zawroc
    if (kontaktYl == 0 && directionY == 1) {
      // set 0 position
      y.setCurrentPosition(0);

      // Serial.print("zeroing: Ypos = ");
      // Serial.println(y.currentPosition());

      // change direction
      directionY = 0;
    }

    // jedziemy w druga strone
    if (kontaktYr == 1 && directionY == 0) {
      y.move(-steps);
    }

    // dojechalismy na drugi koniec
    if (kontaktYr == 0 && directionY == 0) {

      // zatrzymaj
      y.stop();

      // set width
      height = y.currentPosition();
      delay(1000);

      // pojedzmy teraz na srodek
      Serial.print("calibratedY: height = ");
      Serial.println(height);

      y.moveTo(height / 2);

      calibratedY = true;
    }

    y.runSpeed();
  }
  // when calibratedY, use runSpeedToPosition
  while (y.distanceToGo() != 0) {

    y.runSpeedToPosition();
    // if (y.distanceToGo() == 0)
    // {
    // }
  }

  // when not calibratedY, use runSpeed
}
//letter positions
void charPos(char letter = 'a') {
  // proba literkowa
  // String txt1 = "siema eniu tu dupa";
  // for (int i = 0; i <= txt1.length(); i++) {
  // Serial.println(txt1.charAt(i));
  if (letter != ' ' || letter != '.') {
    r = height / 2;
    if (letter == '1' || letter == '2' || letter == '3' || letter == '4' || letter == '5' || letter == '6' || letter == '7' || letter == '8' || letter == '9' || letter == '0') {
      r = height / 3.5;
    }
  }
  if (letter == ' ' || letter == '.') {
    // t = -((6.28319 / 26) * 25);
    r = 0;
  }

  if (letter == 'A' || letter == 'a') {
    t = -((6.28319 / 26) * 0);
  }
  if (letter == 'B' || letter == 'b') {
    t = -((6.28319 / 26) * 1);
  }
  if (letter == 'C' || letter == 'c') {
    t = -((6.28319 / 26) * 2);
  }
  if (letter == 'D' || letter == 'd') {
    t = -((6.28319 / 26) * 3);
  }
  if (letter == 'E' || letter == 'e') {
    t = -((6.28319 / 26) * 4);
  }
  if (letter == 'F' || letter == 'f') {
    t = -((6.28319 / 26) * 5);
  }
  if (letter == 'G' || letter == 'g') {
    t = -((6.28319 / 26) * 6);
  }
  if (letter == 'H' || letter == 'h') {
    t = -((6.28319 / 26) * 7);
  }
  if (letter == 'I' || letter == 'i') {
    t = -((6.28319 / 26) * 8);
  }
  if (letter == 'J' || letter == 'j') {
    t = -((6.28319 / 26) * 9);
  }
  if (letter == 'K' || letter == 'k') {
    t = -((6.28319 / 26) * 10);
  }
  if (letter == 'L' || letter == 'l') {
    t = -((6.28319 / 26) * 11);
  }
  if (letter == 'M' || letter == 'm') {
    t = -((6.28319 / 26) * 12);
  }
  if (letter == 'N' || letter == 'n') {
    t = -((6.28319 / 26) * 13);
  }
  if (letter == 'O' || letter == 'o') {
    t = -((6.28319 / 26) * 14);
  }
  if (letter == 'P' || letter == 'p') {
    t = -((6.28319 / 26) * 15);
  }
  if (letter == 'Q' || letter == 'q') {
    t = -((6.28319 / 26) * 16);
  }
  if (letter == 'R' || letter == 'r') {
    t = -((6.28319 / 26) * 17);
  }
  if (letter == 'S' || letter == 's') {
    t = -((6.28319 / 26) * 18);
  }
  if (letter == 'T' || letter == 't') {
    t = -((6.28319 / 26) * 19);
  }
  if (letter == 'U' || letter == 'u') {
    t = -((6.28319 / 26) * 20);
  }
  if (letter == 'V' || letter == 'v') {
    t = -((6.28319 / 26) * 21);
  }
  if (letter == 'W' || letter == 'w') {
    t = -((6.28319 / 26) * 22);
  }
  if (letter == 'X' || letter == 'x') {
    t = -((6.28319 / 26) * 23);
  }
  if (letter == 'Y' || letter == 'y') {
    t = -((6.28319 / 26) * 24);
  }
  if (letter == 'Z' || letter == 'z') {
    t = -((6.28319 / 26) * 25);
  }

  if (letter == '0') {
    t = -((6.28319 / 10) * 0);
  }
  if (letter == '1') {
    t = -((6.28319 / 10) * 1);
  }
  if (letter == '2') {
    t = -((6.28319 / 10) * 2);
  }
  if (letter == '3') {
    t = -((6.28319 / 10) * 3);
  }
  if (letter == '4') {
    t = -((6.28319 / 10) * 4);
  }
  if (letter == '5') {
    t = -((6.28319 / 10) * 5);
  }
  if (letter == '6') {
    t = -((6.28319 / 10) * 6);
  }
  if (letter == '7') {
    t = -((6.28319 / 10) * 7);
  }
  if (letter == '8') {
    t = -((6.28319 / 10) * 8);
  }
  if (letter == '9') {
    t = -((6.28319 / 10) * 9);
  }

  // }
}

void playString(String sentence = txt0) {
  if (xl.distanceToGo() == 0 && y.distanceToGo() == 0) {  // go to next letter

    for (int i = 0; i <= sentence.length();) {
      // Serial.println(i);
      if (i == sentence.length() && xl.distanceToGo() == 0 && y.distanceToGo() == 0) {
          Serial.println("dojechali");
          bingo = 0;
          
          Serial.println("dojechali 22");
          break;
        }
      if (xl.distanceToGo() == 0 && y.distanceToGo() == 0) {

        delay(2000);

        
        charPos(sentence.charAt(i));       // set t value
        xPos = r * cos(t) + (width / 2);   // calculate position of letters in a circle x = r*cos(t) + h, y = r*sin(t) + w , where h and w are the coordinates of the circle center
        yPos = r * sin(t) + (height / 2);  // r = height/2 for big circle (letters), r = height/3 for small circle (numbers)

        moveToX(xPos);
        y.moveTo(yPos);

        i++;
      }
      runSpeedToPositionX();
      y.runSpeedToPosition();
    }
  }
}

void setup() {
  Serial.begin(9600);
  // Serial.println(txt1.length());
  // Change these to suit your stepper if you want
  steppersSetup(15000.0, 10000.0, 100000.0);  // steppersSetup(maxspeed, speed, accel)

  moveToX(steps);

  pinMode(kontXtPin, INPUT_PULLUP);  // kontaktron Xt
  pinMode(kontXbPin, INPUT_PULLUP);  // kontatkron Xb
  pinMode(kontYlPin, INPUT_PULLUP);  // kontaktron Yl
  pinMode(kontYrPin, INPUT_PULLUP);  // kontatkron Yr

  // rfid reader setup
  pinMode(elock, OUTPUT);
  digitalWrite(elock, LOW);
  SPI.begin();      // Init SPI bus
  rfid.PCD_Init();  // Init MFRC522 card
                    // Serial.println("Scan a MIFARE Classic PICC to demonstrate Value Blocks.");
                    // Serial.println("HI PLS SCAN BOSS");

  // prepare sentences array
  txt[0] = txt0;
  txt[1] = txt1 + " ";
  txt[2] = txt2 + " ";
  txt[3] = txt3 + " ";
  txt[4] = txt4 + " ";
  txt[5] = txt5 + " ";
  txt[6] = txt6 + " ";
  txt[7] = txt7 + " ";
  txt[8] = txt8 + " ";
  txt[9] = txt9 + " ";

  // Serial.println(txt[1]);

//calibrate the machine
  calibrateX();
  calibrateY();


  // bool resX = calibrateX();  // calibrate X
  // if (resX == true) {
  //   //Serial.println("x done");
  //   bool resY = calibrateY();  // calibrate Y

  //   if (resY == true) {  // if calibrated
  //     Serial.println("yesss");
  //   }
  // }
}

void loop() {

  // calibrateX();
  // calibrateY();

  // Serial.println("skalivrowana kurwa");

  // xPos = width / 2;
  // yPos = height / 2; // go to the middle

  // moveToX(xPos);
  // y.moveTo(yPos);
  // runSpeedToPositionX();
  // y.runSpeedToPosition();

  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.

  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  // Look for new cards
  if (!rfid.PICC_IsNewCardPresent()) {

    return;
  }




  // Select one of the cards
  if (!rfid.PICC_ReadCardSerial()) {
    Serial.println("new card");
    return;
  }



  // Now a card is selected. The UID and SAK is in rfid.uid.

  // Dump UID
  Serial.print("Card UID:");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], DEC);
    if (i >= rfid.uid.size -1){
      Serial.println(" ");
    }
  }


  // Dump PICC type
  byte piccType = rfid.PICC_GetType(rfid.uid.sak);
  //    Serial.print("PICC type: ");
  // Serial.println(mfrc522.PICC_GetTypeName(piccType));
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    // Serial.println("This sample only works with MIFARE Classic cards.");
    return;
  }


  //playString(txt[0]);
  // bingo = 0;
  // defining Cards here
  if ((rfid.uid.uidByte[0] == 243) && (rfid.uid.uidByte[1] == 36) && (rfid.uid.uidByte[2] == 177) && (rfid.uid.uidByte[3] == 207)) {
    // Serial.println("Tag A detected");
    bingo = 1;
  } else if ((rfid.uid.uidByte[0] == 218) && (rfid.uid.uidByte[1] == 136) && (rfid.uid.uidByte[2] == 125) && (rfid.uid.uidByte[3] == 122)) {
    //Serial.println("Tag B detected");
    bingo = 2;
  } else if ((rfid.uid.uidByte[0] == 218) && (rfid.uid.uidByte[1] == 94) && (rfid.uid.uidByte[2] == 219) && (rfid.uid.uidByte[3] == 122)) {
    // Serial.println("Tag C detected");
    bingo = 3;
  } else if ((rfid.uid.uidByte[0] == 50) && (rfid.uid.uidByte[1] == 243) && (rfid.uid.uidByte[2] == 12) && (rfid.uid.uidByte[3] == 255)) {
    // Serial.println("Tag D detected");
    bingo = 4;
  } else if ((rfid.uid.uidByte[0] == 193) && (rfid.uid.uidByte[1] == 50) && (rfid.uid.uidByte[2] == 55) && (rfid.uid.uidByte[3] == 13)) {
    // Serial.println("Tag E detected");
    bingo = 5;
  } else if ((rfid.uid.uidByte[0] == 123) && (rfid.uid.uidByte[1] == 194) && (rfid.uid.uidByte[2] == 17) && (rfid.uid.uidByte[3] == 255)) {
    // Serial.println("Tag F detected");
    bingo = 6;
  } else if ((rfid.uid.uidByte[0] == 74) && (rfid.uid.uidByte[1] == 188) && (rfid.uid.uidByte[2] == 129) && (rfid.uid.uidByte[3] == 129)) {
    // Serial.println("Tag G detected");
    bingo = 7;
  } else if ((rfid.uid.uidByte[0] == 74) && (rfid.uid.uidByte[1] == 220) && (rfid.uid.uidByte[2] == 70) && (rfid.uid.uidByte[3] == 129)) {
    // Serial.println("Tag H detected");
    bingo = 8;
  } else if ((rfid.uid.uidByte[0] == 50) && (rfid.uid.uidByte[1] == 113) && (rfid.uid.uidByte[2] == 84) && (rfid.uid.uidByte[3] == 207)) {
    // Serial.println("Tag I detected");
    bingo = 9;
  } else {
    bingo = 0;
  }
  Serial.print("bingo: ");
  Serial.println(bingo);
  playString(txt[bingo]);
  // if (xl.distanceToGo() == 0 && y.distanceToGo() == 0)
  // {

  // }
}
