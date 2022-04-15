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
#ifndef __CPUCounter_h
#define __CPUCounter_h

// system dependent, for accurate calculation of cpu counts
#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

class CPUCounter
{
public:
    CPUCounter() : mCounts(0) {}

	void start()
    {
#ifdef WIN32
        LARGE_INTEGER li;
        ::QueryPerformanceCounter(&li);
        mCounts = (double)(li.QuadPart);
#else
        mCounts = static_cast<double>(clock());
#endif
        mLastCounts = mCounts;
    }

	double getCounts()
    {
#ifdef WIN32
        LARGE_INTEGER li;
        ::QueryPerformanceCounter(&li);
        return (static_cast<double>(li.QuadPart) - mCounts);
#else
        return (static_cast<double>(clock()) - mCounts);
#endif
    }

    double getLastCounts()
    {
#ifdef WIN32
        LARGE_INTEGER li;
        ::QueryPerformanceCounter(&li);
        double counts = static_cast<double>(li.QuadPart);
#else
        double counts = static_cast<double>(clock());
#endif
        double ret = counts - mLastCounts;
        mLastCounts = counts;
        return (ret);
    }

	double getFrequence()
    {
#ifdef WIN32
        LARGE_INTEGER li;
        ::QueryPerformanceFrequency(&li);
        return static_cast<double>(li.QuadPart);
#else
        return static_cast<double>(CLOCKS_PER_SEC);
#endif
    }

    double getTime() { return (this->getCounts() / this->getFrequence()); }
    double getLastTime() { return (this->getLastCounts() / this->getFrequence()); }

protected:
	double mCounts;
    double mLastCounts;

};

#endif
