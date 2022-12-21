#include <ETH.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>
#include <analogWrite.h>

#include <PubSubClient.h>
#include <Wire.h>                                                           
#include "Config.h"

/* MQTT Instance */
WiFiClient espClient;
PubSubClient client(espClient);

/* Use MQTT's IP Address */
const char* mqttServer = "192.168.0.104"; //Check IP Address of your MQTT Broker (Mosquitto) E.g., 192.168.1.1
const int mqttPort = 1883;

String hex = "";
/* Value Buffer */
char buf[100]; //Reserved for 100 bytes

// LED Pin
#define red 18
#define green 19
#define blue 21

void setup() {
  Serial.begin(115200);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  digitalWrite(red, HIGH);
  digitalWrite(green, HIGH);
  digitalWrite(blue, HIGH);

  /* (WiFi) Connection Setup */
  WiFi.mode(WIFI_STA);      // set to station mode
  WiFi.begin(ssid, pass);   // connect to an access point
  delay(5000);

  /* loop until ESP32 can sucesfully connect to the WiFi */
  Serial.printf("Connecting to %s ", ssid);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  /* connection is successful */ 
  Serial.println(" CONNECTED");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  /* MQTT Server */
  client.setServer(mqttServer, mqttPort); //mqtt server and port
  client.setCallback(callback);
  
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
      hex = messageTemp;
      Serial.println("Switch");
     String value;
    value=hex;

    char charbuf[8];
    value.toCharArray(charbuf,8);
    long int rgb=strtol(charbuf,0,16); 
    byte r=(byte)(rgb>>16);
    byte g=(byte)(rgb>>8);
    byte b=(byte)(rgb);
    
    Serial.println(r);
    Serial.println(g);
    Serial.println(b);
    digitalWrite(red, r);
    digitalWrite(green, g);
    digitalWrite(blue, b);
    analogWrite(red, r);
    analogWrite(green, g);
    analogWrite(blue, b);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("esp32 Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
