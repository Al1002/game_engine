/*=======================================================
  ______               _____  _       ____    _____  _  __
  / ____|_      _     / ____|| |     / __ \  / ____|| |/ /
 | |   _| |_  _| |_  | |     | |    | |  | || |     | ' / 
 | |  |_   _||_   _| | |     | |    | |  | || |     |  <  
 | |____|_|    |_|   | |____ | |____| |__| || |____ | . \ 
  \_____|             \_____||______|\____/  \_____||_|\_\
=========================================================*/

/*
    C++ clock - a utility class for a RTC in the C++ language 

    Copyright (C) 2024  Aleksandar Iliev "Bicepsa"

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "clock.h"
#include <time.h>
#include <unistd.h>

/**
 * @brief Timer will start on creation.
 * 
 */
Clock::Clock()
{
    start_timer();
}

/**
 * @brief (Re)Starts the timer.
 *
 * \sa `Clock::stop_timer()`
 */
void Clock::start_timer()
{
    stopped = false;
    clock_gettime(CLOCK_REALTIME, &present);
    past = present;
}

/**
 * @brief Will stop the timer. If the timer already stoped, does nothing.
 * When another time function is called, it will act as if the time of the stop is current time.
 *
 * @return Time since last start, 0 if timer is stopped.
 *
 * \sa `Clock::start_timer()`
 * \sa `Clock::resume_timer()`
 */
double Clock::stop_timer()
{
    if (stopped)
        return 0;
    stopped = true;
    clock_gettime(CLOCK_REALTIME, &stop); // get delta w/o deleting `present`
    return (stop.tv_sec - past.tv_sec) + (double)(stop.tv_nsec - past.tv_nsec) / NANOS_PER_SEC;
}

/**
 * @brief Returns time elapsed since the timer was started, in seconds.
 *
 * @return Time elapsed, in seconds
 */
double Clock::get_time()
{
    if (stopped)
        return (stop.tv_sec - past.tv_sec) + (double)(stop.tv_nsec - past.tv_nsec) / NANOS_PER_SEC;
    clock_gettime(CLOCK_REALTIME, &present);
    return (present.tv_sec - past.tv_sec) + (double)(present.tv_nsec - past.tv_nsec) / NANOS_PER_SEC;
}

/**
 * @brief Will resume the timer.
 * When another time function is called, will act as if the time, durring which the timer was stopped, didnt exist.
 *
 * @return The time for which the clock was stopped, in seconds, or 0 if the timer wasnt stopped.
 *
 * \sa `Clock::stop_timer()`
 */
double Clock::resume_timer()
{
    if (!stopped)
        return 0;
    stopped = false;
    clock_gettime(CLOCK_REALTIME, &present);
    delta = (present.tv_sec - stop.tv_sec) + (double)(present.tv_nsec - stop.tv_nsec) / NANOS_PER_SEC;
    past.tv_sec += present.tv_sec - stop.tv_sec; // adjust `past`
    past.tv_nsec += present.tv_nsec - stop.tv_nsec;
    if (past.tv_nsec >= NANOS_PER_SEC)
    {
        past.tv_nsec -= NANOS_PER_SEC;
        past.tv_sec++;
    }
    return delta;
}

/**
 * @brief Returns the durration since
 * the timer was started, in seconds,
 * then restarts the timer. Will wait for
 * at least `tick_duration` seconds to have elapsed since the timer was started before returning,
 * while blocking the thread.
 * Use for rapid time checking and setting iteration rate (ex: for loop).
 *
 * @return The time elapsed between function calls
 *
 * @note
 * Uses `usleep()` to block the thread while waiting.
 *
 * @param tick_durration The time to wait, in secconds
 */
double Clock::delta_time(double tick_durration)
{
    if (stopped)
        return delta;
    if (tick_durration <= 0)
    {
        clock_gettime(CLOCK_REALTIME, &present);
        delta = (present.tv_sec - past.tv_sec) + (double)(present.tv_nsec - past.tv_nsec) / NANOS_PER_SEC;
        past = present;
        return delta;
    }
    timespec wait_for = {past.tv_sec + (long)tick_durration, 0};
    wait_for.tv_nsec = past.tv_nsec + (long long)((tick_durration - (long)tick_durration) * NANOS_PER_SEC);
    if(wait_for.tv_nsec >= NANOS_PER_SEC)
    {
        wait_for.tv_nsec -= NANOS_PER_SEC;
        wait_for.tv_sec ++;
    }
    while (true)
    {
        int code = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &wait_for, NULL); 
        if(code == 0)
            break;
        else
            continue;
    }
    clock_gettime(CLOCK_REALTIME, &present);
    delta = (present.tv_sec - past.tv_sec) + (double)(present.tv_nsec - past.tv_nsec) / NANOS_PER_SEC;
    past = present;
    return delta;
}
