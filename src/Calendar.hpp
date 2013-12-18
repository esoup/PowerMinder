
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


namespace PowerMinder {

  /** Types of power consumption periods */

  typedef enum period_e {OFF_PEAK     = 0,
			 PARTIAL_PEAK = 1,
			 ON_PEAK      = 2} period_t;


  class Calendar {
  public:
    Calendar();
    ~Calendar();


    /** Find the rate period information corresponding to the specified date and time.
     *  Returns TRUE if succesful.
     */
    bool findPeriod(uint8_t month,      ///< 1-12
		    uint8_t day,        ///< 1-31
		    uint8_t dayOfWeek,  ///< 1-7  (1 == Sunday)
		    uint8_t hour,       ///< 0-23
		    uint8_t min);       ///< 0-59


  private:
    class Implementation;
    Implementation *m_impl;

  };
  
}

