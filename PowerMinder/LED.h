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


namespace PowerMinder {

  /** Class to manage a LED connected to a digital pin */
  class LED_t {

  public:
    /** Is the LED on? */
    bool is_on();

    /** Turn the LED on (turns off blink mode) */
    void on();

    /** Turn the LED off (turns off blink mode) */
    void off();

    /** Toggle the LED (turns off blink mode) */
    void toggle();

    /** Blink the LED at the specified interval */
    void blink(unsigned int msec);    ///< Interval in milliseconds (0 == turn off blink mode)

    /** LED Service loop method: Call in the main loop() routine */
    void loop();

  // Looks like Sketches don't support private constructors...
  //private:
    /** Create a LED control class */
    LED_t(unsigned char pin,                  ///< Pin number controlling the LED
	  bool          HIGH_is_ON = true);   ///< LED is ON if output is HIGH
    ~LED_t();

  private:
    int  m_pin;
    bool m_is_on;

    int m_HIGH;
    int m_LOW;

    unsigned int  m_blink_msec;
    unsigned long m_blink_stamp;

    void m_on()
    {
      digitalWrite(m_pin, m_HIGH);
      m_is_on = true;
    }

    void m_off()
    {
      digitalWrite(m_pin, m_LOW);
      m_is_on = false;
    }

    void m_toggle()
    {
      if (m_is_on) m_off();
      else m_on();
    }
  };

}
