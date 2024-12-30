// #include "Arduino.h"
#include <AccelStepper.h>
// #include <SPI.h>
// #include <MFRC522.h>

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

#define RST_PIN 6

#define dirPin 8    // pins for xl motor
#define stepPin 9   // pins for xl motor
#define dir2Pin 10  // pins for xr motor
#define step2Pin 11 // pins for xr motor
#define dir3Pin 12  // pins for y motor
#define step3Pin 13 // pins for y motor
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

// stepper settings
long steps = 5000;
// int direction = 1;
// int directionY = 1;
long width = 0;
long height = 0;
bool calibratedX = false;
bool calibratedY = false;

// letter positioning variables
float t = 0;
long r = 0; // radius
long xPos = 0;
long yPos = 0;

//declare speed
float gSpeed = 10000.0;

// decalre functions
void runSpeedX()
{
  xl.runSpeed();
  xr.runSpeed();
}

void runSpeedToPositionX()
{
  xl.runSpeedToPosition();
  xr.runSpeedToPosition();
}

void moveToX(long x)
{
  xl.moveTo(x);
  xr.moveTo(x);
}

void stopX()
{
  xl.stop();
  xr.stop();
}

void moveX(long st)
{
  xl.move(st);
  xr.move(st);
}

void setCurrentPositionX(long pos)
{
  xl.setCurrentPosition(pos);
  xr.setCurrentPosition(pos);
}

void steppersSetup(float maxSpeed, float speed, float accel)
{
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

// calibration functions
bool calibrateX()
{

  int direction = 1;
  bool done = false;
  Serial1.println("calibrating x..."); 
  while (!calibratedX)
  {
    if (!done)
    {

      int kontaktXt = digitalRead(kontXtPin);
      int kontaktXb = digitalRead(kontXbPin);
      // jedź w jedną
      if (direction == 1)
      {
        // kontaktorn 1 nie wykrywa nic, jedziemy
        if (kontaktXt == 1)
        {
          moveX(steps);
          // kontaktron 1 zlapal - zeruj pozycje i zawroc
        }
        else
        {
          // set 0 position
          setCurrentPositionX(0);

          // change direction
          direction = 0;
        }
      }

      else
      {
        // jedziemy w druga strone
        if (kontaktXb == 1)
        {
          moveX(-steps);
        }
        // dojechalismy na drugi koniec
        else
        {

          // zatrzymaj
          stopX();

          // set width
          width = xl.currentPosition();
          delay(1000);

          // pojedzmy teraz na srodek

          done = true;
          moveToX(width / 2);
          // Serial1.print("calibratedX: width = ");
          // Serial1.println(width);
        }
      }
      runSpeedX();
    }
    // it is done - return
    else
    {

      runSpeedToPositionX();
      if (xl.distanceToGo() == 0)
      {
        calibratedX = true;
        Serial1.println("X calibrated");
      }
    }
  }
}

bool calibrateY()
{
  bool done = false;
  Serial1.println("calibrating y...");
  int direction = 0;
  while (!calibratedY)
  {
    if (!done)
    {

      int kontaktYl = digitalRead(kontYlPin);
      int kontaktYr = digitalRead(kontYrPin);

      // jedź w jedną
      if (direction == 0)
      {
        // kontaktorn 1 nie wykrywa nic, jedziemy
        if (kontaktYl == 1)
        {
          y.move(steps);
          // kontaktron 1 zlapal - zeruj pozycje i zawroc
        }
        else
        {
          // set 0 position
          y.setCurrentPosition(0);
          //Serial1.println("y change direction  ");
          // change direction
          direction = 1;
        }
      }

      else
      {
        // jedziemy w druga strone
        if (kontaktYr == 1)
        {
          y.move(-steps);
        }
        // dojechalismy na drugi koniec
        else
        {

          // zatrzymaj
          y.stop();

          // set width
          height = y.currentPosition();
          delay(1000);

          // pojedzmy teraz na srodek

          done = true;
          y.moveTo(height / 2);
          // Serial1.print("calibratedY = ");
          // Serial1.println(height);
          continue;
        }
      }
      y.runSpeed();
    }
    // it is done - return
    else
    {

      y.runSpeedToPosition();
      if (y.distanceToGo() == 0)
      {
        calibratedY = true;
        Serial1.println("Y calibrated");
        return true;
      }
    }
  }

  // when not calibratedX, use runSpeed
}

// letter positioning
void charPos(char letter = 'a')
{
  if (letter != ' ' || letter != '.')
  {
    r = height / 2;
    if (letter == '1' || letter == '2' || letter == '3' || letter == '4' || letter == '5' || letter == '6' || letter == '7' || letter == '8' || letter == '9' || letter == '0')
    {
      r = height / 3.5;
    }
  }
  if (letter == ' ' || letter == '.')
  {
    // t = -((6.28319 / 26) * 25);
    r = 0;
  }

  if (letter == 'A' || letter == 'a')
  {
    t = -((6.28319 / 26) * 0);
  }
  if (letter == 'B' || letter == 'b')
  {
    t = -((6.28319 / 26) * 1);
  }
  if (letter == 'C' || letter == 'c')
  {
    t = -((6.28319 / 26) * 2);
  }
  if (letter == 'D' || letter == 'd')
  {
    t = -((6.28319 / 26) * 3);
  }
  if (letter == 'E' || letter == 'e')
  {
    t = -((6.28319 / 26) * 4);
  }
  if (letter == 'F' || letter == 'f')
  {
    t = -((6.28319 / 26) * 5);
  }
  if (letter == 'G' || letter == 'g')
  {
    t = -((6.28319 / 26) * 6);
  }
  if (letter == 'H' || letter == 'h')
  {
    t = -((6.28319 / 26) * 7);
  }
  if (letter == 'I' || letter == 'i')
  {
    t = -((6.28319 / 26) * 8);
  }
  if (letter == 'J' || letter == 'j')
  {
    t = -((6.28319 / 26) * 9);
  }
  if (letter == 'K' || letter == 'k')
  {
    t = -((6.28319 / 26) * 10);
  }
  if (letter == 'L' || letter == 'l')
  {
    t = -((6.28319 / 26) * 11);
  }
  if (letter == 'M' || letter == 'm')
  {
    t = -((6.28319 / 26) * 12);
  }
  if (letter == 'N' || letter == 'n')
  {
    t = -((6.28319 / 26) * 13);
  }
  if (letter == 'O' || letter == 'o')
  {
    t = -((6.28319 / 26) * 14);
  }
  if (letter == 'P' || letter == 'p')
  {
    t = -((6.28319 / 26) * 15);
  }
  if (letter == 'Q' || letter == 'q')
  {
    t = -((6.28319 / 26) * 16);
  }
  if (letter == 'R' || letter == 'r')
  {
    t = -((6.28319 / 26) * 17);
  }
  if (letter == 'S' || letter == 's')
  {
    t = -((6.28319 / 26) * 18);
  }
  if (letter == 'T' || letter == 't')
  {
    t = -((6.28319 / 26) * 19);
  }
  if (letter == 'U' || letter == 'u')
  {
    t = -((6.28319 / 26) * 20);
  }
  if (letter == 'V' || letter == 'v')
  {
    t = -((6.28319 / 26) * 21);
  }
  if (letter == 'W' || letter == 'w')
  {
    t = -((6.28319 / 26) * 22);
  }
  if (letter == 'X' || letter == 'x')
  {
    t = -((6.28319 / 26) * 23);
  }
  if (letter == 'Y' || letter == 'y')
  {
    t = -((6.28319 / 26) * 24);
  }
  if (letter == 'Z' || letter == 'z')
  {
    t = -((6.28319 / 26) * 25);
  }

  if (letter == '0')
  {
    t = -((6.28319 / 10) * 0);
  }
  if (letter == '1')
  {
    t = -((6.28319 / 10) * 1);
  }
  if (letter == '2')
  {
    t = -((6.28319 / 10) * 2);
  }
  if (letter == '3')
  {
    t = -((6.28319 / 10) * 3);
  }
  if (letter == '4')
  {
    t = -((6.28319 / 10) * 4);
  }
  if (letter == '5')
  {
    t = -((6.28319 / 10) * 5);
  }
  if (letter == '6')
  {
    t = -((6.28319 / 10) * 6);
  }
  if (letter == '7')
  {
    t = -((6.28319 / 10) * 7);
  }
  if (letter == '8')
  {
    t = -((6.28319 / 10) * 8);
  }
  if (letter == '9')
  {
    t = -((6.28319 / 10) * 9);
  }
}

// play the sentence
void playString(String sentence = " ")
{
  // Serial1.print("Sentence: ");
  // Serial1.println(sentence);
  if (xl.distanceToGo() == 0 && y.distanceToGo() == 0)
  { // go to next letter

    for (int i = 0; i <= sentence.length();)
    {
      // Serial1.println(i);
      if (i == sentence.length() && xl.distanceToGo() == 0 && y.distanceToGo() == 0)
      {
        // Serial1.print("dojechali >> ");
        // Serial1.print("bingo :");
        // Serial1.println(bingo);
        // bingo = 0;

        Serial1.println("done displaying");
        break;
      }
      if (xl.distanceToGo() == 0 && y.distanceToGo() == 0)
      {

        delay(2000);

        charPos(sentence.charAt(i));      // set t value
        xPos = r * cos(t) + (width / 2);  // calculate position of letters in a circle x = r*cos(t) + h, y = r*sin(t) + w , where h and w are the coordinates of the circle center
        yPos = r * sin(t) + (height / 2); // r = height/2 for big circle (letters), r = height/3 for small circle (numbers)

        moveToX(xPos);
        y.moveTo(yPos);

        i++;
      }
      runSpeedToPositionX();
      y.runSpeedToPosition();
    }
  }
}

void calibrate()
{
  Serial1.println("calibrating...");
  // Change these to suit your stepper if you want
  // gSpeed=10000.0;
  steppersSetup(15000.0, 10000.0, 100000.0); // steppersSetup(maxspeed, speed, accel)

  moveToX(steps);

  calibrateX();
  calibrateY();
  Serial1.println("calibration finished");
}

void setup()
{
  Serial1.begin(9600);
  Serial1.begin(9600); // the one for ESP

  pinMode(kontXtPin, INPUT_PULLUP); // kontaktron Xt
  pinMode(kontXbPin, INPUT_PULLUP); // kontatkron Xb
  pinMode(kontYlPin, INPUT_PULLUP); // kontaktron Yl
  pinMode(kontYrPin, INPUT_PULLUP); // kontatkron Yr
}

void loop()
{

  // if (Serial1.available())
  // {
  //   String message = Serial1.readString();
  //   message.trim(); // Remove any whitespace or newlines

  //   // Echo back what was received
  //   // Serial1.println(message);
  //   // delay(1000);
  //   // Serial1.println(message);
  //   // Blink LED 3 times
  // }

  if (Serial1.available())
  {
    String message = Serial1.readString(); // Read the entire command
    message.trim();

    // Split the message into command and argument (command:argument)
    int delimiterIndex = message.indexOf(' ');               // Find the first space
    String command = message.substring(0, delimiterIndex);   // Extract the command
    String argument = message.substring(delimiterIndex + 1); // Extract the argument (if any)

    // Parse the argument as an integer (for motor speed)
    // Handle the command
    if (command == String("DISPLAY"))
    {
      handleDisplay(argument); // No argument needed for stopping
      
    }
    else if (command == String("HOME"))
    {
      handleHome();
    }
    else if (command == String("MOVE"))
    {
      handleMove(argument);
    }
    else if (command == String("SET_SPEED"))
    {
      handleSetSpeed(argument);
    }
       else if (command == String("GET_SPEED"))
    {
      handleGetSpeed();
    }
       else if (command == String("CALIBRATE"))
    {
      calibrate();
    }
      else if (command == String("ECHO"))
    {
      handleEcho(argument);
    }
       else if (command == String("VERSION"))
    {
      handleVersion();
    }
  }
}


void handleDisplay(String params) {
  if (params.length() == 0) {
    Serial1.println("ERROR: nothing to display");
    return;
  }
  // Tutaj dodaj kod obsługujący wyświetlanie tekstu
  Serial1.println("OK");
  Serial1.print("Displaying: ");
  Serial1.println(params);
  playString(params);
}

void handleHome() {
  // Tutaj dodaj kod obsługujący powrót do pozycji domowej
  Serial1.println("OK");
  Serial1.println("Homing sequence initiated");
  playString(" "); //spacja jest na środku
}

void handleMove(String params) {
  int spaceIndex = params.indexOf(' ');
  if (spaceIndex == -1) {
    Serial1.println("ERROR: Invalid parameters");
    return;
  }
  String xString = params.substring(0, spaceIndex);
  String yString = params.substring(spaceIndex + 1);

  int xCoord = xString.toInt();
  int yCoord = yString.toInt();

  // Tutaj dodaj kod obsługujący ruch do pozycji (x, y)
  Serial1.println("OK");
  Serial1.print("Moving to X: ");
  Serial1.print(xCoord);
  Serial1.print(", Y: ");
  Serial1.println(yCoord);

  if (xl.distanceToGo() == 0 && y.distanceToGo() == 0)
      {
        moveToX(yCoord);
        y.moveTo(xCoord);

        runSpeedToPositionX();
        y.runSpeedToPosition();
      }
  Serial1.println("Done moving");    
}

void handleSetSpeed(String params) {
  float newSpeed = params.toFloat();
  if (newSpeed <= 0.0 || newSpeed > 100.0) {
    Serial1.println("ERROR: Invalid speed value. It must be between 0.0 and 100.0");
    return;
  }
  // speed = newSpeed;
  Serial1.println("OK");
  Serial1.print("Speed set to ");
  Serial1.println(newSpeed);

  gSpeed=map(newSpeed,0.0,100.0,0.0,15000.0);
  steppersSetup(15000.0, gSpeed, 100000.0); // steppersSetup(maxspeed, speed, accel)

}

void handleGetSpeed() {
  Serial1.print("SPEED ");
  Serial1.println(map(gSpeed,0.0,15000.0,0.0,100));
}

void handleCalibrate() {
  // Tutaj dodaj kod obsługujący kalibrację maszyny
  Serial1.println("OK");
  Serial1.println("Calibration started");
  calibrate();
}

void handleEcho(String params) {
  Serial1.print("ECHO ");
  Serial1.println(params);
}

void handleVersion() {
  Serial1.println("VERSION 0.9.0");
}
