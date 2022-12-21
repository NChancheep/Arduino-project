#define _SYNC_NTP //Uncomment this if you want the synchronize RTC with the NTP server

#include <WiFi.h>
#include <time.h>
#include <DS3231.h>
#include <Wire.h>

//const char* ssid       = "";
//const char* password   = "";

const char* ssid       = "MyWifi2G";
const char* password   = "31133985"; 

const char* ntpServer = "th.pool.ntp.org";
//const char* ntpServer = "clock.mahidol.ac.th";
const long  gmtOffset_sec = 3600 * 7; //UTC +7.00
const int   daylightOffset_sec = 0; //0 means no DST observed; otherwise, 3600.

DS3231  rtc;

bool h12Format;
bool ampm;
bool centuryRollover;

struct tm timeinfo;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  
  #ifdef _SYNC_NTP
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println("M:" + String(timeinfo.tm_mon) + ", Y:" + String(timeinfo.tm_year));
  rtc.enableOscillator(true, true, 1);
  rtc.setClockMode(h12Format); //24-h format
  rtc.setDoW(timeinfo.tm_wday);
  rtc.setHour(timeinfo.tm_hour);
  rtc.setMinute(timeinfo.tm_min);
  rtc.setSecond(timeinfo.tm_sec);
  rtc.setDate(timeinfo.tm_mday);
  rtc.setMonth(timeinfo.tm_mon + 1); //Month from NTP starts from zero
  rtc.setYear(timeinfo.tm_year - 100); //Year from NTP is an offset from 1900

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  #endif
}

void loop()
{
  #ifdef _SYNC_NTP
  //Show time from NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println("M:" + String(timeinfo.tm_mon) + ", Y:" + String(timeinfo.tm_year));
  #endif

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
