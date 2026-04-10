/*
  SENDER BOARD (AUTO-DISCOVERY VIA BROADCAST)
  Broadcasts a UDP packet to the whole network every 3 seconds.
*/

#include <WiFiS3.h>
#include <WiFiUdp.h>
#include "secrets.h"

// --- Update these with your network details ---
char ssid[] = WIFI_NAME;      
char pass[] = WIFI_PASS;  

// --- The Magic Broadcast Address ---
// 255.255.255.255 tells the router to send this to EVERY device on the network.
IPAddress broadcastIP(255, 255, 255, 255); 
unsigned int receiverPort = 2390;      

int status = WL_IDLE_STATUS;
WiFiUDP Udp;
int counter = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect
  }

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true); 
  }

  Serial.print("Connecting to SSID: ");
  Serial.println(ssid);
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }

  Serial.println("Connected to WiFi!");
  Udp.begin(2391); // Start UDP on the sender
}

void loop() {
  Serial.print("Broadcasting message ");
  Serial.println(counter);

  String message = "Hello everyone! Count: " + String(counter);

  // Send the UDP packet to the broadcast IP
  Udp.beginPacket(broadcastIP, receiverPort);
  Udp.print(message);
  Udp.endPacket();

  counter++;
  delay(3000); 
}