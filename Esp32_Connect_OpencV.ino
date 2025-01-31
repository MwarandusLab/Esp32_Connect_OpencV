#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

// WiFi credentials
const char* ssid = "TRUSTKISIA-TECH";      
const char* password = "37526308";         

enum State{
  CHECK_MOTION,
  CHECK_CAMERA,
  SEND_ALERT
};
 
State currentState = CHECK_MOTION;

// Static IP configuration
IPAddress local_IP(192, 168, 100, 132);
IPAddress gateway(192, 168, 100, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);  // HTTP server on port 80

void setup() {
  Serial.begin(115200);
  delay(10);

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
  switch(currentState){
    case CHECK_MOTION:
      check_motion();
      break;
    case CHECK_CAMERA:
      check_camera();
      break;
    case SEND_ALERT:
      send_alert();
      break;
  }
}
void check_motion(){

}

void check_camera(){
  server.handleClient();  // Handle incoming client requests
}
void send_alert(){

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
    }
    
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}
