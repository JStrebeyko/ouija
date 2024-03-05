
#include "Arduino.h"
// Bounce.pde
// -*- mode: C++ -*-
//
// Make a single stepper bounce from one limit to another
//
// Copyright (C) 2012 Mike McCauley
// $Id: Random.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>




#define dirPin 8
#define stepPin 9
#define dir2Pin 10
#define step2Pin 11
#define motorInterfaceType 1
// Define a stepper and the pins it will use
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin); 
 AccelStepper stepper2 = AccelStepper(motorInterfaceType, step2Pin, dir2Pin); 
int steps = 5000;
int direction = 1;
long width = 0;
bool calibrated = false;

void setup()
{  
  Serial.begin(9600);
  // Change these to suit your stepper if you want
  stepper.setMaxSpeed(10000);
  stepper.setSpeed(8000);
  stepper.setAcceleration(100000);
  //stepper.moveTo(5000);

  stepper2.setMaxSpeed(10000);
  stepper2.setSpeed(8000);
  stepper2.setAcceleration(100000);

stepper.moveTo(steps);
stepper2.moveTo(steps);


   pinMode(2, INPUT_PULLUP); //kontaktron 1
   pinMode(3, INPUT_PULLUP); //kontatkron 2

 
}

void loop()
{

  int kontakt1 = digitalRead(2);
  int kontakt2 = digitalRead(3);
 

  // Serial.print("kontakt1: ");
  // Serial.print(kontakt1);
  // Serial.print(" kontakt2: ");
  // Serial.println(kontakt2);
  //  Serial.print(" dir: ");
  // Serial.println(direction);

    //kontaktorn 1 nie wykrywa nic, jedziemy
    if (kontakt1 == 1 && direction == 1 && calibrated == false){
      stepper.move(steps);
      stepper2.move(steps);
      
    } 

//  if (stepper.distanceToGo() == 0)
//       stepper.moveTo(-stepper.currentPosition());

//     stepper.run();

    // kontaktron 1 zlapal - zeruj pozycje i zawroc
    if (kontakt1 == 0 && direction == 1 && calibrated == false){
      stepper.setCurrentPosition(0);
      stepper2.setCurrentPosition(0);
      direction = 0;
      Serial.print("zeroing: ");
      Serial.println(stepper.currentPosition());
    }

    //jedziemy w druga strone
    if (kontakt2 == 1 && direction == 0 && calibrated == false){
      stepper.move(-steps);
      stepper2.move(-steps);
      

      // Serial.print("stepCount: ");
      // Serial.println(stepCount);
    }

    //dojechalismy na drugi koniec
    if (kontakt2 == 0 && direction == 0 && calibrated == false){
      //Serial.print("calibrated, steps counted: ");
      //Serial.println(stepCount);
      
      stepper.stop();
      stepper2.stop();
      Serial.print("step position = ");
      Serial.println(stepper.currentPosition());
      width = stepper.currentPosition();
      delay(1000);
      //pojedzmy teraz na srodek
      //stepper.setAcceleration(1000);
      Serial.print("calibrated: width = ");
      Serial.println(width);
      stepper.moveTo(width/2);
      stepper2.moveTo(width/2);
      
      
   
  


      calibrated = true;
    }
//popisowa
    if (calibrated == true){
      // stepper.setSpeed(8000);
      // stepper2.setSpeed(8000);
      stepper.runSpeedToPosition();
      stepper2.runSpeedToPosition();
      // Serial.println(stepper.currentPosition());
    }

if (calibrated == false){
  // stepper.setSpeed(8000);
  // stepper2.setSpeed(8000);
    stepper.runSpeed();
    stepper2.runSpeed();

}

    
}








