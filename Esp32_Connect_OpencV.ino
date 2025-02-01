#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <HardwareSerial.h>

// WiFi credentials
const char* ssid = "TRUSTKISIA-TECH";
const char* password = "37526308";

int Green_Led = 26;
int Red_Led = 14;
int Blue_Led = 33;

enum State {
  CHECK_MOTION,
  CHECK_CAMERA,
  SEND_ALERT,
  RESET_SYSTEM
};

State currentState = CHECK_MOTION;

// Static IP configuration
IPAddress local_IP(192, 168, 100, 132);
IPAddress gateway(192, 168, 100, 1);
IPAddress subnet(255, 255, 255, 0);


unsigned long previousMillis = 0;
const long interval = 1000;

WebServer server(80);  // HTTP server on port 80
HardwareSerial GSM(1);

void setup() {
  Serial.begin(9600);
  GSM.begin(9600, SERIAL_8N1, 17, 16);

  delay(1000);

  GSM.println("AT");  //Once the handshake test is successful, it will back to OK
  updateSerial();

  GSM.println("AT+CMGF=1");  // Configuring TEXT mode
  updateSerial();
  GSM.println("AT+CNMI=1,2,0,0,0");  // Decides how newly arrived SMS messages should be handled
  updateSerial();

  Serial.println("Initializing GSM...");
  delay(10);

  pinMode(Green_Led, OUTPUT;);
  pinMode(Red_Led, OUTPUT);
  pinMode(Blue_Led, OUTPUT);

  digitalWrite(Green_Led, LOW);
  digitalWrite(Red_Led, HIGH);
  digitalWrite(Blue_Led, LOW);
 
  // Connect to Wi-Fi with static IP
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Failed to configure static IP!");
  }

  // Connect to Wi-Fi
  connectToWiFi();

  // Start the server
  server.on("/status", HTTP_POST, handlePostRequest);
  server.begin();
  Serial.println("Server started");
}

void loop() {
  runEverySecond();

  switch (currentState) {
    case CHECK_MOTION:
      check_motion();
      break;
    case CHECK_CAMERA:
      check_camera();
      break;
    case SEND_ALERT:
      send_alert();
      break;
    case RESET_SYSTEM:
      reset_system();
      break;
  }
}
void check_motion() {
}

void check_camera() {
  server.handleClient();  // Handle incoming client requests
}
void send_alert() {
}
void connectToWiFi() {
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(250);
      Serial.print(".");
    }
    Serial.println("Connected to Wi-Fi");
  }
}

void handlePostRequest() {
  if (server.hasArg("plain")) {
    String message = server.arg("plain");
    Serial.println("Received: " + message);

    // Respond based on the received message
    if (message == "1") {
      Serial.println("Known face detected");
    } else if (message == "0") {
      Serial.println("No face detected");
    } else if (message == "2") {
      Serial.println("Intruder detected");
      if (Sms == 0) {
        GSM.println("AT");  //Once the handshake test is successful, it will back to OK
        updateSerial();

        GSM.println("AT+CMGF=1");  // Configuring TEXT mode
        updateSerial();
        GSM.println("AT+CMGS=\"+254748613509\"");  //change ZZ with country code and xxxxxxxxxxx with phone number to sms
        updateSerial();
        GSM.print("Alert!! Intruder Detected Check The Camera Feed");  //text content
        updateSerial();
        GSM.write(26);
        Sms = 1;
        currentState = RESET_SYSTEM;
      }
    }

    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}
void reset_system() {
  if (GSM.available()) {                             // check if there is a message available
    String message = GSM.readString();               // read the message
    Serial.println("Received message: " + message);  // print the message to the serial monitor
    if (message.indexOf("RESET") != -1) {            // if the message contains "ON"
      Serial.println("System Reseting...");
      currentState = CHECK_MOTION;
    } else if (message.indexOf("OFF") != -1) {  // if the message contains "OFF"

      Serial.println("Relay turned OFF.");
    }
  }
  delay(1000);
}
void runEverySecond() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (GSM.available()) {                             // check if there is a message available
      String message = GSM.readString();               // read the message
      Serial.println("Received message: " + message);  // print the message to the serial monitor
      if (message.indexOf("ON") != -1) {            // if the message contains "ON"
        digitalWrite(Red_Led, HIGH);
      } else if (message.indexOf("OFF") != -1) {  // if the message contains "OFF"
        digitalWrite(Red_Led, LOW);
      }
    }
    delay(1000);
  }
}
void updateSerial() {
  delay(500);
  while (Serial.available()) {
    GSM.write(Serial.read());  //Forward what Serial received to Software Serial Port
  }
  while (GSM.available()) {
    Serial.write(GSM.read());  //Forward what Software Serial received to Serial Port
  }
}