//------------------------------------------------------------------------------
//   Copyright 2014 Janick Bergeron
//   All Rights Reserved Worldwide
//
//   Licensed under the Apache License, Version 2.0 (the
//   "License"); you may not use this file except in
//   compliance with the License.  You may obtain a copy of
//   the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in
//   writing, software distributed under the License is
//   distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
//   CONDITIONS OF ANY KIND, either express or implied.  See
//   the License for the specific language governing
//   permissions and limitations under the License.
//------------------------------------------------------------------------------

#include <stdint.h>

namespace PowerMinder {

  /** Class to manage a LED connected to a digital pin */
  class LED_t {

  public:
    /** Initialize the LED */
    void init();

    /** Is the LED on? */
    bool is_on();

    /** Turn the LED on (turns off blink mode) */
    void on();

    /** Turn the LED off (turns off blink mode) */
    void off();

    /** Toggle the LED (turns off blink mode) */
    void toggle();

    /** Blink the LED at the specified on/off interval */
    void blink(uint16_t msec_on,       ///< ON Interval in milliseconds (0 == turn off blink mode)
	       uint16_t msec_off = 0); ///< OFF Interval in milliseconds (0 == same as ON interval)

    /** LED Service loop method: Call in the main loop() routine */
    void loop();

  // Looks like Sketches don't support private constructors...
  //private:
    /** Create a LED control class */
    LED_t(uint8_t pin,               ///< Pin number controlling the LED
	  uint8_t turn_on = HIGH);   ///< Digital level to turn LED ON
    ~LED_t();

  private:
    uint8_t m_pin;
    bool    m_is_on;

    uint8_t m_ON;
    uint8_t m_OFF;

    uint16_t      m_msec_on;
    uint16_t      m_msec_off;
    unsigned long m_blink_stamp;

    void m_on()
    {
      digitalWrite(m_pin, m_ON);
      m_is_on = true;
    }

    void m_off()
    {
      digitalWrite(m_pin, m_OFF);
      m_is_on = false;
    }

    void m_toggle()
    {
      if (m_is_on) m_off();
      else m_on();
    }
  };

}
