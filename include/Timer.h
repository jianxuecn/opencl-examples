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
