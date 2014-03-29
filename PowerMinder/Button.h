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

  /** Class to manage a button connected to a digital pin */
  class Button_t {

  public:
    /** Is the button pressed? */
    bool is_pressed();

    /** Has the button been pressed since last time? */
    bool has_been_pressed();

    /** Has the button been released since last time? */
    bool has_been_released();

    /** Button Service loop method: Call in the main loop() routine */
    void loop();

  // Looks like Sketches don't support private constructors...
  //private:
    /** Create a button control class */
    Button_t(unsigned char pin,                     ///< Pin number controlling the LED
	     int           when_pressed = HIGH,     ///< digital value when pressed
	     int           pulled = LOW);           ///< Pull up/down
    ~Button_t();

  private:
    int  m_pin;
    bool m_previous_state;
    bool m_was_pressed;
    bool m_was_released;

    int m_HIGH;
  };

}
