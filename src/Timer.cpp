/**
 * -------------------------------------------------------------------------------
 * This source file is part of opencl-examples, which is developed for
 * the Cloud Computing Course and the Computer Graphics Course at the School
 * of Engineering Science (SES), University of Chinese Academy of Sciences (UCAS).
 * Copyright (C) 2020-2022 Xue Jian (xuejian@ucas.ac.cn)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * -------------------------------------------------------------------------------
 */
#include "Timer.h"
#ifdef WIN32
#include <time.h>
#include <sys/types.h> // Needed for Win32 implementation of timer
#include <sys/timeb.h> // Needed for Win32 implementation of timer
#else
#include <time.h>      // Needed for unix implementation of timer
#include <sys/time.h>  // Needed for unix implementation of timer
#include <sys/types.h> // Needed for unix implementation of timer
#include <sys/times.h> // Needed for unix implementation of timer
#endif
//-------------------------------------------------------------------------
Timer::Timer()
{
    mStartTime = mStopTime = 0;
}
//-------------------------------------------------------------------------
Timer::~Timer()
{
}
//----------------------------------------------------------------------------
double Timer::getUniversalTime()
{
    double currentTimeInSeconds;

#ifdef WIN32
    timeb CurrentTime;
    static double scale = 1.0/1000.0;
    ::ftime( &CurrentTime );
    currentTimeInSeconds = CurrentTime.time + scale * CurrentTime.millitm;
#else
    timeval CurrentTime;
    static double scale = 1.0/1000000.0;
    gettimeofday( &CurrentTime, NULL );
    currentTimeInSeconds = CurrentTime.tv_sec + scale * CurrentTime.tv_usec;
#endif

    return currentTimeInSeconds;
}
//----------------------------------------------------------------------------
double Timer::getCPUTime()
{
    double   currentCPUTime = 1.0;
    currentCPUTime = static_cast<double>(clock()) /static_cast<double>(CLOCKS_PER_SEC);
    return currentCPUTime;
}
//----------------------------------------------------------------------------
void Timer::start()
{
    mStartTime = Timer::getUniversalTime();
}
//----------------------------------------------------------------------------
void Timer::stop()
{
    mStopTime = Timer::getUniversalTime();
}
//----------------------------------------------------------------------------
double Timer::getElapsedTime()
{
    return (mStopTime - mStartTime);
}

double Timer::getLastElapsedTime()
{
    this->stop();
    double elapsedTime = mStopTime - mStartTime;
    this->start();
    return elapsedTime;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
