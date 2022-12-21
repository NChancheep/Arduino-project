#define POT 33
#define RED 25
#define GREEN 26
#define BLUE 27

#define PWM_CH_1 0
#define PWM_CH_2 1
#define PWM_CH_3 2

#define PWM_FREQ 15000
#define PWM_RES 8 // Resolution in bits 12

int aValue;

void setup() {
  Serial.begin(9600);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  ledcSetup(PWM_CH_1, PWM_FREQ, PWM_RES);
  ledcSetup(PWM_CH_2, PWM_FREQ, PWM_RES);
  ledcSetup(PWM_CH_3, PWM_FREQ, PWM_RES);

  ledcAttachPin(RED, PWM_CH_1);
  ledcAttachPin(GREEN, PWM_CH_2);
  ledcAttachPin(BLUE, PWM_CH_3);
}

void loop() {
  aValue = analogRead(POT); //8, 12 bits ADC
  Serial.println("aValue =" + String(aValue));
  ledcWrite(PWM_CH_1, aValue);
  delay(250);
  ledcWrite(PWM_CH_2, aValue);
  delay(250);
  ledcWrite(PWM_CH_3, aValue);
  delay(250);
}
