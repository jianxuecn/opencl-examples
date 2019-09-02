#include "FileLogger.h"

FileLogger::FileLogger(char const *logfilename)
{
    mLogFile.open(logfilename);
}

FileLogger::~FileLogger()
{
    //LOG_INFO("FileLogger " << (unsigned int)this << " destroy!");
    mLogFile.close();
}

void FileLogger::appendMessage( char const *msg )
{
    mLogFile << msg << std::endl;
}

void FileLogger::appendSegment( char const *seg )
{
    mLogFile << seg;
}
