//------------------------------------------------------------------------------
//   Copyright 2013 Janick Bergeron
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


#include <Arduino.h>

#include "Button.h"

using namespace PowerMinder;

Button_t::Button_t(unsigned char pin,
		   int           when_pressed,
		   int           pulled)
  : m_pin(pin), m_previous_state(0), m_was_pressed(0), m_was_released(0), m_HIGH((when_pressed == HIGH) ? HIGH : LOW)
{
  pinMode(pin, INPUT);
  // Set the pull-up/down
  digitalWrite(pin, pulled);
  m_previous_state = (digitalRead(pin) == m_HIGH);
}


Button_t::~Button_t()
{
}


bool
Button_t::is_pressed()
{
  m_previous_state = (digitalRead(m_pin) == m_HIGH);
  m_was_pressed  = 0;
  m_was_released = 0;
  return m_previous_state;
}


bool
Button_t::has_been_pressed()
{
  loop();
  bool tmp = m_was_pressed;
  m_was_pressed  = 0;
  return tmp;
}


bool
Button_t::has_been_released()
{
  loop();
  bool tmp = m_was_released;
  m_was_released  = 0;
  return tmp;
}


void
Button_t::loop()
{
  bool curr_state = (digitalRead(m_pin) == m_HIGH);
  if (m_previous_state && !curr_state) m_was_released = true;
  if (!m_previous_state && curr_state) m_was_pressed  = true;
  m_previous_state = curr_state;
}
