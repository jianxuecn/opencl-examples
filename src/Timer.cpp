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
