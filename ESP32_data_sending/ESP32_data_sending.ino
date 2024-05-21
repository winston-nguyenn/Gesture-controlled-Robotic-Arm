//////////////////////////////////////////////////

#include <Wire.h>
#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"


// Set your access point network credentials
const char* ssid = "ESP8266-Access-Point";
const char* password = "123456789";
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String sent_data;

String reading_data() {
  return sent_data;
};


void setup(void) {
	//Serial.begin(9600);
  Serial.begin(9600, SERIAL_8N2);
    // Setting the ESP as an access point
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", reading_data().c_str());
  });

  // Start server
  server.begin();
	delay(100);
}

void loop() {
    if (Serial.available()) {
        sent_data = Serial.readStringUntil('\n');  // Read the full line of data until newline
    }
}