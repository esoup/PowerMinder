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
//#include "Calendar.h"

using namespace PowerMinder;

namespace LED {
  LED_t red(0);
  LED_t yellow(1);
  LED_t green(5);

  void loop()
  {
    red.loop();
    yellow.loop();
    green.loop();
  }
}


#define LIGHT 3     // light sensitive resistor pin
#define IN_BUTTON 4 // Momentary button pin


// Misc testing variables for testing hardware elements
int     counter = 1;
int     light = 0;
int     AMBIENT = 0;
unsigned long t= 0;


// Service the Button interupt
bool lastButton = 0;
bool buttonPressed = false;
ISR(PCINT0_vect) {
  if (lastButton == 0 && digitalRead(IN_BUTTON)) {
    buttonPressed = true;
  }
  lastButton = digitalRead(IN_BUTTON);
}

void setup()
{   
  // This seems to work on the digispark - not on some other boards - missing board specific includes?
  GIMSK = _BV(PCIE);    // Enable pin change interrupt
  PCMSK = _BV(PCINT4);  // Enable the interrupt for only pin 4.

  int x = 0; // a counter

  // Start all (most) of the pins as outputs and let the rest of the code switch modes when needed.
  // These are digispark pns / attiny85 pins
  pinMode(2, OUTPUT);    digitalWrite(2, LOW);
  pinMode(3, OUTPUT);    digitalWrite(3, LOW);
  pinMode(IN_BUTTON, INPUT);    digitalWrite(IN_BUTTON, HIGH); //Set button pin to input and HIGH sets pull up resistor.

  // pause just to pause
  delay(500);

  //some code here directly from rtc sample (see rtc.h or link)
  // http://playground.arduino.cc//Main/DS1302?action=sourceblock&num=1
  
  // Build a time structure
  ds1302_struct rtc;


  // Start by clearing the Write Protect bit
  // Otherwise the clock data cannot be written
  // The whole register is written, 
  // but the WP-bit is the only bit in that register.
  DS1302_write (DS1302_ENABLE, 0);
  
  // Disable Trickle Charger.
  DS1302_write (DS1302_TRICKLE, 0x00);

  // Remove the next define, 
  // after the right date and time are set.
#define SET_DATE_TIME_JUST_ONCE
#ifdef SET_DATE_TIME_JUST_ONCE  

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

// This is one way to look at the light sensor (see the function below).
AMBIENT = GetAmbient();

// Let's just brute force the GREEN LED on and off so we know our code has made it this far.
  for (int x=0; x < 4; x++) {
    LED::green.on();
    delay( 150);
    LED::green.off();
    delay( 150);  

    LED::yellow.on();
    delay( 150);
    LED::yellow.off();
    delay( 150);  

    LED::red.on();
    delay( 150);
    LED::red.off();
    delay( 150);  
  }
  delay(250);

// Set up the red LED to blinkk every 1/2 seconds
 LED::red.blink(500);
}

void loop()
{
  // local variable to set the clock - not used in production version where program mode is used instead.
  ds1302_struct rtc;
  char buffer[80];     // the code uses 70 characters.

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

  pinMode(LIGHT, INPUT); 
  delayMicroseconds( 4);
  light = analogRead(LIGHT);

  if (buttonPressed) {
    LED::yellow.toggle();
    buttonPressed = false;
  }

  LED::loop();

  // Again delay = bad but let's just flash the green LED to be sure things are running
  LED::green.toggle();
  delay(250);
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
