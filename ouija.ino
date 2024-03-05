
#include "Arduino.h"


#include <AccelStepper.h>

#define dirPin 8
#define stepPin 9
#define dir2Pin 10
#define step2Pin 11
#define motorInterfaceType 1
// Define a stepper and the pins it will use
AccelStepper stepperXl= AccelStepper(motorInterfaceType, stepPin, dirPin);
AccelStepper stepperXr = AccelStepper(motorInterfaceType, step2Pin, dir2Pin);

int kontXtPin = 2;
int kontXbPin = 3;

int steps = 5000;
int direction = 1;
long width = 0;
bool calibrated = false;

void setup()
{
  Serial.begin(9600);
  // Change these to suit your stepper if you want
  stepperXl.setMaxSpeed(10000);
  stepperXl.setSpeed(8000);
  stepperXl.setAcceleration(100000);


  stepperXr.setMaxSpeed(10000);
  stepperXr.setSpeed(8000);
  stepperXr.setAcceleration(100000);

  stepperXl.moveTo(steps);
  stepperXr.moveTo(steps);

  pinMode(kontXtPin, INPUT_PULLUP); // kontaktron Xt
  pinMode(kontXbPin, INPUT_PULLUP); // kontatkron Xb
}

void loop()
{
  // contactrons X
  int kontaktXt = digitalRead(kontXtPin);
  int kontaktXb = digitalRead(kontXbPin);


  // kontaktorn 1 nie wykrywa nic, jedziemy
  if (kontaktXt == 1 && direction == 1 && calibrated == false)
  {
    stepperXl.move(steps);
    stepperXr.move(steps);
  }

  


  // kontaktron 1 zlapal - zeruj pozycje i zawroc
  if (kontaktXt == 0 && direction == 1 && calibrated == false)
  {
    //set 0 position
    stepperXl.setCurrentPosition(0);
    stepperXr.setCurrentPosition(0);

    //change direction
    direction = 0;


    // Serial.print("zeroing: ");
    // Serial.println(stepperXl.currentPosition());
  }

  // jedziemy w druga strone
  if (kontaktXb == 1 && direction == 0 && calibrated == false)
  {
    stepperXl.move(-steps);
    stepperXr.move(-steps);


  }

  // dojechalismy na drugi koniec
  if (kontaktXb == 0 && direction == 0 && calibrated == false)
  {
    
    //zatrzymaj
    stepperXl.stop();
    stepperXr.stop();
    // Serial.print("step position = ");
    // Serial.println(stepperXl.currentPosition());

    // set width
    width = stepperXl.currentPosition();
    delay(1000);

    // pojedzmy teraz na srodek
    Serial.print("calibrated: width = ");
    Serial.println(width);
    stepperXl.moveTo(width / 2);
    stepperXr.moveTo(width / 2);

    calibrated = true;
  }
  // popisowa
  if (calibrated == true)
  {
    stepperXl.runSpeedToPosition();
    stepperXr.runSpeedToPosition();
  }

  if (calibrated == false)
  {
 
    stepperXl.runSpeed();
    stepperXr.runSpeed();
  }
}

void runSpeedX(AccelStepper xl = stepperXl, AccelStepper xr = stepperXr){
    xl.runSpeed();
    xr.runSpeed();
}

void runSpeedToPositionX(AccelStepper xl = stepperXl, AccelStepper xr = stepperXr){
    xl.runSpeedToPosition();
    xr.runSpeedToPosition();
}

void moveToX(long x, AccelStepper xl = stepperXl, AccelStepper xr = stepperXr){
    xl.moveTo(x);
    xr.moveTo(x);
}

void stopX(AccelStepper xl = stepperXl, AccelStepper xr = stepperXr){ 
    xl.stop();
    xr.stop();
}

void moveToX(long st, AccelStepper xl = stepperXl, AccelStepper xr = stepperXr){
    xl.move(st);
    xr.move(st);
}

void setCurrentPositionX(long pos, AccelStepper xl = stepperXl, AccelStepper xr = stepperXr){
    xl.setCurrentPosition(pos);
    xr.setCurrentPosition(pos);
}


void steppersXsetup(float maxSpeed, float speed, float accel){
  stepperXl.setMaxSpeed(maxSpeed);
  stepperXl.setSpeed(speed);
  stepperXl.setAcceleration(accel);


  stepperXr.setMaxSpeed(maxSpeed);
  stepperXr.setSpeed(speed);
  stepperXr.setAcceleration(accel);
}