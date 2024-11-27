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

#pragma once
#include <time.h>

/**
 * @brief A clock that can be used for measuring time and limiting loop speed.
 * 			Has: `start_timer()`, `stop_timer()`, `get_time()`, `resume_timer()`, `delta_time()`
 * @note
 * Note(s): Works with RTC, NOT thread time; Uses time since system start,
 * aka may break if OS has not been rebooted for a looong time.
 */
class Clock
{
	const static long NANOS_PER_SEC = 1000000000;
	const static long MICROS_PER_SEC = 1000000;
	timespec past, present, stop; // past - when the timer was last started, present - static place to load current time, stop - when the timer was stopped
	double delta;
	bool stopped = false;

public:

	/**
	 * @brief Timer will start on creation.
	 * 
	 */
	Clock();
	/**
	 * @brief Starts the timer.
	 *
	 * \sa `Clock::stop_timer()`
	 */
	void start_timer();

	/**
	 * @brief Will stop the timer. If the timer already stoped, does nothing.
	 * When another time function is called, it will act as if the time of the stop is current time.
	 *
	 * @return Time since last start, 0 if timer is stopped.
	 *
	 * \sa `Clock::start_timer()`
	 * \sa `Clock::resume_timer()`
	 */
	double stop_timer();

	/**
	 * @brief Returns time elapsed since the timer was started.
	 *
	 * @return Time elapsed, in seconds
	 */
	double get_time();

	/**
	 * @brief Will resume the timer.
	 * When another time function is called, will act as if the time, durring which the timer was stopped, didnt exist.
	 *
	 * @return The time for which the clock was stopped, in seconds, or 0 if the timer wasnt stopped.
	 *
	 * \sa `Clock::stop_timer()`
	 */
	double resume_timer();

	/**
	 * @brief Returns the durration since
	 * the timer was started, in seconds,
	 * then restarts the timer. Will wait for
	 * at least `tick_duration` seconds to have elapsed before returning,
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
	double delta_time(double tick_durration = -1 );


};
