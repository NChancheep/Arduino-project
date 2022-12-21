#include <WiFi.h>
#include <PubSubClient.h>

#include <Adafruit_AHTX0.h>

#include "config.h"

#define LED 4
#define FREQ 5000
#define LED_CH0 0
#define LED_RES 8

/* MQTT Instance */
WiFiClient espClient;
PubSubClient client(espClient);

bool wifiConnected = true;

Adafruit_AHTX0 aht;
sensors_event_t humidity, temp;



/* Value Buffer */
char buf[200]; //Reserved for 200 bytes
long now, lastMsg;
String output;

void setup() {
  Serial.begin(115200);

  ledcSetup(LED_CH0, FREQ, LED_RES);
  ledcAttachPin(LED, LED_CH0);
  
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

  /* MQTT NetPie Server */
  client.setServer(mqttServer, mqttPort); //NetPie server and port
  client.setCallback(callback);
}

void loop() {
  now = millis(); //Milliseconds now (timestamp)
  if(now - lastMsg > 5000){ //Publish new messages to the broker again when 5s passes; otherwise, let it handle subscribed messages with little blocking
    lastMsg = now;
    if(!client.connected())
      netpieReconnect();
    client.loop();
    aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  
    /* NetPie Transmission */
    //sprintf(buf, "{\"data\":{\"temperature\":%.2f, \"humidity\":%.2f}}", temp.temperature, humidity.relative_humidity);
    //Serial.println(buf);
    output = "{\"data\": {\"temperature\":" + String(temp.temperature) + ", \"humidity\":" + String(humidity.relative_humidity) + ",\"place\":\"Timmy\"}}";
    Serial.println(output);
    output.toCharArray(buf, (output.length() + 1));
    client.publish("@shadow/data/update", buf);
    delay(1);
  }
}

void netpieReconnect(){
  while(!client.connected()){
    Serial.println("Connecting to NetPie...");
    if (client.connect(mqttClient, mqttUser, mqttPassword )) {
      Serial.println("connected");
      client.subscribe("@msg/led");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length){
  char msg[length+1];
  memcpy(msg, payload, length);
  msg[length] = '\0';
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.printf(": %s]\n", msg);
  int val = String(msg).toInt();
  int written;
  
  if(val <= 0)
    written = 0;
  else if(val >= 255)
    written = 255;
  else
    written = val;
  ledcWrite(LED_CH0, written);

  sprintf(buf, "{\"data\":{\"led\":%d}}", written);
  Serial.println(buf);
  client.publish("@shadow/data/update", buf); //Feedback and record the updated value to NetPie
}
