#include <Arduino.h>
#include <Wire.h>
#include <SSD1306.h>
#include <credentials.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ESP8266mDNS.h>

#include <Ticker.h>
#include <font.h>

SSD1306  display(0x3C, D2, D1);

// Running a web server
ESP8266WebServer server;

Ticker timer;

// Adding a websocket to the server
WebSocketsServer webSocket = WebSocketsServer(81);

// PROGMEM werkt niet. :( 
static char webpage[] = R"=====(
<!doctype html>
<html lang="en">
  <head>
    <!-- Required meta tags -->
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

    <!-- Bootstrap CSS -->
    <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/css/bootstrap.min.css" integrity="sha384-MCw98/SFnGE8fJT3GXwEOngsV7Zt27NXFoaoApmYm81iuXoPkFOJwJ8ERdknLPMO" crossorigin="anonymous">

    <title>Wemos</title>
    <style>
      .rotary {
        font-size: 3em;
      }
    </style>
  </head>
<body onload="javascript:init()">
<div class="container">
<h1>Wemos websocket rotary control</h1>
<p class="rotary" id="rotary">
</p>
</dvi>
<script>
  var webSocket;

  function init() {
    webSocket = new WebSocket('ws://' + window.location.hostname + ':81');
    webSocket.onmessage = function(event) {
      var data = JSON.parse(event.data);
      document.getElementById("rotary").innerHTML = data.value;
    }
  }
</script>
</body>
</html>
)=====";

#define ROTARYPA D5
#define ROTARYPB D6

long rotaryCount = 0;
long lastRotaryCount = 0;

void rotaryTurn();
void getData();

void setup() {     

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.begin(115200);
  while(WiFi.status()!=WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  display.init();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(64, 0, WiFi.localIP().toString());
  display.display();

  display.setFont(Dialog_plain_48);

  server.on("/",[](){
    server.send_P(200, "text/html", webpage);
  });
  server.begin();

  webSocket.begin();
  //webSocket.onEvent(webSocketEvent);

  pinMode(ROTARYPA, INPUT_PULLUP);
  pinMode(ROTARYPB, INPUT_PULLUP);  
  
  digitalWrite (ROTARYPA, HIGH);   // enable pull-up
  digitalWrite (ROTARYPB, HIGH);   // enable pull-up

  attachInterrupt (ROTARYPA, rotaryTurn, RISING);

  timer.attach(5, getData);

  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin(MDNS_NAME)) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

  // Start TCP (HTTP) server
  server.begin();
  Serial.println("TCP server started");

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("ws", "tcp", 81);
  
  
}

void loop() {

  if (lastRotaryCount != rotaryCount) {
    lastRotaryCount = rotaryCount;
    display.clear();
    display.drawString(64, 0, String(rotaryCount));
    display.display();
    getData();
    Serial.println(rotaryCount);
  }
  server.handleClient();
  webSocket.loop();
}

void rotaryTurn()
{
  // Interrupt Service Routine for a change to Rotary Encoder pin A
    
  if (digitalRead (ROTARYPB)) {
     rotaryCount--;   // Turn it Down!
  } else {
     rotaryCount++;   // Turn it Up!
  }

} 

void getData() {
  String json = "{\"value\":";
  json += rotaryCount;
  json += "}";
  webSocket.broadcastTXT(json.c_str(), json.length());

}
