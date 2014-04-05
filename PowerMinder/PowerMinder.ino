// Main program file for PowerMinder
// By Ian McCutcheon
// see github esoup https://github.com/esoup/PowerMinder
// December 22 2013
// This will become the main code - today it only holds
// code to prove the hardware functions.
// Can we get lights to flash?
// Next changes code to 

#include <stdint.h>
#include <stdlib.h> // for malloc and free
#include <stddef.h>

#include <Arduino.h>
#include "rtc.h"
#include "LED.h"
#include "Button.h"
#include "LightSensor.h"
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

// This Button class does software debouncing for reliable button sensing.
Button_t button(4, HIGH, HIGH);

LightSensor_t light(3);


//
// Programming Mode
//
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

  LED::red.off();
  LED::yellow.off();
  LED::green.off();

}


//
// Interrupt service routine
//
ISR(PCINT0_vect) {
   button.loop();
}

void setup()
{
  LED::init();
  button.init();
  light.init();

  // Set the RTC CE pin to OUT
  pinMode(2, OUTPUT);    digitalWrite(2, LOW);

  // Enable pin-change interrupt to detect button presses
  GIMSK = _BV(PCIE);    // Enable pin change interrupt
  PCMSK = _BV(PCINT4);  // Enable the interrupt for only pin 4.

  //
  // Go into programming mode if the button is pressed for at least 3 seconds at boot time
  //
  if (button.is_pressed()) {
    // OK, it's pressed now...
    LED::yellow.blink(100, 400);
    unsigned long now = millis();
    while (millis() - now < 3000) {
      LED::loop();
    }
    LED::yellow.off();
    // Has it been released in the last 3 seconds?
    if (!button.has_been_released()) {
      // Wait for it to be released
      while (button.is_pressed()) LED::loop();
      set_time();
    }
  }

  // Set up the green LED to blink every second
  // LED::green.blink(50, 950);

}

int strobe = 0;

void loop()
{

#undef BUTTON_TEST
#ifdef BUTTON_TEST
  LED::loop();
  
  if (button.has_been_pressed() ) LED::red.toggle();
#endif

#undef LIGHT_TEST_RAW
#ifdef LIGHT_TEST_RAW
  display(light.current());
  delay(5000);
#endif

#undef LIGHT_TEST
#ifdef LIGHT_TEST
  // Turn on 1, 2 or 3 LEDs according to the current light level
  uint16_t brightness = light.current();
  if (brightness < 0x0200) {
    LED::green.off();
    LED::yellow.off();
    LED::red.off();
  }
  else {
    LED::green.on();
    if (brightness < 0x0280) {
      LED::yellow.off();
      LED::red.off();
    }
    else {
      LED::yellow.on();
      if (brightness < 0x02F0) LED::red.off();
      else LED::red.on();
    }
  }
#endif

#define STROBE_TEST
#ifdef STROBE_TEST
  // Turn on red LED when light is detected
  // Toggle green LED every 10 pulses
  int brightness = light.current() - light.baseline();
  if (brightness > 0x0040) {
    if (!LED::red.is_on()) {
      LED::red.on();
      if (++strobe == 10) {
	strobe = 0;
	LED::green.toggle();
      }
    }
  }
  else LED::red.off();
#endif

}


/** "Display" a 16-bit integer value on the LEDs.
 *  The value is shown two bits at a time, from MSB to LSB
 *  on the red (MSB) and yellow (LSB) LEDs at 1-sec intervals.
 *  The value on the red & yellow LEDs is valid when the green LED is ON.
 *
 *  For example, the value 0xC9F0 would be blinked as:
 *
 *  Red:     *   *   * *
 *  Yellow:  *     * * *
 *  Green:   * * * * * * * *
 *            C   9   F   0
 */

void
display(uint16_t value)
{
  LED::green.off();
  LED::yellow.off();
  LED::red.off();

  uint16_t mask = 0x8000;
  while (mask) {
    if (value & mask) LED::red.on();
    mask >>= 1;
    if (value & mask) LED::yellow.on();
    mask >>= 1;
    LED::green.on();

    delay(1000);

    LED::green.off();
    LED::yellow.off();
    LED::red.off();

    delay(1000);
  }
}
