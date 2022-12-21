//https://iotdesignpro.com/projects/how-to-send-data-to-thingspeak-cloud-using-esp32
//https://tinoiot.com/tutorials/thingspeak-with-esp-mc/
#include <WiFi.h>

#include "config.h"
#include <Wire.h>                                                           
#include <Adafruit_GFX.h>
#include <Adafruit_AHTX0.h>

/* MQTT Instance */
WiFiClient espClient;

/* (WiFi) Variables */
//const char* ssid = ""; // Your WiFi credentials.
//const char* password = ""; // Set password to "" for open networks.
bool wifiConnected = true;

Adafruit_AHTX0 aht;
sensors_event_t humidity, temp, pressure;

/* Value Buffer */
char buf[100]; //Reserved for 100 bytes

/* Thingspeak */
#include "ThingSpeak.h"
unsigned long myChannelNumber = 1;
const char * myWriteAPIKey = "FPRO10BTJYX0UC10";

// const char* server = "api.thingspeak.com";

// Your thingspeak channel url with api_key query
// String serverName = "";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;


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

  /* Initialize Thinkspeak */
  ThingSpeak.begin(espClient);  
}

void loop() 
{
  if ((millis() - lastTime) > timerDelay) 
  {
     
    // Get a new temperature and humidity reading
    aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
    Serial.print("temp: "); Serial.println(temp.temperature);

    Serial.print("humid: "); Serial.println(humidity.relative_humidity);

     Serial.print("Pressure (hPa): "); Serial.println(pressure.pressure);
    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    ThingSpeak.setField(1, temp.temperature);
    ThingSpeak.setField(2, humidity.relative_humidity);
    ThingSpeak.setField(3, pressure.pressure);
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    
    //int x = ThingSpeak.writeField(myChannelNumber, 1, temp.temperature, myWriteAPIKey);
    //int y = ThingSpeak.writeField(myChannelNumber, 2, humidity.relative_humidity, myWriteAPIKey);
    
    //uncomment if you want to get temperature in Fahrenheit
    //int x = ThingSpeak.writeField(myChannelNumber, 1, temperatureF, myWriteAPIKey);
    
    if(x == 200)
      Serial.println("Temp Humid and pressure update successful.");
    else 
      Serial.println("Problem updating Temp and Humid. HTTP error code " + String(x));
   
    lastTime = millis();
  }
}
