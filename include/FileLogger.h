#ifndef __FileLogger_h
#define __FileLogger_h

#include "Logger.h"
#include <fstream>

class FileLogger : public Logger
{
public:
    FileLogger(char const *logfilename);
    virtual ~FileLogger();

    virtual void appendMessage(char const *msg);
    virtual void appendSegment(char const *seg);

protected:
    std::ofstream mLogFile;
};

#endif
