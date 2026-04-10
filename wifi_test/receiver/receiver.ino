/*
  RECEIVER BOARD
  Listens for UDP packets and prints them to the Serial Monitor.
*/

#include <WiFiS3.h>
#include <WiFiUdp.h>
#include "secrets.h"

// --- Update these with your network details ---
char ssid[] = WIFI_NAME;      
char pass[] = WIFI_PASS;  

int status = WL_IDLE_STATUS;
unsigned int localPort = 2390; // The port we are listening on
char packetBuffer[256];        // Buffer to hold incoming packets

WiFiUDP Udp;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect
  }

  // Check for the WiFi module
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true); // Don't continue
  }

  // Attempt to connect to WiFi network
  Serial.print("Connecting to SSID: ");
  Serial.println(ssid);
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    delay(5000); // Wait 5 seconds for connection
  }

  Serial.println("Connected to WiFi!");
  Serial.print("IMPORTANT: The Receiver IP Address is: ");
  Serial.println(WiFi.localIP()); // WRITE THIS DOWN FOR THE SENDER CODE

  Udp.begin(localPort);
  Serial.print("Listening on port ");
  Serial.println(localPort);
}

void loop() {
  // Check if a packet has been received
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From IP: ");
    Serial.println(Udp.remoteIP());

    // Read the packet into the buffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0; // Null-terminate the string
    }
    
    Serial.print("Message: ");
    Serial.println(packetBuffer);
    Serial.println("--------------------");
  }
}