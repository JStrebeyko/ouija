
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

// bool ACCEPT_DOUBLE_TAGS = false; //trus

// String txt[10];

// // texts to display
// String txt1 = "1";
// String txt2 = "2";
// String txt3 = "3";
// String txt4 = "4";
// String txt5 = "5";
// String txt6 = "6";
// String txt7 = "7";
// String txt8 = "8";
// String txt9 = "9";

// // List of Tags UIDs
// byte tagarray[][4] = {
//     {243, 36, 177, 207},  // Tag A
//     {218, 136, 125, 122}, // Tag B
//     {218, 94, 219, 122},  // Tag C
//     {50, 243, 12, 255},   // Tag D
//     {193, 50, 55, 13},    // Tag E
//     {123, 194, 17, 255},  // Tag F
//     {74, 188, 129, 129},  // Tag G
//     {74, 220, 70, 129},   // Tag H
//     {50, 113, 84, 207}    // Tag I
// };

// String txt0 = " ";

// calibration switch pin
// int calibrationSwitch = 31; // gnd = 0, 5v = 1

// pins for rfid
/*
SS pins on the shield: 25,26,27,28,29
default: 53
*/

// // 5 RFID PINs
// #define SS_1_PIN 24 // Configurable, take a unused pin, only HIGH/LOW required, must be different to SS 2
// #define SS_2_PIN 25 // Configurable, take a unused pin, only HIGH/LOW required, must be different to SS 1
// #define SS_3_PIN 26 // Configurable, take a unused pin, only HIGH/LOW required, must be different to SS 2
// #define SS_4_PIN 27
// #define SS_5_PIN 28
// #define NR_OF_READERS 5

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

// int reader_already_connected = -1;

// letter positioning variables
float t = 0;
long r = 0; // radius
long xPos = 0;
long yPos = 0;

// rfid variables
// int elock = 39;

// byte ssPins[] = {SS_1_PIN, SS_2_PIN, SS_3_PIN, SS_4_PIN, SS_5_PIN};
// MFRC522 mfrc522[NR_OF_READERS]; // Create MFRC522 instance.

// int bingo = 0;

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
          Serial.print("calibratedX: width = ");
          Serial.println(width);
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
        Serial.println("skonczylismy kalibrowanie X");
      }
    }
  }

  // return true;
  //  when not calibratedX, use runSpeed
}

bool calibrateY()
{
  bool done = false;
  Serial.println("zaczynamy y");
  int direction = 0;
  while (!calibratedY)
  {
    if (!done)
    {

      int kontaktYl = digitalRead(kontYlPin);
      int kontaktYr = digitalRead(kontYrPin);
      // Serial.println("kontYl  " + kontaktYl);
      // Serial.println("kontYr  " + kontaktYr);

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
          Serial.println("y change direction  ");
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
          Serial.print("calibratedY = ");
          Serial.println(height);
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
        Serial.println("skonczylismy kalibrowanie Y");
        return true;
      }
    }
  }

  // when not calibratedX, use runSpeed
}

// when not calibratedY, use runSpeed

// letter positioning
void charPos(char letter = 'a')
{ // letter positions
  // proba literkowa
  // String txt1 = "siema eniu tu dupa";
  // for (int i = 0; i <= txt1.length(); i++) {
  // Serial.println(txt1.charAt(i));
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

  // }
}

// play the sentence
void playString(String sentence = "bingo")
{
  Serial.print("Sentence: ");
  Serial.println(sentence);
  if (xl.distanceToGo() == 0 && y.distanceToGo() == 0)
  { // go to next letter

    for (int i = 0; i <= sentence.length();)
    {
      // Serial.println(i);
      if (i == sentence.length() && xl.distanceToGo() == 0 && y.distanceToGo() == 0)
      {
        Serial.print("dojechali >> ");
        Serial.print("bingo :");
        Serial.println(bingo);
        bingo = 0;

        Serial.println("dojechali 22");
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

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
// void dump_byte_array(byte *buffer, byte bufferSize)
// {
//   Serial.println(buffer[0]);
//   for (byte i = 0; i < bufferSize; i++)
//   {
//     Serial.print(buffer[i] < 0x10 ? " 0" : " ");
//     Serial.print(buffer[i], HEX);
//   }
// }

void calibrate()
{
  Serial.println("calibration");
  // Change these to suit your stepper if you want
  steppersSetup(15000.0, 10000.0, 100000.0); // steppersSetup(maxspeed, speed, accel)

  moveToX(steps);

  calibrateX();
  calibrateY();
}

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600); // the one for ESP

  pinMode(kontXtPin, INPUT_PULLUP); // kontaktron Xt
  pinMode(kontXbPin, INPUT_PULLUP); // kontatkron Xb
  pinMode(kontYlPin, INPUT_PULLUP); // kontaktron Yl
  pinMode(kontYrPin, INPUT_PULLUP); // kontatkron Yr

  // rfid reader setup
  // pinMode(elock, OUTPUT);
  // digitalWrite(elock, LOW);

  // SPI.begin(); // Init SPI bus

  // for (uint8_t reader = 0; reader < NR_OF_READERS; reader++)
  // {
  //   mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN); // Init each MFRC522 card
  //   Serial.print(F("Reader "));
  //   Serial.print(reader);
  //   Serial.print(F(": "));
  //   mfrc522[reader].PCD_DumpVersionToSerial();
  // }

  // prepare sentences array
  // TODO delete
  // txt[0] = txt0;
  // txt[1] = txt1 + " ";
  // txt[2] = txt2 + " ";
  // txt[3] = txt3 + " ";
  // txt[4] = txt4 + " ";
  // txt[5] = txt5 + " ";
  // txt[6] = txt6 + " ";
  // txt[7] = txt7 + " ";
  // txt[8] = txt8 + " ";
  // txt[9] = txt9 + " ";

  // set the calibration switch pin to input
  // pinMode(calibrationSwitch, INPUT);

  // if (digitalRead(calibrationSwitch) == 0)
  // { // gnd = 0, 5v = 1

  // calibrate the machine

  // calibrateX();
  // calibrateY();

  // }
  // if (digitalRead(calibrationSwitch) == 1)
  // {
  //   calibratedX = true;
  //   calibratedY = true;
  //   setCurrentPositionX(0);
  //   y.setCurrentPosition(0);
  //   Serial.println("skipped");
  // }
}

// todo: redo
// String get_sentence(MFRC522::Uid uid)
// {
//   for (int x = 0; x < sizeof(tagarray); x++) // tagarray's row
//   {
//     for (int i = 0; i < uid.size; i++) // tagarray's columns
//     {
//       if (uid.uidByte[i] == tagarray[x][i]) // Comparing the UID in the buffer to the UID in the tag array.
//       {
//         if (i == uid.size - 1)
//         {
//           Serial.print("txt[x]: ");
//           Serial.println(txt[x]);
//           return txt[x];
//         }
//         continue;
//       }
//       else
//       {
//         break;
//       }
//     } // If the Tag is allowed, quit the test.
//   }
//   return txt[0];
// };

// // todo: redo
// String get_double_sentence(MFRC522::Uid uid_one, MFRC522::Uid uid_two)
// {
//   return "baba ";
// };
void loop()
{
  if (Serial1.available())
  {
    String message = Serial1.readStringUntil('\n'); // Read the entire command
    Serial.println("Received message: " + message); // Print for debugging

    // Split the message into command and argument (command:argument)
    int delimiterIndex = message.indexOf(':');               // Find the ':' delimiter
    String command = message.substring(0, delimiterIndex);   // Extract the command
    String argument = message.substring(delimiterIndex + 1); // Extract the argument (if any)

    // Parse the argument as an integer (for motor speed)
    // Handle the command
    if (command == "calibrate")
    {
      calibrate(); // Pass the argument (e.g., speed) to the function
    }
    else if (command == "write")
    {
      playString(argument); // No argument needed for stopping
    }
  }
}
