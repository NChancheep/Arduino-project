
// Chancheep Mahacharoensuk 6288092
// Kantapong Matangkarat 6288160
// Project 1
// Ultrasonic with servo motor
//esp32

#include <ESP32Servo.h>

#define trigPin 23 
#define echoPin 22 

#define red 18
#define green 19
#define blue 21
//
#define buttonPin 5 //interupt on arudino board is on 2
//int button_interrupt = 2; // Interrupt 0 is on pin 2 !!
bool toggleState = false;


Servo servo_att;
float duration, distance;

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
void setup() {
  // begin serial port
  
  Serial.begin (9600);
  servo_att.attach(15); 
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
//  attachInterrupt(buttonPin, toggleLED, CHANGE);
}

void loop() {
  Serial.println(digitalRead(buttonPin));
  toggleState = checkSwitch();
  if(toggleState){
    Serial.println("on");
    ultraCal();
    if(distance <= 10){
      servo_att.write(90);
      delay(1000);
      digitalWrite(red, HIGH);
      digitalWrite(green, LOW);
      digitalWrite(blue, LOW);
    }
   else{
    servo_att.write(0);
    delay(1000);
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
