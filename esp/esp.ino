#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>

const char* ssid = "smacznego";
const char* password = "poker1234";
SoftwareSerial mySerial(13, 12);
ESP8266WebServer server(80);
String serialLog = "";
const int LED_PIN = LED_BUILTIN;
unsigned long previousMillis = 0;
const long interval = 1000;
bool ledState = false;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, LOW);
    delay(200);
    digitalWrite(LED_PIN, HIGH);
    delay(200);
  }
  Serial.begin(9600);
  mySerial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.on("/", handleRoot);
  server.on("/send", HTTP_POST, handleSend);
  server.on("/read", handleRead);
  server.on("/clear", HTTP_POST, handleClear);
  
  server.begin();
}

void loop() {
  server.handleClient();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }
}

void handleRoot() {
  server.send(200, "text/html", 
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<style>"
    "body {"
    "  display: flex;"
    "  justify-content: center;"
    "  align-items: center;"
    "  flex-direction: column;"
    "  font-family: Arial, sans-serif;"
    "  max-width: 600px;"
    "  margin: 0 auto;"
    "  text-align: center;"
    "}"
    "form { margin: 20px 0; }"
    "#serialLog {"
    "  border: 1px solid #ccc;"
    "  height: 300px;"
    "  overflow-y: scroll;"
    "  width: 100%;"
    "  text-align: left;"
    "  padding: 10px;"
    "  white-space: pre-wrap;"           // Added this line
    "  word-wrap: break-word;"           // Added this line
    "  font-family: monospace;"          // Added this line
    "  line-height: 1.4;"               // Added this line
    "}"
    ".button {"
    "  margin: 10px;"
    "  padding: 5px 15px;"
    "  cursor: pointer;"
    "}"
    "</style>"
    "</head>"
    "<body>"
    "<h1>Node MCU Serial Communication</h1>"
    "<form id=\"messageForm\">"
    "<input type=\"text\" id=\"messageInput\" placeholder=\"Enter message\">"
    "<input type=\"submit\" value=\"Send\">"
    "</form>"
    "<button onclick=\"clearSerialLog()\" class=\"button\">Clear Output</button>"
    "<div id=\"serialLog\"></div>"
    "<script>"
    "document.getElementById(\"messageForm\").addEventListener(\"submit\", function(e) {"
    "  e.preventDefault();"
    "  var message = document.getElementById(\"messageInput\").value;"
    "  fetch(\"/send\", {"
    "    method: \"POST\","
    "    body: new URLSearchParams({message: message}),"
    "    headers: {"
    "      \"Content-Type\": \"application/x-www-form-urlencoded\""
    "    }"
    "  });"
    "  document.getElementById(\"messageInput\").value = \"\";"
    "});"
    ""
    "function updateSerialData() {"
    "  fetch(\"/read\")"
    "    .then(response => response.text())"
    "    .then(data => {"
    "      document.getElementById(\"serialLog\").innerHTML = data;"
    "      const serialLog = document.getElementById(\"serialLog\");"
    "      serialLog.scrollTop = serialLog.scrollHeight;"    // Added this line
    "    });"
    "}"
    ""
    "function clearSerialLog() {"
    "  fetch(\"/clear\", {"
    "    method: \"POST\""
    "  }).then(() => {"
    "    document.getElementById(\"serialLog\").innerHTML = \"\";"
    "  });"
    "}"
    ""
    "setInterval(updateSerialData, 1000);"
    "</script>"
    "</body>"
    "</html>"
  );
}

void handleSend() {
  if (server.hasArg("message")) {
    String message = server.arg("message");
    mySerial.println(message);
    serialLog += "Sent: " + message + "\n";    // Using single backslash for newline
    server.send(200, "text/plain", "Message sent");
  } else {
    server.send(400, "text/plain", "No message received");
  }
}

void handleRead() {
  String receivedMessage = "";
  while (mySerial.available()) {
    receivedMessage += mySerial.readStringUntil('\n');
  }
  if (receivedMessage.length() > 0) {
    serialLog += "Received: " + receivedMessage + "\n";    // Using single backslash for newline
  }
  server.send(200, "text/html", serialLog);
}

void handleClear() {
  serialLog = "";
  server.send(200, "text/plain", "Log cleared");
}