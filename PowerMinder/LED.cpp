
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

#include "LED.h"

using namespace PowerMinder;

LED_t::LED_t(unsigned char pin,
	     bool        HIGH_is_ON)
  : m_pin(pin), m_is_on(0), m_HIGH(HIGH_is_ON ? HIGH : LOW), m_LOW(HIGH_is_ON ? LOW : HIGH),
    m_blink_msec(0), m_blink_stamp(0)
{
  pinMode(pin, OUTPUT);
  m_off();
}


LED_t::~LED_t()
{
}


bool
LED_t::is_on()
{
  return m_is_on;
}


void
LED_t::on()
{
  m_on();
  m_blink_msec = 0;
}


void
LED_t::off()
{
  m_off();
  m_blink_msec = 0;
}


void
LED_t::toggle()
{
  m_toggle();
  m_blink_msec = 0;
}


void
LED_t::blink(unsigned int msec)
{
  m_blink_msec = msec;
  if (msec == 0) {
    m_off();
    return;
  }
  m_blink_stamp = millis();
}


void
LED_t::loop()
{
  // Is blink mode ON?
  if (m_blink_msec > 0) {
    unsigned long now = millis();
    // If the clock has wrapped around, consider the interval over.
    // This will cause a glitch once every 50 days so acceptable
    if (now < m_blink_stamp
	|| (now - m_blink_stamp) > m_blink_msec) {
      m_toggle();
      m_blink_stamp = now;
    }
  }
}
