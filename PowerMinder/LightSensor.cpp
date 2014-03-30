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

#include <Arduino.h>
#include "LightSensor.h"

using namespace PowerMinder;

LightSensor_t::LightSensor_t(unsigned char pin)
  : m_pin(pin)
{
  init();
}


LightSensor_t::~LightSensor_t()
{
}


void
LightSensor_t::init()
{
  pinMode(m_pin, INPUT);

  // Calibrate the ambient light level
  uint32_t sum = 0;
  int n = 0;
  while (n++ < 40) {
    delayMicroseconds(4);
    sum += analogRead(m_pin);
  }
  m_baseline = sum / n;
}


uint16_t
LightSensor_t::current()
{
  return analogRead(m_pin);
}


uint16_t
LightSensor_t::baseline()
{
  return m_baseline;
}
