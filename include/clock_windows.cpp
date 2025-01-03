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

#include <windows.h>
#include <cmath>
#include <thread>

Clock::Clock()
{
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&past);
}

void Clock::start_timer()
{
    QueryPerformanceCounter(&past);
    stopped = false;
}

double Clock::stop_timer()
{
    if (stopped)
        return 0.0;
    QueryPerformanceCounter(&stop);
    stopped = true;
    return (stop.QuadPart - past.QuadPart) / frequency.QuadPart;
}

double Clock::get_time()
{
    if (stopped)
        return (stop.QuadPart - past.QuadPart) / frequency.QuadPart;
    QueryPerformanceCounter(&present);
    return (present.QuadPart - past.QuadPart) / frequency.QuadPart;
}

double Clock::resume_timer()
{
    if (!stopped)
        return 0.0;
    LARGE_INTEGER resume_time;
    QueryPerformanceCounter(&resume_time);
    double stopped_duration = (resume_time.QuadPart - stop.QuadPart) / frequency.QuadPart;
    past.QuadPart += (resume_time.QuadPart - stop.QuadPart);
    stopped = false;
    return stopped_duration;
}

double Clock::delta_time(double tick_duration)
{
    QueryPerformanceCounter(&present);
    delta = (present.QuadPart - past.QuadPart) / frequency.QuadPart;
    if (delta < tick_duration)
    {
        double sleep_time = tick_duration - delta;
        if (sleep_time > 0)
            Sleep(MILIS_PER_SEC * sleep_time);
    }
    QueryPerformanceCounter(&present);
    delta = (present.QuadPart - past.QuadPart) / frequency.QuadPart;
    past = present;
    return delta;
}