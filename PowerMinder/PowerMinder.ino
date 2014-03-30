// Main program file for PowerMinder
// By Ian McCutcheon
// see github esoup https://github.com/esoup/PowerMinder
// December 22 2013
// This will become the main code - today it only holds
// code to prove the hardware functions.
// Can we get lights to flash?
// Next changes code to 
#include <stdlib.h> // for malloc and free
#include <stddef.h>
//void* operator new(size_t size) { return malloc(size); }
//void operator delete(void* ptr) { free(ptr); }

#include <Arduino.h>
#include "rtc.h"
#include "LED.h"
#include "Button.h"
//#include "Calendar.h"

using namespace PowerMinder;


//
// Hardware Resources
//
namespace LED {
  LED_t red(0);
  LED_t yellow(1);
  LED_t green(5);

  void init()
  {
    red.init();
    yellow.init();
    green.init();
  }

  void loop()
  {
    red.loop();
    yellow.loop();
    green.loop();
  }
}

// Cannot use pin-change interrupts for the button because it is VERY bouncy!
// This Button class does software debouncing for reliable button sensing.
Button_t button(4, HIGH, HIGH);


#define LIGHT 3     // light sensitive resistor pin


// Misc testing variables for testing hardware elements
int     counter = 1;
int     light = 0;
int     AMBIENT = 0;
unsigned long t= 0;


void set_time()
{
  // Flash all three LEDs until the button is released then pressed again
  LED::red.blink(50, 950);
  LED::yellow.blink(50, 950);
  LED::green.blink(50, 950);
  while (!button.has_been_released()) {
    LED::loop();
    delay(50);
  }
  while (!button.has_been_pressed()) {
    LED::loop();
    delay(50);
  }
  LED::red.off();
  LED::yellow.off();
  LED::green.off();

#ifdef NOT YET
  //some code here directly from rtc sample (see rtc.h or link)
  // http://playground.arduino.cc//Main/DS1302?action=sourceblock&num=1
  
  // Build a time structure
  ds1302_struct rtc;


  // Fill these variables with the date and time.
  int seconds, minutes, hours, dayofweek, dayofmonth, month, year;

  // Example for april 15, 2013, 10:08, monday is 2nd day of Week.
  // Set your own time and date in these variables.
  seconds    = 0;
  minutes    = 10;
  hours      = 13;
  dayofweek  = 5;  // Day of week, any day can be first, counts 1...7
  dayofmonth = 29; // Day of month, 1...31
  month      = 11;  // month 1...12
  year       = 2013;

  // Set a time and date
  // This also clears the CH (Clock Halt) bit, 
  // to start the clock.

  // Fill the structure with zeros to make 
  // any unused bits zero
  memset ((char *) &rtc, 0, sizeof(rtc));

  rtc.Seconds    = bin2bcd_l( seconds);
  rtc.Seconds10  = bin2bcd_h( seconds);
  rtc.CH         = 0;      // 1 for Clock Halt, 0 to run;
  rtc.Minutes    = bin2bcd_l( minutes);
  rtc.Minutes10  = bin2bcd_h( minutes);
  // To use the 12 hour format,
  // use it like these four lines:
  //    rtc.h12.Hour   = bin2bcd_l( hours);
  //    rtc.h12.Hour10 = bin2bcd_h( hours);
  //    rtc.h12.AM_PM  = 0;     // AM = 0
  //    rtc.h12.hour_12_24 = 1; // 1 for 24 hour format
  rtc.h24.Hour   = bin2bcd_l( hours);
  rtc.h24.Hour10 = bin2bcd_h( hours);
  rtc.h24.hour_12_24 = 0; // 0 for 24 hour format
  rtc.Date       = bin2bcd_l( dayofmonth);
  rtc.Date10     = bin2bcd_h( dayofmonth);
  rtc.Month      = bin2bcd_l( month);
  rtc.Month10    = bin2bcd_h( month);
  rtc.Day        = dayofweek;
  rtc.Year       = bin2bcd_l( year - 2000);
  rtc.Year10     = bin2bcd_h( year - 2000);
  rtc.WP = 0;  

  // Write all clock data at once (burst mode).
  DS1302_clock_burst_write( (uint8_t *) &rtc);  
#endif

}


void setup()
{   
  LED::init();
  button.init();

  // Start all (most) of the pins as outputs and let the rest of the code switch modes when needed.
  // These are digispark pns / attiny85 pins
  pinMode(2, OUTPUT);    digitalWrite(2, LOW);
  pinMode(3, OUTPUT);    digitalWrite(3, LOW);


  // Start by clearing the Write Protect bit in the RTC
  // Otherwise the clock data cannot be written
  DS1302_write (DS1302_ENABLE, 0);
  
  // Disable Trickle Charger.
  DS1302_write (DS1302_TRICKLE, 0x00);

  //
  // Go into programming mode if the button is pressed for at least 3 seconds at boot time
  //
  if (button.is_pressed()) {
    // OK, it's pressed now...
    LED::yellow.blink(100, 400);
    unsigned long now = millis();
    while (millis() - now < 3000) {
      LED::loop();
      button.loop();
    }
    LED::yellow.off();
    // Has it been released in the last 3 seconds?
    if (!button.has_been_released()) {
      // Wait for it to be released
      while (button.is_pressed()) LED::loop();
      set_time();
    }
  }

// This is one way to look at the light sensor (see the function below).
AMBIENT = GetAmbient();

// Let's just brute force the GREEN LED on and off so we know our code has made it this far.
  for (int x=0; x < 3; x++) {
    LED::green.on();
    delay(50);
    LED::green.off();
    delay(450);  

    LED::yellow.on();
    delay(50);
    LED::yellow.off();
    delay(450);  

    LED::red.on();
    delay(50);
    LED::red.off();
    delay(450);  
  }

  // Set up the red LED to blink every second
  LED::red.blink(50, 950);
}

void loop()
{
  // local variable to set the clock - not used in production version where program mode is used instead.
  //ds1302_struct rtc;
  //char buffer[80];     // the code uses 70 characters.

  // Read all clock data at once (burst mode).
  //XXX DS1302_clock_burst_read( (uint8_t *) &rtc);
  //pinMode(YELLOW, INPUT); 
  //delayMicroseconds( 4); 
  //digitalWrite(YELLOW, HIGH);

  //bcd2bin( rtc.h24.Hour10, rtc.h24.Hour);
  //bcd2bin( rtc.Minutes10, rtc.Minutes);
  //bcd2bin( rtc.Seconds10, rtc.Seconds);
  //bcd2bin( rtc.Date10, rtc.Date);
  //bcd2bin( rtc.Month10, rtc.Month);
  //bcd2bin( rtc.Year10, rtc.Year);  

  //pinMode(LIGHT, INPUT); 
  //delayMicroseconds( 4);
  //light = analogRead(LIGHT);

  LED::loop();
  button.loop();

  if (button.has_been_pressed() ) LED::yellow.toggle();
}

//  One important light sensor function will be to establish the ambient light / average
//  reading.  This GetAmbient() function enables a dynamic threshold to be found and used
//  when attempting to sens light pulses or changes.
//
//  This project will build a class to use the light sensor in a far more involved manner.
//  A cpp file to encapsulate the below is a TBD.

int GetAmbient() {
  pinMode(LIGHT, INPUT); 
  delayMicroseconds( 4);
  for (int x=0; x < 40; x++) {
    delayMicroseconds( 4);
    light = light + analogRead(LIGHT);
  }
  return  light / 40;
}
