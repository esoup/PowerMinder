
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


#include <stdint.h>
#ifdef DEBUG
#include <stdio.h>
#endif

#include "Calendar.hpp"

using namespace PowerMinder;
  

/** The maximum number of period change points in a daily schedule */

const unsigned int MAX_CHANGE_POINTS = 5;


/** A daily schedule, composed of MAX_CHANGE_POINTS period change points.
 *
 *  The first period change point MUST be at midnight (m_time == 0).
 *  Subsequent period change points must be in chronological order.
 *  If fewer than MAX_CHANGE_POINTS period change points are required,
 *  the daily schedule must be padded with dummy period change points with
 *  a m_time == 0.
 *
 *  Time is specified using 30-min precision, so 0 == midnight, 5 = 2:30a, and 28 == 2:00p.
 */

typedef struct schedule_s {
  struct period_change_s {
    uint8_t  m_time   : 6;  ///< Time of the period change, in 30-min intervals
    period_t m_period : 2;  ///< Type of period that starts at this time
  } m_period_change[MAX_CHANGE_POINTS];
} schedule_t;



/** A season specification.
 * 
 *  A season starts at the specified date and lasts until the start of the next season.
 * 
 *  A calendar is composed by a an array of season descriptors, in chronological order.
 *  A calendar is terminated by a season with an ID == 0xFF.
 */
typedef struct season_s {
  uint8_t m_startMonth;
  uint8_t m_startDay;
  uint8_t m_workday_schedule_idx;
  uint8_t m_holiday_schedule_idx;
} season_t;



/** Default schedule, from PG&E
 *  http://www.pge.com/en/myhome/environment/whatyoucando/electricdrivevehicles/rateoptions/index.page
 */

static const schedule_t PGE_schedules[2] = {{{{0,  OFF_PEAK}, // Weekday
					      {14, PARTIAL_PEAK},
					      {28, ON_PEAK},
					      {42, PARTIAL_PEAK},
					      {44, OFF_PEAK}}},
					    {{{0,  OFF_PEAK}, // Weekend/Holidays
					      {30, ON_PEAK},
					      {38, OFF_PEAK},
					      {0, OFF_PEAK},
					      {0, OFF_PEAK}}}};


/** Default calendar, from PG&E */

static const season_t PGE_seasons[3] = {{5, 1, 0, 1},
					{11, 1, 0, 1},
					{0, 0, 0, 0}};



/** Where the user-defined schedules are stored in NVRAM */
static schedule_t *user_schedules = /*0x0000;*/ new schedule_t[31];


/** Where the user-defined seasons are stored in NVRAM */
static season_t *user_seasons = /*0x0000;*/ new season_t[64];


/** Days in months */
static uint8_t daysInMonth[12] = {31, 28, 31, 30, 31, 30,
				  31, 31, 30, 31, 30, 31};



/** PIMPL idiom to truly hide private stuff */
struct Calendar::Implementation {

  /** Set of active schedules (default ones or user-defined) */
  const schedule_t *schedules;

  /** Active calendar (default one or user-defined) */
  const season_t *seasons;


  /** The current cost period, as found by Calendar::fidnPeriod */
  period_t m_currentCost;

  /** The next cost period, as found by Calendar::fidnPeriod */
  period_t m_nextPeriod;

  /** The number minutes until the start of the next period */
  unsigned short m_timeToNextPeriod;


  Implementation()
    : schedules(user_schedules), seasons(user_seasons)
  {
    if (schedules[0].m_period_change[0].m_time != 0) schedules = PGE_schedules;
    if (seasons[0].m_startMonth == 0) seasons = PGE_seasons;
  }

  ~Implementation()
  {}


#ifdef DEBUG  
  void printPeriodChange(int unsigned time,
			 period_t     cost)
  {
    printf("      %02d:%02d ", time / 2, time % 2 * 30);

    switch (cost) {

    case OFF_PEAK: {
      printf("OFF-PEAK");
      break;
    }

    case PARTIAL_PEAK: {
      printf("MID-PEAK");
      break;
    }

    case ON_PEAK: {
      printf("ON-PEAK");
      break;
    }

    default: {
      printf("?? (%d)", cost);
      break;
    }

    }
    printf("\n");
  }

  void printSchedule(const schedule_t *schedule)
  {
    printPeriodChange(schedule->m_period_change[0].m_time,
		      schedule->m_period_change[0].m_period);

    unsigned char i = 1;
    while (i < MAX_CHANGE_POINTS &&
	   schedule->m_period_change[i].m_time > 0) {

      printPeriodChange(schedule->m_period_change[i].m_time,
			schedule->m_period_change[i].m_period);
      i++;
    }
  }
#endif

  /** Find the index of the schedule corresponding to the specified date */
  unsigned char
  findScheduleIndex(uint8_t month,     ///< 1-12
		    uint8_t day,       ///< 1-31
		    uint8_t dayOfWeek) ///< 1-7  (1 == Sunday)
  {
    unsigned char i = 0;

    // Find the first calendar entry with a start date
    // PAST the current date. The current entry will be
    // the one before it. If we reach the end of the calendar,
    // that means there are no further entries, so the last one
    // is the current one.
    while (seasons[i].m_startMonth > 0) {
      if (seasons[i].m_startMonth > month ||
          seasons[i].m_startDay > day) {
	if (i == 0) {
	  // Calendars are circular, hence the entry previous to
	  // the first one is the last one
	  while (seasons[++i].m_startMonth != 0);
	}
	break;
      }
    }
    // “i” is now the index of the current season in the calendar

    // Find the relevant schedule for this season
    unsigned char j = seasons[i].m_workday_schedule_idx;
    if (dayOfWeek > 5) j = seasons[i].m_holiday_schedule_idx;
    
    return j;
  }


};




Calendar::Calendar()
  : m_impl(new Implementation)
{
}



Calendar::~Calendar()
{
  delete m_impl;
}



bool
Calendar::defineSchedule(unsigned char id,
			 period_t      cost_at_00_00)
{
  if (id > 31) return false;

  user_schedules[id].m_period_change[0].m_time = 0;
  user_schedules[id].m_period_change[0].m_period = cost_at_00_00;

  for (int i = 1; i < MAX_CHANGE_POINTS; i++ ) {
    user_schedules[id].m_period_change[i].m_time = 0;
  }

  if (id == 0) m_impl->schedules = user_schedules;

  return true;
}


bool
Calendar::addPeriod(unsigned char id,
		    unsigned char hrs,
		    unsigned char mins,
		    period_t      cost)
{
  if (id > 31) return false;
  if (user_schedules[id].m_period_change[0].m_time != 0) return false;
  if (hrs > 23) return false;
  if (mins > 59) return false; 

  int time = (hrs * 60 * mins + 15) / 30;
  if (hrs == 0) return false;

  // Find the next "empty" entry in the scedule
  for (int i = 1; i < MAX_CHANGE_POINTS; i++ ) {
    if (user_schedules[id].m_period_change[i].m_time == 0 ||
	user_schedules[id].m_period_change[i].m_time == time) {
      user_schedules[id].m_period_change[i].m_time = time;
      user_schedules[id].m_period_change[i].m_period = cost;
      return true;
    }

    // Not in chronological order?
    if (user_schedules[id].m_period_change[i].m_time > time) return false;
  }

  // We ran out of room!
  return false;
}


bool
Calendar::deleteSchedules()
{
  m_impl->schedules = PGE_schedules;
}


bool
Calendar::defineSeason(unsigned char id,
		       unsigned char month,
		       unsigned char day,
		       unsigned char workdayScheduleId,
		       unsigned char weekendScheduleId)
{
  if (id > 63) return false;
  if (month < 1 || month > 12) return false;
  if (day < 1 || day > daysInMonth[month-1]) return false;

  user_seasons[id].m_startMonth           = month;
  user_seasons[id].m_startDay             = day;
  user_seasons[id].m_workday_schedule_idx = workdayScheduleId;
  user_seasons[id].m_holiday_schedule_idx = weekendScheduleId;

  if (id == 0) m_impl->seasons = user_seasons;

  return true;
}


bool
Calendar::deleteSeasons()
{
  m_impl->seasons = PGE_seasons;
}


bool
Calendar::check_calendar()
{
  // ToDo
}


#ifdef DEBUG
void
Calendar::print(bool user)
{
  const schedule_t *schedules = (user) ? user_schedules : PGE_schedules;
  const season_t   *seasons   = (user) ? user_seasons   : PGE_seasons;;

  int i = 0;

  printf("Calendar:\n");
  while (seasons[i].m_startMonth > 0) {
    printf("  %02d/%02d\n", seasons[i].m_startMonth, seasons[i].m_startDay);
    printf("    Workday Schedule:\n");
    m_impl->printSchedule(&schedules[seasons[i].m_workday_schedule_idx]);
    printf("    Weekend Schedule:\n");
    m_impl->printSchedule(&schedules[seasons[i].m_holiday_schedule_idx]);

    i++;
  }
}
#endif


bool
Calendar::findPeriod(uint8_t month,
		     uint8_t day,
		     uint8_t dayOfWeek,
		     uint8_t hour,
		     uint8_t min)
{
  unsigned char schedIdx = m_impl->findScheduleIndex(month, day, dayOfWeek);
  
  // Find the period based on current time
  uint8_t timeOfDay = 2 * hour + min / 30;
  unsigned char k = 0;
  while (k < MAX_CHANGE_POINTS-1 &&
	 m_impl->schedules[schedIdx].m_period_change[k+1].m_time < timeOfDay) k++;
  // “k” is now the index of the current period
  
  m_impl->m_currentCost = m_impl->schedules[schedIdx].m_period_change[k].m_period;

  // Now find next period
  m_impl->m_timeToNextPeriod = 0;
  while (m_impl->schedules[schedIdx].m_period_change[k].m_period != m_impl->m_currentCost) {
    // Is there another valid period in this schedule?
    if (k < MAX_CHANGE_POINTS-1 &&
	m_impl->schedules[schedIdx].m_period_change[k+1].m_time > 0) {
      k++;
      continue;
    }

    // Need to go to the next day
    m_impl->m_timeToNextPeriod += 48;
    // Must increment M/D/DOW variables to avoid infinite loop
    // if there is no period change in the next period
    // e.g. we’re OFF-PEAK and tomorrow is OFF-PEAK all day.
    month     = month % 12 + 1;
    day       = (day >= daysInMonth[month-1]) ? 1 : day+1;
    dayOfWeek = dayOfWeek%7 + 1;
    schedIdx = m_impl->findScheduleIndex(month, day, dayOfWeek);
    k = 0;
  }

  m_impl->m_nextPeriod = m_impl->schedules[schedIdx].m_period_change[k].m_period;
  m_impl->m_timeToNextPeriod += m_impl->schedules[schedIdx].m_period_change[k].m_time - timeOfDay;

  return true;
}


period_t
Calendar::getCurrentCost()
{
  return m_impl->m_currentCost;
}


period_t
Calendar::getNextCost()
{
  return m_impl->m_nextPeriod;
}


uint16_t
Calendar::getTimeToNextCost()
{
  return (m_impl->m_timeToNextPeriod > 65535/30) ? 65535 : m_impl->m_timeToNextPeriod * 30;
}


#ifdef TEST
int
main(int argc, const char* argv[])
{
  Calendar c;

  c.print();

  return 0;
}
#endif
