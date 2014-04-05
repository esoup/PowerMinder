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

  /** Class to manage the light-sensitive resistor */
  class LightSensor_t {

  public:
    /** Initialize and calibrate the sensor */
    void init();

    /** Return the current light level reading, scaled to a value in the 0-1023 range.
     *  The higher the value, the brighter it is.
     *  Scale may not be perfectly linear.
     *  It may not be possible to reach the limits of the scale.
     *
     * Experiments with unit #2 showed values of ~0x0260 for ambient/no light conditions
     * and values of ~0x02FC when lit with a white LED from 4" away.
     */
    uint16_t current();

    /** Return the baseline ambient light level as measured during calibration */
    uint16_t baseline();

  // Looks like Sketches don't support private constructors...
  //private:
    /** Create a light sensor control class */
    LightSensor_t(uint8_t pin);                ///< Analog pin number reading the light sensor
    ~LightSensor_t();

  private:
    uint8_t  m_pin;
    uint16_t m_baseline;
  };

}
