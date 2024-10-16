#include <SoftwareSerial.h>
// ESP8266 code
#define LED_BUILTIN 2

// Define SoftwareSerial pins (D1 and D2 are GPIO 5 and GPIO 4 on ESP8266)
SoftwareSerial softSerial(4, 5); // RX, TX

void setup()
{
  Serial.begin(9600);
  softSerial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // LED off initially
}

void blinkLED(int times)
{
  for (int i = 0; i < times; i++)
  {
    digitalWrite(LED_BUILTIN, LOW); // LED ON
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH); // LED OFF
    delay(200);
  }
}

void loop()
{

  if (Serial.available())
  {
    String message = Serial.readString();
    message.trim(); // Remove any whitespace or newlines

    // Echo back what was received
    // Serial.print("Received: ");
    // Serial.println(message);
    softSerial.println("write:" + message);
    // Blink LED 3 times
    blinkLED(2);
  }
  if (softSerial.available())
  {
    blinkLED(5);
    String message = softSerial.readString();
    message.trim(); // Remove any whitespace or newlines

    Serial.println("recieved from mega: " + message);
  }
}