// Chancheep Mahacharoensuk 6288092
// Kantapong Matangkarat 6288160
// Project 1
// Ultrasonic with servo motor
#include <Servo.h>
#include <RTClib.h>
#include <Wire.h>

Servo servo_att;
int trigPin = 6;
int echoPin = 7;
long distance;
long duration;

int red = 3;
int green = 4;
int blue = 5;

int buttonPin = 2; //interupt on arudino board is on 2
bool toggleState = false;

RTC_DS3231 rtc;
char t[32];

void setup() 
{
 servo_att.attach(8); 
 Serial.begin(9600);
 pinMode(red, OUTPUT);
 pinMode(green, OUTPUT);
 pinMode(blue, OUTPUT);
 pinMode(trigPin, OUTPUT);
 pinMode(echoPin, INPUT);
 pinMode(buttonPin, INPUT_PULLUP);

 Wire.begin();
 rtc.begin();
 rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));

 attachInterrupt(digitalPinToInterrupt(buttonPin),checkSwitch, FALLING);
}
 
void loop() {
  toggleState = checkSwitch();
  if(toggleState){
     ultra();
     servo_att.write(0);
     led_bright();
  }
  else{
    digitalWrite(red, LOW);
    digitalWrite(green,LOW);
    digitalWrite(blue, HIGH);
    
  }

}
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

void led_bright(){
  if(distance <= 10){
    servo_att.write(90);
    digitalWrite(red, HIGH);
    digitalWrite(green, LOW);
    digitalWrite(blue, LOW);
    DateTime now = rtc.now();
    sprintf(t, "%02d:%02d:%02d %02d/%02d/%02d", now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());  
    Serial.print(F("Date/Time: "));
    Serial.println(t);
  }
  else{
    servo_att.write(0);
    digitalWrite(red, LOW);
    digitalWrite(green, HIGH);
    digitalWrite(blue, LOW);
  }
}
void ultra(){
  digitalWrite(trigPin, LOW);
  digitalWrite(trigPin, HIGH);
  digitalWrite(trigPin, LOW);
  distance = (pulseIn(echoPin, HIGH))*0.034/2;
  Serial.print(distance);
  Serial.println(" :cm");
  }
