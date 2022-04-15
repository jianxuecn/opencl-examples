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
#ifndef __Timer_h
#define __Timer_h

class Timer
{
public:
	Timer();
    ~Timer();

    ///////////////////////////////////////////////////////////////////////////
    /// Get the elapsed number of seconds since January 1, 1970. This
    /// is also called Universal Coordinated Time.
    /// \return Return the Universal Coordinated Time in seconds.
    ///////////////////////////////////////////////////////////////////////////
    static double getUniversalTime();

    ///////////////////////////////////////////////////////////////////////////
    /// Get the CPU time for this process.
    /// \return Return the CPU time for this process.
    /// \note On Win32 platforms this actually returns wall time.
    ///////////////////////////////////////////////////////////////////////////
    static double getCPUTime();

    void start();
    void stop();

    double getElapsedTime();
    double getLastElapsedTime();

private:
    double mStartTime;
    double mStopTime;
};


#endif
