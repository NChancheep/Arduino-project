#include <PID.h> //ArcPID library

#define LED 32 //Output
#define LDR 33 //Input sensor
#define POT 34 //This potentiometer is to set PID target

#define PWM_CH_1 0
#define PWM_FREQ 15000
#define PWM_RES 10 // Resolution in bits

#define MAX 1023.0 //10-bit max value

arc::PID<double> ledPid(5,4,3); //Kp, Ki, Kd


void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(LDR, INPUT);
  pinMode(POT, INPUT);

  ledcSetup(PWM_CH_1, PWM_FREQ, PWM_RES);
  ledcAttachPin(LED, PWM_CH_1);
  analogReadResolution(PWM_RES); //Set analog input resolusion to be the same as PWM
}

unsigned int ledValue = 0;

void loop() {
  delay(100);
  unsigned short ldrRaw = analogRead(LDR);
  unsigned short potRaw = analogRead(POT);
  ledPid.setTarget(potRaw);
  ledPid.setInput(ldrRaw);
  Serial.print(",\"raw led\":");
  Serial.print(ledValue,DEC); 
  ledValue = min(MAX, max(0.0,ledValue + ledPid.getOutput()));
  Serial.print("{\"ldr\":");
  Serial.print(ldrRaw,DEC);
  Serial.print(",\"pot\":");
  Serial.print(potRaw,DEC); 
  Serial.print(",\"adjusted led\":");
  Serial.print(ledValue,DEC); 
  Serial.println("}");
  ledcWrite(PWM_CH_1, ledValue);
}
