
#include "Arduino.h"


#include <AccelStepper.h>

#define dirPin 8     //pins for xl motor
#define stepPin 9    //pins for xl motor
#define dir2Pin 10   //pins for xr motor
#define step2Pin 11  //pins for xr motor
#define dir3Pin 12   //pins for y motor
#define step3Pin 13  //pins for y motor
#define motorInterfaceType 1
// Define a stepper and the pins it will use
AccelStepper xl = AccelStepper(motorInterfaceType, stepPin, dirPin);
AccelStepper xr = AccelStepper(motorInterfaceType, step2Pin, dir2Pin);
AccelStepper y = AccelStepper(motorInterfaceType, step3Pin, dir3Pin);

//set contactrons pins
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


//decalre functions
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
  int kontaktXt = digitalRead(kontXtPin);
  int kontaktXb = digitalRead(kontXbPin);

  // kontaktorn 1 nie wykrywa nic, jedziemy
  if (kontaktXt == 1 && direction == 1 && calibratedX == false) {
    moveX(steps);
  }

  // kontaktron 1 zlapal - zeruj pozycje i zawroc
  if (kontaktXt == 0 && direction == 1 && calibratedX == false) {
    //set 0 position
    setCurrentPositionX(0);

    //change direction
    direction = 0;
  }

  // jedziemy w druga strone
  if (kontaktXb == 1 && direction == 0 && calibratedX == false) {
    moveX(-steps);
  }

  // dojechalismy na drugi koniec
  if (kontaktXb == 0 && direction == 0 && calibratedX == false) {

    //zatrzymaj
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

  // when calibratedX, use runSpeedToPosition
  if (calibratedX == true) {
    runSpeedToPositionX();
    if (xl.distanceToGo() == 0) {
      return true;
    }
  }

  // when not calibratedX, use runSpeed
  if (calibratedX == false) {
    runSpeedX();
    return false;
  }
}


boolean calibrateY() {
  // contactrons Y

  int kontaktYl = digitalRead(kontYlPin);
  int kontaktYr = digitalRead(kontYrPin);


  // kontaktorn 1 nie wykrywa nic, jedziemy
  if (kontaktYl == 1 && directionY == 1 && calibratedY == false) {
    y.move(steps);
  }

  // kontaktron 1 zlapal - zeruj pozycje i zawroc
  if (kontaktYl == 0 && directionY == 1 && calibratedY == false) {
    //set 0 position
    y.setCurrentPosition(0);

    // Serial.print("zeroing: Ypos = ");
    // Serial.println(y.currentPosition());

    //change direction
    directionY = 0;
  }

  // jedziemy w druga strone
  if (kontaktYr == 1 && directionY == 0 && calibratedY == false) {
    y.move(-steps);
  }

  // dojechalismy na drugi koniec
  if (kontaktYr == 0 && directionY == 0 && calibratedY == false) {

    //zatrzymaj
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

  // when calibratedY, use runSpeedToPosition
  if (calibratedY == true) {
    y.runSpeedToPosition();
    if (y.distanceToGo() == 0) {
      return true;
    }
  }

  // when not calibratedY, use runSpeed
  if (calibratedY == false) {
    y.runSpeed();
    return false;
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("siema wariaty duups");
  // Change these to suit your stepper if you want
  steppersSetup(15000.0, 10000.0, 100000.0);  // steppersSetup(maxspeed, speed, accel)

  moveToX(steps);


  pinMode(kontXtPin, INPUT_PULLUP);  // kontaktron Xt
  pinMode(kontXbPin, INPUT_PULLUP);  // kontatkron Xb
  pinMode(kontYlPin, INPUT_PULLUP);  // kontaktron Yl
  pinMode(kontYrPin, INPUT_PULLUP);  // kontatkron Yr
}



void loop() {


  // // proba literkowa
  // String txt1 = "siema eniu tu dupa";
  // for (int i = 0; i <= txt1.length();) {
  //   //Serial.println(txt1.charAt(i));

  //   if (txt1.charAt(i) == 'A' || txt1.charAt(i) == 'a') {
  //     t = ((6.28319 / 26) * 0);
  //   }
  //    if (txt1.charAt(i) == 'B' || txt1.charAt(i) == 'b') {
  //     t = ((6.28319 / 26) * 1);
  //   }
  //   if (txt1.charAt(i) == 'C' || txt1.charAt(i) == 'c') {
  //     t = ((6.28319 / 26) * 2);
  //   }
  //    if (txt1.charAt(i) == 'D' || txt1.charAt(i) == 'd') {
  //     t = ((6.28319 / 26) * 3);
  //   }
  //      if (txt1.charAt(i) == 'E' || txt1.charAt(i) == 'e') {
  //     t = ((6.28319 / 26) * 4);
  //   }
  //    if (txt1.charAt(i) == 'F' || txt1.charAt(i) == 'f') {
  //     t = ((6.28319 / 26) * 5);
  //   }
  //   if (txt1.charAt(i) == 'G' || txt1.charAt(i) == 'g') {
  //     t = ((6.28319 / 26) * 6);
  //   }
  //    if (txt1.charAt(i) == 'H' || txt1.charAt(i) == 'h') {
  //     t = ((6.28319 / 26) * 7);
  //   }
  //    if (txt1.charAt(i) == 'I' || txt1.charAt(i) == 'i') {
  //     t = ((6.28319 / 26) * 8);
  //   }
  //    if (txt1.charAt(i) == 'J' || txt1.charAt(i) == 'j') {
  //     t = ((6.28319 / 26) * 9);
  //   }
  //   if (txt1.charAt(i) == 'K' || txt1.charAt(i) == 'k') {
  //     t = ((6.28319 / 26) * 10);
  //   }
  //    if (txt1.charAt(i) == 'L' || txt1.charAt(i) == 'l') {
  //     t = ((6.28319 / 26) * 11);
  //   }
  //      if (txt1.charAt(i) == 'M' || txt1.charAt(i) == 'm') {
  //     t = ((6.28319 / 26) * 12);
  //   }
  //    if (txt1.charAt(i) == 'N' || txt1.charAt(i) == 'n') {
  //     t = ((6.28319 / 26) * 13);
  //   }
  //   if (txt1.charAt(i) == 'O' || txt1.charAt(i) == 'o') {
  //     t = ((6.28319 / 26) * 14);
  //   }
  //    if (txt1.charAt(i) == 'P' || txt1.charAt(i) == 'p') {
  //     t = ((6.28319 / 26) * 15);
  //   }
  //    if (txt1.charAt(i) == 'Q' || txt1.charAt(i) == 'q') {
  //     t = ((6.28319 / 26) * 16);
  //   }
  //    if (txt1.charAt(i) == 'R' || txt1.charAt(i) == 'r') {
  //     t = ((6.28319 / 26) * 17);
  //   }
  //   if (txt1.charAt(i) == 'S' || txt1.charAt(i) == 's') {
  //     t = ((6.28319 / 26) * 18);
  //   }
  //    if (txt1.charAt(i) == 'T' || txt1.charAt(i) == 't') {
  //     t = ((6.28319 / 26) * 19);
  //   }
  //      if (txt1.charAt(i) == 'U' || txt1.charAt(i) == 'u') {
  //     t = ((6.28319 / 26) * 20);
  //   }
  //    if (txt1.charAt(i) == 'V' || txt1.charAt(i) == 'v') {
  //     t = ((6.28319 / 26) * 21);
  //   }
  //   if (txt1.charAt(i) == 'W' || txt1.charAt(i) == 'w') {
  //     t = ((6.28319 / 26) * 22);
  //   }
  //    if (txt1.charAt(i) == 'X' || txt1.charAt(i) == 'x') {
  //     t = ((6.28319 / 26) * 23);
  //   }
  //   if (txt1.charAt(i) == 'Y' || txt1.charAt(i) == 'y') {
  //     t = ((6.28319 / 26) * 24);
  //   }
  //    if (txt1.charAt(i) == 'Z' || txt1.charAt(i) == 'z') {
  //     t = ((6.28319 / 26) * 25);
  //   }

  // }


  bool resX = calibrateX();  // calibrate X
  if (resX == true) {
    //Serial.println("x done");
    bool resY = calibrateY();  // calibrate Y


    if (resY == true) {  // if calibrated
      //Serial.println("xy done");

      long xPos = (height / 2.5) * cos(t) + (width / 2);   //calculate position of letters in a circle
      long yPos = (height / 2.5) * sin(t) + (height / 2);  // r*cos(t) + h

      moveToX(xPos);
      y.moveTo(yPos);


      if (xl.distanceToGo() == 0 && y.distanceToGo() == 0) {  //go to next letter
        delay(2000);

        Serial.println(t);

         t = t + (6.28319 / 26);                              //6.28319 == full turn
      
      }
      runSpeedToPositionX();
      y.runSpeedToPosition();
    }
  }
}
