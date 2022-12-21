//Complete this file according to the FSM
#define STATE1 1
#define STATE2 2
#define STATE3 3
#define STATE4 4
#define STATE_END 100

int sensor1 = 13;
int sensor2 = 14;
int led1 = 4;
int led2 = 2;

unsigned char state=4;

void setup() {
  pinMode (sensor1, INPUT);
  pinMode (sensor2, INPUT);
  pinMode (led1, OUTPUT);
  pinMode (led2, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  Serial.println(state);
  Serial.println(digitalRead(sensor1));
  Serial.println(digitalRead(sensor2));
  switch(state) {
    case STATE1:
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      if((digitalRead(sensor1)==LOW) && (digitalRead(sensor2)==HIGH))
        state = STATE2;
      else if((digitalRead(sensor1)==HIGH) && (digitalRead(sensor2)==LOW))
        state = STATE3;
      else if((digitalRead(sensor1)==HIGH) && (digitalRead(sensor2)==HIGH))
        state = STATE4;
      break;
     case STATE2:
       digitalWrite(led1, HIGH);
       digitalWrite(led2, LOW);
       if((digitalRead(sensor1)==HIGH) && (digitalRead(sensor2)==LOW))
          state = STATE3;
       else if((digitalRead(sensor1)==LOW) && (digitalRead(sensor2)==LOW))
          state = STATE1;
       else if((digitalRead(sensor1)==HIGH) && (digitalRead(sensor2)==HIGH))
          state = STATE4;
       break;
      case STATE3:
       digitalWrite(led1, LOW);
       digitalWrite(led2, HIGH);
       if((digitalRead(sensor1)==LOW) && (digitalRead(sensor2)==HIGH))
         state = STATE2;
       else if((digitalRead(sensor1)==LOW) && (digitalRead(sensor2)==LOW))
         state = STATE1;
       else if((digitalRead(sensor1)==HIGH) && (digitalRead(sensor2)==HIGH))
         state = STATE4;
        break;
        

         case STATE4:
       digitalWrite(led1, LOW);
       digitalWrite(led2, LOW);
       if((digitalRead(sensor1)==HIGH) && (digitalRead(sensor2)==LOW))
         state = STATE3;
       else if((digitalRead(sensor1)==LOW) && (digitalRead(sensor2)==LOW))
         state = STATE1;
       else if((digitalRead(sensor1)==LOW) && (digitalRead(sensor2)==HIGH))
         state = STATE2;
        break;

      
    //to be continued 
  }
  delay(100);
}
