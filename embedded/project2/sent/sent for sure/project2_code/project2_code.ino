//Chancheep Mahacharoensuk 6288092
//Kantapong Matangkarat 6288160
#include <WiFi.h>
#include <PubSubClient.h>
#include <time.h>
#include <DS3231.h>
#include <Wire.h>                                                           
#include <BlynkSimpleEsp32.h>
BlynkTimer timer;
#include "Config.h"

#include <ESP32Servo.h>

/* MQTT Instance */
WiFiClient espClient;
PubSubClient client(espClient);

/* Use MQTT's IP Address */
const char* mqttServer = "192.168.0.102"; //Check IP Address of your MQTT Broker (Mosquitto) E.g., 192.168.1.1
const int mqttPort = 1883;



/* Value Buffer */
char buf[100]; //Reserved for 100 bytes

//for use with ultrasonic
#define trigPin 16
#define echoPin 17

//RGB led pin
#define red 18
#define green 19
#define blue 23

#define buttonPin 5 

bool toggleState = false; // for interupt state
bool turnSystem = false; // for turn system off

Servo servo_att;
float duration, distance;






const char* ntpServer = "th.pool.ntp.org";
//const char* ntpServer = "clock.mahidol.ac.th";
const long  gmtOffset_sec = 3600 * 7; //UTC +7.00
const int   daylightOffset_sec = 0; //0 means no DST observed; otherwise, 3600.

DS3231  rtc;

bool h12Format;
bool ampm;
bool centuryRollover;

struct tm timeinfo;


//function to calculate the distance of ultrasonic
void ultraCal(){
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = 0.017 * duration;
  Serial.print("distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}

// for interupt state
bool checkSwitch() {
  bool state = false;
  
  if (digitalRead(buttonPin) == LOW) {
    state = false;
  }
  else{
    state = true;
  }
  return state;
}

// callback from node red
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
      Serial.println("on");
      turnSystem = true;
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      turnSystem = false;
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
BLYNK_WRITE(V2)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  
  Serial.print("V2 value is: ");
  Serial.println(pinValue);
  if(pinValue == 0){
    turnSystem = false;
  }
  else{
    turnSystem = true;
  }
}
void setup() {
  Serial.begin(115200);
  Wire.begin();
  /* Blynk Setup */
  Blynk.begin(BLYNK_AUTH, ssid, pass, "blynk.cloud", 80);
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
  servo_att.attach(15); 

  //setup pin
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);


  
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  Blynk.run();
  client.loop();
  if(turnSystem){
    Serial.println(digitalRead(buttonPin));
    toggleState = checkSwitch();
    if(toggleState){
      Serial.println("on");
      ultraCal();
      sprintf(buf, "%.2f", distance); //Make it text in the buffer (buf)
      client.publish("esp32/distance", buf); //sent the distance to node red
      Blynk.virtualWrite(V0, distance);
      if(distance <= 10){
        servo_att.write(90);
        digitalWrite(red, HIGH);
        digitalWrite(green, LOW);
        digitalWrite(blue, LOW);
        sprintf(buf, "%.2f", distance); //Make it text in the buffer (buf)
        client.publish("esp32/closeDistance", buf); //sent the distance to mongoDB via node red
        client.publish("esp32/timeTrigger", "1"); //for trigger to sent time to store in mongoDB via node red
      }
     else{
      servo_att.write(0);
      digitalWrite(red, LOW);
      digitalWrite(green, HIGH);
      digitalWrite(blue, LOW);
     }
    }
   else{
      Serial.println("off");
      digitalWrite(red, LOW);
      digitalWrite(green,LOW);
      digitalWrite(blue, HIGH);
 }
  }
  else if(!turnSystem){
    Serial.println("The system is now turn off!");
    digitalWrite(red, LOW);
    digitalWrite(green, LOW);
    digitalWrite(blue, LOW);
  }
  else{
    Serial.println("error");
  }


   // Send Day-of-Week
  Serial.print("DoW:");
  Serial.print(rtc.getDoW());
  Serial.print(" ");
  
  // Send date
  Serial.print("-- Date: ");
  Serial.print(rtc.getDate(), DEC);
  Serial.print("/");
  Serial.print(rtc.getMonth(centuryRollover), DEC);
  Serial.print("/");
  Serial.print("2"); //This program is still valid until almost the next 1000 years.
  if(centuryRollover)
    Serial.print("1");
  else
    Serial.print("0");
  Serial.print(rtc.getYear(), DEC);

  // Send time
  Serial.print(" -- Time: ");
  Serial.print(rtc.getHour(h12Format, ampm), DEC);
  Serial.print(":");
  Serial.print(rtc.getMinute(), DEC);
  Serial.print(":");
  Serial.print(rtc.getSecond(), DEC);

  //Temperature
  Serial.print(" -- RTC Temperature: ");
  Serial.println(rtc.getTemperature());
 delay(1000);
}
  
