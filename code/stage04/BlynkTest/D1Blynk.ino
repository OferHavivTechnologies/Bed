/**************************************************************
 *  This project is defined following myBed project
 *  https://github.com/oferhaviv/Bed
 *
 * Change WiFi ssid, pass, and Blynk auth token to run :)
 * 
 * the project using:
 *    4 digital connection (2,3,4,5)
 *    1 digital for selctor (8)
 *    RTC V5
 *    slider V6 (choose timer value)
 *    button V3 (apply)
 *    debug display (V1, V2)
 *    
 **************************************************************    
 * Blynk is a platform with iOS and Android apps to control
 * Arduino, Raspberry Pi and the likes over the Internet.
 * You can easily build graphic interfaces for all your
 * projects by simply dragging and dropping widgets.
 *
 *   Downloads, docs, tutorials: http://www.blynk.cc
 *   Blynk community:            http://community.blynk.cc
 *   Social networks:            http://www.fb.com/blynkapp
 *                               http://twitter.com/blynk_app
 *
 * Blynk library is licensed under MIT license
 * This example code is in public domain.
 *
 **************************************************************
 * You need to install this for ESP8266 development:
 *   https://github.com/esp8266/Arduino
 *
 * Please be sure to select the right ESP8266 module
 * in the Tools -> Board menu! (i used WeMos D1)
 **************************************************************/

#define BLYNK_PRINT Serial  // Comment this out to disable prints and save space
#define VP_RTC_WIDGET V5    // virtual port for RTC
#define VP_TIMER_DIPLAY V2  // virtual port for timer display
#define VP_SLIDER V3        // virtual port for slider
#define VP_DEBUG_CURRENT_TIME_DISPLAY V1 // virtual port for debug display
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

//struc for time working and manipulation
struct myTime
{
  int hours;
  int minutes;
  int seconds;
};

char auth[] = "<your project token>"; // Put your Auth Token here. (see Step 3 above)
char ssid[] = "<your SSIDE>"; //WiFi SSID
char pass[] = "<Your WiFi Password>"; //WiFi pwd
myTime timer_1 = {-1,-1,-1}; // timer for Lia's side
myTime timer_2 = {-1,-1,-1}; //timer for Ofer's side
myTime unApplyTimer = {-1,-1,-1};
bool side_selector = LOW; //Low is ofer High is lia (same as pin 8)


SimpleTimer timer;
WidgetRTC rtc;

BLYNK_ATTACH_WIDGET(rtc, VP_RTC_WIDGET);
// printDigits(int digits) get min/sec and check add leading 0 in case needed - return string
String printDigits(int digits){
  String result;
  if(digits < 10)
    result = '0';
  result +=digits;
  return result;
}
// getCurrentTime() return current time in myTime format
myTime getCurrentTime()
{
  myTime result;
  time_t t = now();
  result.hours=hour(t);
  result.minutes=minute(t);
  result.seconds=second(t);
  return result;
}
// isTimer(myTime t) check if t contains legal values
bool isTimer(myTime t)
{
  if (t.hours < 0){return false;}
  return true;
}

// addMinutes(myTime t, int m) add minutes to mtTime and returm myTime object
myTime addMinutes(myTime t, int m)
{
  t.minutes +=m;
  while ((t.minutes + m) >=60)
  {
    t.hours ++;
    t.minutes -=60;
  }
  
  return t;
}
//set illegal value to time
myTime removeTimer()
{
  return {-1,-1,-1}; 
}
// showTime(myTime t) return string based time
String showTime(myTime t)
{
  String result = printDigits(t.hours) + ":" + printDigits(t.minutes) + ":" + printDigits(t.seconds);
  return result;
}
// timerEnd(myTime t) check if timer ended if yes return true
bool timerEnd(myTime t)
{
  if (!isTimer(t)){return false;}

  myTime c = getCurrentTime();
  if (t.hours == c.hours)
  { 
    if (t.minutes == c.minutes)
    {
      return true;
    }
  }
  return false;
}

// checkTimer() - this method execute every timer defined
void checkTimer()
{

  // check if timer ended
  if (timerEnd(timer_1)){Serial.println("Lia's side timer ended.");}
  if (timerEnd(timer_2)){Serial.println("Ofer's side timer ended.");}
  
  String currentTimer ="no timer";
  if (side_selector)
  {
    //lia's side
     currentTimer = showTime(timer_1);
  }else{
    //ofer's side
     currentTimer = showTime(timer_2);
  }  
  

  // show the updated time in the display
  String currentTime = showTime(getCurrentTime());

  // Send time to App
  Blynk.virtualWrite(VP_DEBUG_CURRENT_TIME_DISPLAY, currentTime);

  //Send timer to app
  Blynk.virtualWrite(VP_TIMER_DIPLAY, currentTimer);

}

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);

  while(Blynk.connect()==false)
  {}
  rtc.begin();
  
  // Display digital clock every 10 seconds
  timer.setInterval(10000L, checkTimer);
}

// Apply button hit
BLYNK_WRITE(V6)
{
  Serial.println("apply button hit");

  //put unapply value in timer
  if (side_selector)
  {
    //lia's side
     timer_1 = unApplyTimer;
  }else{
    //ofer's side
     timer_2 = unApplyTimer;
  } 
  
}

// Slider had been change keep value in temporary object (unApplyTimer)
BLYNK_WRITE(VP_SLIDER)
{
  Serial.print("slider value: " );
  Serial.println(param.asStr());
  if (param.asInt() == 0)
  { unApplyTimer = removeTimer();
  }else{
    unApplyTimer = addMinutes(getCurrentTime(),param.asInt());
  }
}
void loop()
{
  Blynk.run();
  timer.run();
}



