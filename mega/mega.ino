#include <AccelStepper.h>

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

boolean isStopped = false;

// set contactrons pins
int kontXtPin = 2;
int kontXbPin = 3;
int kontYlPin = 4;
int kontYrPin = 5;

// stepper settings
long steps = 10000; // default 5000
long width = 0;
long height = 0;
bool calibratedX = false;
bool calibratedY = false;

// letter positioning variables
float t = 0;
long r = 0; // radius
long xPos = 0;
long yPos = 0;

// declare speed
float gSpeed = 10000.0;

// Define structures for character positions
struct Position
{
  int x; // X coordinate (0-50)
  int y; // Y coordinate (0-50)
};

#define NUM_LETTERS 26
#define NUM_NUMBERS 10

// Arrays to store x,y positions for each character
Position letterPositions[NUM_LETTERS]; // A-Z
Position numberPositions[NUM_NUMBERS]; // 0-9

// Initialize default positions in a circle
void initializeDefaultPositions()
{
  // Calculate default positions for letters (larger circle)
  for (int i = 0; i < NUM_LETTERS; i++)
  {
    float angle = -((6.28319 / 26) * i);
    // Convert from polar to cartesian coordinates and map to 0-50 range
    letterPositions[i].x = 25 + round(25 * cos(angle));
    letterPositions[i].y = 25 + round(25 * sin(angle));
  }

  // Calculate default positions for numbers (smaller circle)
  for (int i = 0; i < NUM_NUMBERS; i++)
  {
    float angle = -((6.28319 / 10) * i);
    // Use smaller radius for numbers
    numberPositions[i].x = 25 + round(15 * cos(angle));
    numberPositions[i].y = 25 + round(15 * sin(angle));
  }
}

// declare functions
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
      if (direction == 1)
      {
        if (kontaktXt == 1)
        {
          moveX(steps);
        }
        else
        {
          setCurrentPositionX(0);
          direction = 0;
        }
      }
      else
      {
        if (kontaktXb == 1)
        {
          moveX(-steps);
        }
        else
        {
          stopX();
          width = xl.currentPosition();
          delay(1000);
          done = true;
          moveToX(width / 2);
        }
      }
      runSpeedX();
    }
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

      if (direction == 0)
      {
        if (kontaktYl == 1)
        {
          y.move(steps);
        }
        else
        {
          y.setCurrentPosition(0);
          direction = 1;
        }
      }
      else
      {
        if (kontaktYr == 1)
        {
          y.move(-steps);
        }
        else
        {
          y.stop();
          height = y.currentPosition();
          delay(1000);
          done = true;
          y.moveTo(height / 2);
          continue;
        }
      }
      y.runSpeed();
    }
    else
    {
      y.runSpeedToPosition();
      if (y.distanceToGo() == 0)
      {
        calibratedY = true;
        Serial1.println("Y calibrated");
        // Serial1.print("width :" + width);
        // Serial1.println(" height :" + height);
        return true;
      }
    }
  }
}

void charPos(char letter = 'a')
{
  if (letter == ' ' || letter == '.')
  {
    // Center position
    xPos = width / 2;
    yPos = height / 2;
    return;
  }

  Position pos;
  if (isAlpha(letter))
  {
    int index = toupper(letter) - 'A';
    if (index >= 0 && index < NUM_LETTERS)
    {
      pos = letterPositions[index];
    }
  }
  else if (isDigit(letter))
  {
    int index = letter - '0';
    if (index >= 0 && index < NUM_NUMBERS)
    {
      pos = numberPositions[index];
    }
  }

  // Map the 0-50 position to actual width/height
  xPos = map(pos.x, 0, 50, 0, width);
  yPos = map(pos.y, 0, 50, 0, height);
}

void playString(String sentence = " ")
{
  int kontaktYl = digitalRead(kontYlPin);
  int kontaktYr = digitalRead(kontYrPin);
  int kontaktXt = digitalRead(kontXtPin);
  int kontaktXb = digitalRead(kontXbPin);

  if (xl.distanceToGo() == 0 && y.distanceToGo() == 0)
  {
    for (int i = 0; i <= sentence.length();)
    {
      if (i == sentence.length() && xl.distanceToGo() == 0 && y.distanceToGo() == 0)
      {
        Serial1.println("done displaying");
        break;
      }

      if (xl.distanceToGo() == 0 && y.distanceToGo() == 0)
      {
        charPos(sentence.charAt(i));
        delay(2000);
        moveToX(xPos);
        y.moveTo(yPos);
        i++;
      }

      if (Serial1.available() || kontaktYl == 0 || kontaktYr == 0 || kontaktXt == 0 || kontaktXb == 0)
      {
        String message = Serial1.readString();
        message.trim();
        if (message == String("STOP"))
        {
          Serial1.println("Movement stopped");
          moveToX(xl.currentPosition());
          y.moveTo(y.currentPosition());
          runSpeedToPositionX();
          y.runSpeedToPosition();
          return;
        }
      }
      runSpeedToPositionX();
      y.runSpeedToPosition();
    }
  }
}

void calibrate()
{
  Serial1.println("calibrating...");
  steppersSetup(15000.0, 10000.0, 100000.0);

  moveToX(steps);
  calibratedX = false;
  calibratedY = false;
  calibrateX();
  calibrateY();
}

// Command handlers
void handleDisplay(String params)
{
  if (params.length() == 0)
  {
    Serial1.println("ERROR: nothing to display");
    return;
  }
  Serial1.println("OK");
  Serial1.print("Displaying: ");
  Serial1.println(params);
  playString(params);
}

void handleHome()
{
  Serial1.println("OK");
  Serial1.println("Homing sequence initiated");
  playString(" ");
}

void handleMove(String params)
{
  int spaceIndex = params.indexOf(' ');
  if (spaceIndex == -1)
  {
    Serial1.println("ERROR: Invalid parameters");
    return;
  }
  String xString = params.substring(0, spaceIndex);
  String yString = params.substring(spaceIndex + 1);

  long xCoord = map(xString.toInt(), 0, 50, 0, width);
  long yCoord = map(yString.toInt(), 0, 50, 0, height);

  Serial1.println("OK");
  Serial1.print("Moving to X: ");
  Serial1.print(xCoord);
  Serial1.print(", Y: ");
  Serial1.println(yCoord);

  moveToX(xCoord);
  y.moveTo(yCoord);

  while (xl.distanceToGo() != 0 || xr.distanceToGo() != 0 || y.distanceToGo() != 0)
  {
    if (Serial1.available())
    {
      String message = Serial1.readString();
      message.trim();
      if (message == String("STOP"))
      {
        Serial1.println("Movement stopped");
        return;
      }
    }
    runSpeedToPositionX();
    y.runSpeedToPosition();
  }

  Serial1.println("Done moving");
}

void handleSetSpeed(String params)
{
  float newSpeed = params.toFloat();
  if (newSpeed <= 0.0 || newSpeed > 100.0)
  {
    Serial1.println("ERROR: Invalid speed value. It must be between 0.0 and 100.0");
    return;
  }
  Serial1.println("OK");
  Serial1.print("Speed set to ");
  Serial1.println(newSpeed);

  gSpeed = map(newSpeed, 0, 100, 0, 15000);
  steppersSetup(15000.0, gSpeed, 100000.0);
}

void handleGetSpeed()
{
  Serial1.print("SPEED ");
  Serial1.println(map(gSpeed, 0, 15000, 0, 100));
}


void handleCharPos(String params)
{
  // Expected format: "CHARPOS [char] [x] [y]"
  int firstSpace = params.indexOf(' ');
  int secondSpace = params.indexOf(' ', firstSpace + 1);

  if (firstSpace == -1 || secondSpace == -1)
  {
    Serial1.println("ERROR: Invalid CHARPOS format. Use: CHARPOS [char] [x] [y]");
    return;
  }

  String charStr = params.substring(0, firstSpace);
  String xStr = params.substring(firstSpace + 1, secondSpace);
  String yStr = params.substring(secondSpace + 1);

  char targetChar = charStr.charAt(0);
  int newX = xStr.toInt();
  int newY = yStr.toInt();

  // Validate coordinates
  if (newX < 0 || newX > 50 || newY < 0 || newY > 50)
  {
    Serial1.println("ERROR: Coordinates must be between 0 and 50");
    return;
  }

  // Update the appropriate array
  if (isAlpha(targetChar))
  {
    int index = toupper(targetChar) - 'A';
    if (index >= 0 && index < NUM_LETTERS)
    {
      letterPositions[index].x = newX;
      letterPositions[index].y = newY;
      Serial1.print("OK: Updated position for '");
      Serial1.print(targetChar);
      Serial1.print("' to x=");
      Serial1.print(newX);
      Serial1.print(" y=");
      Serial1.println(newY);
    }
    else
    {
      Serial1.println("ERROR: Invalid letter");
    }
  }
  else if (isDigit(targetChar))
  {
    int index = targetChar - '0';
    if (index >= 0 && index < NUM_NUMBERS)
    {
      numberPositions[index].x = newX;
      numberPositions[index].y = newY;
      Serial1.print("OK: Updated position for '");
      Serial1.print(targetChar);
      Serial1.print("' to x=");
      Serial1.print(newX);
      Serial1.print(" y=");
      Serial1.println(newY);
    }
    else
    {
      Serial1.println("ERROR: Invalid number");
    }
  }
  else
  {
    Serial1.println("ERROR: Invalid character");
  }
}

void handleCalibrate()
{
  Serial1.println("OK");
  Serial1.println("Calibration started");
  calibrate();
}

void handleEcho(String params)
{
  Serial1.print("ECHO ");
  Serial1.println(params);
}

void handleVersion()
{
  Serial1.println("VERSION 0.9.2 08.02.2025");
}

void handleGetChars()
{
  // Print all letter positions
  Serial1.println("Letter positions:");
  for (int i = 0; i < NUM_LETTERS; i++)
  {
    char letter = 'A' + i;
    Serial1.print(letter);
    Serial1.print(": x=");
    Serial1.print(letterPositions[i].x);
    Serial1.print(" y=");
    Serial1.println(letterPositions[i].y);
  }

  // Print all number positions
  Serial1.println("\nNumber positions:");
  for (int i = 0; i < NUM_NUMBERS; i++)
  {
    Serial1.print(i);
    Serial1.print(": x=");
    Serial1.print(numberPositions[i].x);
    Serial1.print(" y=");
    Serial1.println(numberPositions[i].y);
  }
}

void handleStop()
{
  Serial1.print("Stopping...");
  isStopped = true;
}

void setup()
{
  Serial1.begin(9600);

  pinMode(kontXtPin, INPUT_PULLUP);
  pinMode(kontXbPin, INPUT_PULLUP);
  pinMode(kontYlPin, INPUT_PULLUP);
  pinMode(kontYrPin, INPUT_PULLUP);

  // Initialize default character positions
  initializeDefaultPositions();
  // calibrate
  calibrate();
}

void loop()
{
  if (Serial1.available())
  {
    String message = Serial1.readString();
    message.trim();

    int delimiterIndex = message.indexOf(' ');
    String command = delimiterIndex == -1 ? message : message.substring(0, delimiterIndex);
    String argument = delimiterIndex == -1 ? "" : message.substring(delimiterIndex + 1);

    if (command == String("CHARPOS"))
    {
      handleCharPos(argument);
    }
    else if (command == String("DISPLAY"))
    {
      handleDisplay(argument);
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
      handleCalibrate();
    }
    else if (command == String("ECHO"))
    {
      handleEcho(argument);
    }
    else if (command == String("VERSION"))
    {
      handleVersion();
    }
    else if (command == String("STOP"))
    {
      handleStop();
    }
    else if (command == String("GET_CHARS"))
    {
      handleGetChars();
    }
  }
}