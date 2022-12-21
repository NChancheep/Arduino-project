#include <Arduino.h>
#include <ESP32Servo.h>
Servo servo;
void setup ()
{
servo.attach(23);//PWM Pin settings , And GPIO The pin number corresponds to .
}
void loop ()
{
// To 0°
servo.write(0);
delay(100);
// To 45°
servo.write(45);
delay(100);
// To 90°
servo.write(90);
delay(100);
// To 135°
servo.write(135);
delay(180);
// To 0°
servo.write(180);
delay(100);
}
