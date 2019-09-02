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
