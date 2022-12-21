#define _SYNC_NTP //Uncomment this if you want the synchronize RTC with the NTP server
#define TFT_GREY 0x5AEB

#include <SPI.h>
#include <TFT_eSPI.h>


#include <Wire.h>
#include <DS3231.h>

#include <WiFi.h>
#include <time.h>

const char* ssid       = "YukiAir";
const char* password   = "41144987";   

const char* ntpServer = "th.pool.ntp.org";
//const char* ntpServer = "clock.mahidol.ac.th";
const long  gmtOffset_sec = 3600 * 7; //UTC +7.00
const int   daylightOffset_sec = 0; //0 means no DST observed; otherwise, 3600.

DS3231  rtc;
TFT_eSPI tft = TFT_eSPI();

uint32_t targetTime = 0;                    // for next 1 second timeout

static uint8_t conv2d(const char* p); // Forward declaration needed for IDE 1.6.x

uint8_t hh = conv2d(__TIME__), mm = conv2d(__TIME__ + 3), ss = conv2d(__TIME__ + 6); // Get H, M, S from compile time

byte omm = 99, oss = 99;
byte xcolon = 0, xsecs = 0;
unsigned int colour = 0;

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

  // init TFT 
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);

  targetTime = millis() + 1000;
  
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
  rtc.setYear(timeinfo.tm_year - 543); //Year from NTP is an offset from 1900

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

  if (targetTime < millis()) {
    // Set next update for 1 second later
    targetTime = millis() + 1000;

    // Adjust the time values by adding 1 second
    hh = rtc.getHour(h12Format, ampm);
    mm = rtc.getMinute();
    ss = rtc.getSecond();
    
    // Update digital time
    int xpos = 0;
    int ypos = 85; // Top left corner ot clock text, about half way down
    int ysecs = ypos + 24;

    if (omm != mm) { // Redraw hours and minutes time every minute
      omm = mm;
      // Draw hours and minutes
      if (hh < 10) xpos += tft.drawChar('0', xpos, ypos, 8); // Add hours leading zero for 24 hr clock
      xpos += tft.drawNumber(hh, xpos, ypos, 8);             // Draw hours
      xcolon = xpos; // Save colon coord for later to flash on/off later
      xpos += tft.drawChar(':', xpos, ypos - 8, 8);
      if (mm < 10) xpos += tft.drawChar('0', xpos, ypos, 8); // Add minutes leading zero
      xpos += tft.drawNumber(mm, xpos, ypos, 8);             // Draw minutes
      xsecs = xpos; // Sae seconds 'x' position for later display updates
    }
    if (oss != ss) { // Redraw seconds time every second
      oss = ss;
      xpos = xsecs;

      if (ss % 2) { // Flash the colons on/off
        tft.setTextColor(0x39C4, TFT_BLACK);        // Set colour to grey to dim colon
        tft.drawChar(':', xcolon, ypos - 8, 8);     // Hour:minute colon
        xpos += tft.drawChar(':', xsecs, ysecs, 6); // Seconds colon
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);    // Set colour back to yellow
      }
      else {
        tft.drawChar(':', xcolon, ypos - 8, 8);     // Hour:minute colon
        xpos += tft.drawChar(':', xsecs, ysecs, 6); // Seconds colon
      }

      //Draw seconds
      if (ss < 10) xpos += tft.drawChar('0', xpos, ysecs, 6); // Add leading zero
      tft.drawNumber(ss, xpos, ysecs, 6);                     // Draw seconds
    }
  }

  //Temperature
  Serial.print(" -- RTC Temperature: ");
  Serial.println(rtc.getTemperature());

  delay(1000);
}

// Function to extract numbers from compile time string
static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}
