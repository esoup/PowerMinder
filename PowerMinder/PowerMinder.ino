// Main program file for PowerMinder
// By Ian McCutcheon
// see github esoup https://github.com/esoup/PowerMinder
// December 22 2013

// This will become the main code - today it only holds
// code to prove the hardware functions.

// Can we get lights to flash?

// Next changes code to 

#include <Arduino.h>
#include "rtc.h"

#define RED 0    // LED pin
#define YELLOW 1 // LED pin
#define GREEN 5  // LED pin

#define LIGHT 3     // light sensitive resistor pin
#define IN_BUTTON 4 // Momentary button pin

#define REDCOUNT1 50  // _temp_ until LED class written - time to have red LED on
#define REDCOUNT2 50  // _temp_ until LED class written - time to have red LED off


// Misc testing variables for testing hardware elements
int     counter = 1;
int     light = 0;
int     AMBIENT = 0;
unsigned long t= 0;
int     x = 0;
int     redloop = 0;
boolean flashingRed = true;


// Setup the Button interupt and flag
boolean yellowOn = false;
ISR(PCINT0_vect) {
  boolean isPinHigh = digitalRead(IN_BUTTON); 
  if (isPinHigh) {
    yellowOn = true;
  }
}

void setup()
{   
  // This seems to work on the digispark - not on some other boards - missing board specific includes?
  GIMSK = _BV(PCIE);    // Enable pin change interrupt
  PCMSK = _BV(PCINT4);  // Enable the interrupt for only pin 4.

  int x = 0; // a counter

  // Start all (most) of the pins as outputs and let the rest of the code switch modes when needed.
  // These are digispark pns / attiny85 pins
  pinMode(0, OUTPUT);    digitalWrite(0, LOW);
  pinMode(1, OUTPUT);    digitalWrite(1, LOW);
  pinMode(2, OUTPUT);    digitalWrite(2, LOW);
  pinMode(3, OUTPUT);    digitalWrite(3, LOW);
  pinMode(IN_BUTTON, OUTPUT);    digitalWrite(4, HIGH); //Set button pin to input and HIGH sets pull up resistor.
  pinMode(5, OUTPUT);    digitalWrite(5, LOW);

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
  for (x=0; x < 4; x++) {
    digitalWrite(GREEN, HIGH);
    delay( 150);
    digitalWrite(GREEN, LOW);
    delay( 150);  
  }
  delay(250);
}

void loop()
{
  // local variable to set the clock - not used in production version where program mode is used instead.
  ds1302_struct rtc;
  char buffer[80];     // the code uses 70 characters.

  // Read all clock data at once (burst mode).
  //XXX DS1302_clock_burst_read( (uint8_t *) &rtc);
  pinMode(YELLOW, INPUT); 
  delayMicroseconds( 4); 
  digitalWrite(YELLOW, HIGH);

  //bcd2bin( rtc.h24.Hour10, rtc.h24.Hour);
  //bcd2bin( rtc.Minutes10, rtc.Minutes);
  //bcd2bin( rtc.Seconds10, rtc.Seconds);
  //bcd2bin( rtc.Date10, rtc.Date);
  //bcd2bin( rtc.Month10, rtc.Month);
  //bcd2bin( rtc.Year10, rtc.Year);  

  pinMode(LIGHT, INPUT); 
  delayMicroseconds( 4);
  light = analogRead(LIGHT);

  ///////////////////////////////
  // Here is a complicated but reasonable way to flash an LED (as Ian sees it)
  // Code should drop through without any significant delays.  The RED LED is treated
  // this way in the below code.
  //
  // We don't know (yet) the state of the pin mode so we set it to what we need.
  // Recall we have overloaded the function of some pins.
  pinMode(RED, OUTPUT); 
  delayMicroseconds( 4);  // microsecons - way smaller than milliseconds.
  if (flashingRed) {
    if (millis() - t < REDCOUNT1) {
      digitalWrite(RED, HIGH); //   dothis = HIGH; delay (50);
    } 
    else {
      digitalWrite(RED, LOW); //   dothis = LOW; delay(100);
    }
    if (abs(millis() - t) > (REDCOUNT1 + REDCOUNT2)) {
      redloop++;
      t = millis();
      if (redloop  > counter) {
        flashingRed = false;
      }
    }
  }
  else {
    counter = 20; //XXX bcd2bin( rtc.Seconds10, rtc.Seconds);
    pinMode(RED, OUTPUT); 
    delayMicroseconds( 4);
    flashingRed = true;
    t = millis();
    redloop = 0;
    digitalWrite(RED, LOW); 
    delay(30);
  }

  ///////////////////////////////
  // Using delay here but testing the interupt is more the point of this code.
  if (yellowOn) {
    pinMode(YELLOW, OUTPUT); 
    delayMicroseconds( 4); 
    // for (int x=0; x < light - AMBIENT ; x++) {
    digitalWrite(YELLOW, HIGH);
    delay( 500);
    digitalWrite(YELLOW, LOW); 
    yellowOn = false;
  }

  // Again delay = bad but let's just flash the green LED to be sure things are running
  digitalWrite(GREEN, HIGH);
  delay( 10);
  digitalWrite(GREEN, LOW);
  //delay( 200);
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
  for (x=0; x < 40; x++) {
    delayMicroseconds( 4);
    light = light + analogRead(LIGHT);
  }
  return  light / 40;
}
