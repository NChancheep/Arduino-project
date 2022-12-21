// Line: fHI61X46skwJjw92LI2aEsYLa8O1jEMvMT5odB0tOcN
#include <WiFi.h>
#include <PubSubClient.h>

#include <Wire.h>                                                           
#include <Adafruit_GFX.h>
#include <Adafruit_AHTX0.h>

#include <BlynkSimpleEsp32.h>

#include "config.h"
/*WiFi*/
bool wifiConnected = true;

/* MQTT Instance */
WiFiClient espClient;
PubSubClient client(espClient);

BlynkTimer timer;

/* AHT20 */
Adafruit_AHTX0 aht;
sensors_event_t humidity, temp;

/* Value Buffer */
char buf[100]; //Reserved for 100 bytes

/* LED Pin */
#define LED_PIN 5
#define FREQ 5000
#define LED_CHANNEL_0 0
#define RES 10

void setup() {
  Serial.begin(115200);
  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT10 or AHT20 found");

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
  
  /* Blynk Setup */
  Blynk.begin(BLYNK_AUTH, ssid, pass, "blynk.cloud", 80);

  /* LED */
  pinMode(LED_PIN, OUTPUT);
  ledcSetup(LED_CHANNEL_0, FREQ, RES);
  ledcAttachPin(LED_PIN, LED_CHANNEL_0);
}

void loop() {
  Blynk.run();
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data

  /* mqtt-server Transmission */
  sprintf(buf, "%.2f", temp.temperature); //Make it text in the buffer (buf)
  Serial.println(buf);
  client.publish("esp32/temperature", buf);
  Blynk.virtualWrite(V0, temp.temperature);
  
  sprintf(buf, "%.2f", humidity.relative_humidity);
  Serial.println(buf);
  client.publish("esp32/humidity", buf);
  Blynk.virtualWrite(V1, humidity.relative_humidity);
  
  delay(2000);
}

// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin 2

BLYNK_WRITE(V2)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V2 Slider value is: ");
  Serial.println(pinValue);

  ledcWrite(LED_CHANNEL_0, pinValue);
}


/* Need to be correct on Node-RED and this function to dim LED as same as Blynk */
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  int messageVal;
  
  for (int i = 0; i < length; i++) {
    Serial.println((char)message[i]);
    messageTemp += (char)message[i];
  }
  
  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  
  messageVal = messageTemp.toInt();
  ledcWrite(LED_CHANNEL_0, messageVal);
  Serial.print("Node-RED Slider value is: ");
  Serial.println(messageVal);
  
  
    
  // Changes the output state according to the message
  //if (String(topic) == "esp32/output") {
  //  Serial.print("Changing output to ");
  //  if(messageTemp == "on"){
  //    Serial.println("on");
  //    digitalWrite(LED_PIN, HIGH);
  //  }
  //  else if(messageTemp == "off"){
  //    Serial.println("off");
  //    digitalWrite(LED_PIN, LOW);
  //  } 
  //}
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
