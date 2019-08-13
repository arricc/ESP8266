/* HEADER
  Wifi module to control a simple relay

  Mark McRitchie 2019-08-04
  https://github.com/arricc/

  Based on:
  https://www.az-delivery.de/products/esp8266-01-mit-relais-kostenfreies-e-book?_pos=8&_sid=daf20fc36&_ss=r&ls=en
  and comments from https://www.hackster.io/makerrelay/esp8266-wifi-5v-1-channel-relay-delay-module-iot-smart-home-e8a437
  
  Wiring:
   Power for Logic board
    IN-  GND
    IN+  5V (not 3.3V!)

   Switched line AC or DC
    IN   NO 
    OUT  COM
    IN#2 NC

    COM-NC connected when relay is Off
    COM-NO connected when relay is On

*/

/* INCLUDE FOR WIFI */
#include <ESP8266WiFi.h>
#include "credentials.h"

const char ssid[] = WIFI_SSID;
const char password[] = WIFI_PASSWORD;

byte rel1ON[] = {0xA0, 0x01, 0x01, 0xA2}; //Hex command to send to serial for open relay
byte rel1OFF[] = {0xA0, 0x01, 0x00, 0xA1}; //Hex command to send to serial for close relay

int relayPin = 2; // GPIO2 of ESP8266 - on board LED
WiFiServer ESPserver(80);//Service Port


void RelayOff() {
  digitalWrite(relayPin, HIGH);
  Serial.write(rel1OFF, sizeof(rel1OFF));
  Serial.println("RELAY is OFF");
}

void RelayOn() {
  digitalWrite(relayPin, LOW);
  Serial.write(rel1ON, sizeof(rel1ON));
  Serial.println("RELAY is ON");
}

void setup() {
  //Must be 9600 as the other chips on the board read at this speed.
  Serial.begin(9600); 
  
  pinMode(relayPin, OUTPUT);
  RelayOff();
  
  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  delay(5000);
  
  /* The following four line of the
    code will assign a Static IP Address to
    the ESP Module. If you do not want this,
    comment out the following four lines.*/
  /* IPAddress ip(192,168,0,254);
    IPAddress gateway(192,168,0,1);
    IPAddress subnet(255,255,255,0);
    WiFi.config(ip, gateway, subnet); */
  delay(5000);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print("*");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  // Start the server
  ESPserver.begin();
  Serial.println("Server started");
  // Print the IP address
  Serial.print("The URL to control ESP8266: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
}


void loop() {
  // Check if a client has connected
  WiFiClient client = ESPserver.available();
  if (!client) {
    return;
  }
  // Wait until the client sends some data
  Serial.println("New Client");
  while (!client.available()) {
    delay(1);
  }
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
  // Match the request

  if (request.indexOf("/RELAYON") != -1) {
    RelayOn();
  }
  if (request.indexOf("/RELAYOFF") != -1) {
    RelayOff();
  }
  int value = digitalRead(relayPin);

  // Return some basic HTML
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); // IMPORTANT
  client.println("<!DOCTYPE HTML>");
  client.println("<html><body>");
  client.print("Status of the RELAY: ");
  if (value == LOW) {
    client.println("ON");
  }
  else {
    client.println("OFF");
  }
  client.println("<br/><br/><a href='/'>STATUS</a> - <a href='/RELAYON'>ON</a> - <a href='/RELAYOFF'>OFF</a><br/>");
  client.println("</body></html>");
  delay(1);
  //client.stop();
  Serial.println("Client disconnected");
  Serial.println("");
}
