#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>                                                           
#include "Config.h"
#include <ESP32Servo.h>

/* MQTT Instance */
WiFiClient espClient;
PubSubClient client(espClient);

/* Use MQTT's IP Address */
const char* mqttServer = "192.168.1.22"; 
const int mqttPort = 1883;

#define LDR 33

/* Value Buffer */
char buf[100]; //Reserved for 100 bytes

Servo servoMotor;
float aValue;
bool turnOnOff;


void setup() {
   servoMotor.attach(5); 
  Serial.begin(115200);
 

  /* (WiFi) Connection Setup */
  WiFi.mode(WIFI_STA);      
  WiFi.begin(ssid, pass);   
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
  client.setServer(mqttServer, mqttPort); 
  client.setCallback(callback);
 

 
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if(!turnOnOff){
    Serial.println("System now turn off and not available");
  }
  else{
    aValue = analogRead(LDR); //12 bits ADC
  Serial.println(aValue);
  sprintf(buf, "%.2f", aValue); 
  client.publish("esp32/lumen", buf); 
  if(aValue < 1000){
    servoMotor.write(0);
    Serial.println("Low");
  }
  else{
    servoMotor.write(90);
    Serial.println("High");
  }
  }
  
 delay(1000);
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
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      turnOnOff = true;
      
    }
    else if(messageTemp == "off"){
      turnOnOff = false;
 
    }
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
  
