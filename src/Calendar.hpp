
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


  /** Class to manage rate period schedules and calendars */
  class Calendar {
  public:
    Calendar();
    ~Calendar();

    /** Define a new user schedule.
     *  User-defined schedule #0 must be defined for the Calendar to use
     *  the user-defined schedules instead of the default ones.
     *  Returns TRUE if succesful.
     */
    bool defineSchedule(unsigned char id,                ///< The schedule ID. Must be 0-31
			period_t      cost_at_00_00);    ///< Cost period at 00:00

    /** Add a period change time to a previsouly-defined scheduled.
     *  Returns TRUE if succesful.
     *
     *  Time changes must be specified in chronological order and must be at least 30 mins apart.
     *  Specify only as many time changes as required.
     */
    bool addPeriod(unsigned char id,     ///< The schedule ID. Must be 0-31
		   unsigned char hrs,    ///< Hours of next period change 0..23
		   unsigned char mins,   ///< Minutes of next period change 0..59
		   period_t      cost);  ///< Cost period at HH:MM #1

    /** Delete ALL user schedules. The default ones will be used. */
    bool deleteSchedules();

    /** Define a new user season.
     *  User-defined season #0 must be defined for the Calendar to use
     *  the user-defined seasons instead of the default ones.
     *  Returns TRUE if succesful.
     *
     *  Seasons must have consecutive ID numbers and must be specified in chronological order
     *
     *  To define a holiday, insert a 1-day season with an appropriate workday schedule ID.
     */
    bool defineSeason(unsigned char id,                 ///< The season ID. Must be 0-63
		      unsigned char month,              ///< The start month 1..12
		      unsigned char day,                ///< The start day 1..30
		      unsigned char workdayScheduleId,  ///< The schedule ID for workdays (M-F)
		      unsigned char weekendScheduleId   ///< The schedule ID for weekends (S-S)
		      );


    /** Delete ALL user seasons. The default ones will be used. */
    bool deleteSeasons();

    /** Check the calendar for correctness. Returns TRUE of it is OK */
    bool check_calendar();

    /** Find the rate period information corresponding to the specified date and time.
     *  Returns TRUE if succesful.
     */
    bool findPeriod(uint8_t month,      ///< 1-12
		    uint8_t day,        ///< 1-31
		    uint8_t dayOfWeek,  ///< 1-7  (1 == Sunday)
		    uint8_t hour,       ///< 0-23
		    uint8_t min         ///< 0-59
		    );

    /** Returns the current cost period, as identified by a previous call to findPeriod */
    period_t getCurrentCost();

    /** Returns the next cost period, as identified by a previous call to findPeriod */
    period_t getNextCost();

    /** Returns the number of minutes until the next cost period (up to a maximum of 65535 mins),
     *  as identified by a previous call to findPeriod */
    uint16_t  getTimeToNextCost();


  private:
    class Implementation;
    Implementation *m_impl;

  };
  
}

