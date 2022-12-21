//AHT20 Temperature sensor demo done on 01/03/2022
//Ref: http://www.esp32learning.com/code/aht20-integrated-temperature-and-humidity-sensor-and-esp32-board-example.php
//Ref for AHT20 Fritzing: https://github.com/adafruit/Fritzing-Library/blob/master/parts/Adafruit%20AHT20%20Temperature%20and%20Humidity%20Sensor.fzpz
#include <Adafruit_AHTX0.h>
 
Adafruit_AHTX0 aht;
 
void setup() 
{
  Serial.begin(115200);
  Serial.println("Adafruit AHT10/AHT20 demo!");
  Serial.println("AHT20 Temperature sensor demo done on 01/03/2022");
  Serial.println("Ref: http://www.esp32learning.com/code/aht20-integrated-temperature-and-humidity-sensor-and-esp32-board-example.php");
 
  if (! aht.begin()) 
  {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT10 or AHT20 found");
}
 
void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  Serial.print("Temperature: "); 
  Serial.print(temp.temperature); 
  Serial.println(" degrees C");
  Serial.print("Humidity: "); 
  Serial.print(humidity.relative_humidity); 
  Serial.println("% rH");
 
  delay(500);
}
