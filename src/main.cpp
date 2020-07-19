#include <Arduino.h> // necessary to work with "arduino environment"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h> 
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>   // Include the WebServer library

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

void handleRoot();              // function prototypes for HTTP handlers
void handleColor();
void getColor();
void handleNotFound();
void aleatoryMode();
void setRandomColors();

int *colorStringToArray(String colorString);

int color[] = { 255, 255, 255 };

const int ledOne[] = { 4, 5, 16 };
const int ledTwo[] = { 14, 13, 12 };

boolean isRandom = false;
int red = random(0, 255);
int green = random(0, 255);
int blue = random(0, 255);

void setup(void){
  
  pinMode(ledOne[0], OUTPUT);
  pinMode(ledOne[1], OUTPUT);
  pinMode(ledOne[2], OUTPUT);

  pinMode(ledTwo[0], OUTPUT);
  pinMode(ledTwo[1], OUTPUT);
  pinMode(ledTwo[2], OUTPUT);

  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  wifiMulti.addAP("LainesSobral 5g", "DontEnter1997@");   // add Wi-Fi networks you want to connect to

  Serial.println("Connecting ...");
  // int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(250);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer

  if (MDNS.begin("esp8266")) {              // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  server.on("/", handleRoot);   
  server.on("/color", handleColor);
  server.on("/currentcolor", getColor);
  server.on("/aleatory-mode", aleatoryMode);
  server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  server.begin();                           // Actually start the server
  Serial.println("HTTP server started");
}

void loop(void){

  if(isRandom) {
    for(int i = 0; i < 4; i++) {
      setRandomColors();
      delay(random(0, 2000));
    }
  } else {
    analogWrite(ledOne[0], color[0]);
    analogWrite(ledOne[1], color[1]);
    analogWrite(ledOne[2], color[2]);

    analogWrite(ledTwo[0], color[0]);
    analogWrite(ledTwo[1], color[1]);
    analogWrite(ledTwo[2], color[2]);
  }

  server.handleClient();                    // Listen for HTTP requests from clients
}

void aleatoryMode() {
  isRandom = !isRandom;
}

void getColor() {

  String red = String(color[0]);
  String green = String(color[1]);
  String blue = String(color[2]);

  server.send(200, "application/json", "{\"message\": \""+ red + "," + green + "," + blue +"\" }");
}

void handleColor() {  

  String newColor = server.arg("newColor");
  int *arrColor = colorStringToArray(newColor);

  color[0] = arrColor[0];
  color[1] = arrColor[1];
  color[2] = arrColor[2];
  
  analogWrite(ledOne[0], color[0]);
  analogWrite(ledOne[1], color[1]);
  analogWrite(ledOne[2], color[2]);

  analogWrite(ledTwo[0], color[0]);
  analogWrite(ledTwo[1], color[1]);
  analogWrite(ledTwo[2], color[2]);

  server.send(200, "application/json", "{\"message\": \"color setted\" }");
}

void setRandomColors() {
  red = random(0, 255);
  green = random(0, 255);
  blue = random(0, 255);
  analogWrite(ledOne[0], red);
  analogWrite(ledOne[1], green);
  analogWrite(ledOne[2], blue);

  analogWrite(ledTwo[0], red);
  analogWrite(ledTwo[1], green);
  analogWrite(ledTwo[2], blue);
}

int *colorStringToArray(String colorString) {
  
  int primeiraVirgula = colorString.indexOf(",");
  int segundaVirgula = colorString.lastIndexOf(",");

  String r = colorString.substring(0, primeiraVirgula);
  String g = colorString.substring((primeiraVirgula + 1), segundaVirgula);
  String b = colorString.substring((segundaVirgula + 1), colorString.length());
  int arrColor[3] = { r.toInt(), g.toInt(), b.toInt() };
  return arrColor;
}

void handleRoot() {
  server.send(200, "application/json", "{\"message\": \"teste\"}");   // Send HTTP status 200 (Ok) and send some text to the browser/client
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}