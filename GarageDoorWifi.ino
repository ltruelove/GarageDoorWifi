// These libraries are required to work with the ESP8266. I'm pretty sure I got what I needed
// here: https://github.com/esp8266/Arduino
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// These are just dummy values. You'll want to configure you're router's SSID and password
// as well as set up some IPs that can be static and use one of them here. Make sure you set
// gateway and subnet correctly by checking any computer that's connected to your wifi for the
// correct values.
const char* ssid = "....";
const char* password = "....";
IPAddress ip(127, 0, 0, 1);
IPAddress gateway(127, 0, 0, 1);
IPAddress subnet(127, 0, 0, 1);

ESP8266WebServer server(80);

const char* message = "{\"doorStatus\" : \"";
String readString;

// This handles click requests and sends the "1" via serial to anything listening
void handleClick() {
  Serial.write("1");
  server.send(200, "text/plain", "clicked button");
}

// GarageDoorArduino doesn't handle this yet
void handleStatus() {
  readString = "";
  Serial.write("s");
  delay(15);
  while(Serial.available() <= 0){}
  while(Serial.available()){
    delay(3);
    char c = Serial.read();
    readString += c;
  }
  readString.trim();

  String response = String(message);
  response.concat(readString);
  response.concat("\"}");
  
  server.send(200, "application/json", response);
}

void handleNotFound(){
  //digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  //digitalWrite(led, 0);
}

void setup(void){
  Serial.begin(9600);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  String ip = WiFi.localIP().toString();

  server.on("/click", handleClick);
  server.on("/status", handleStatus);
  server.onNotFound(handleNotFound);

  server.begin();
}

void loop(void){
  server.handleClient();
}

