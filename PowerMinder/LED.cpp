
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

LED_t::LED_t(uint8_t pin,
	     uint8_t turn_on)
  : m_pin(pin), m_is_on(0), m_ON(turn_on), m_OFF(turn_on == HIGH ? LOW : HIGH),
    m_msec_on(0), m_msec_off(0), m_blink_stamp(0)
{
  init();
}


LED_t::~LED_t()
{
}


void
LED_t::init()
{
  pinMode(m_pin, OUTPUT);
  off();
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
  m_msec_on = 0;
}


void
LED_t::off()
{
  m_off();
  m_msec_on = 0;
}


void
LED_t::toggle()
{
  m_toggle();
  m_msec_on = 0;
}


void
LED_t::blink(uint16_t msec_on,
	     uint16_t msec_off)
{
  m_msec_on  = msec_on;
  m_msec_off = (msec_off == 0) ? msec_on : msec_off;
  if (msec_on == 0) return;

  m_blink_stamp = millis();
  m_off();
}


void
LED_t::loop()
{
  // Is blink mode ON?
  if (m_msec_on > 0) {
    unsigned long now = millis();
    // If the clock has wrapped around, consider the interval over.
    // This will cause a glitch once every 50 days so acceptable
    if (now < m_blink_stamp
	|| (now - m_blink_stamp) > ((m_is_on) ? m_msec_on : m_msec_off)) {
      m_toggle();
      m_blink_stamp = now;
    }
  }
}
