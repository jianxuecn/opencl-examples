#include "LogManager.h"
#include "FileLogger.h"

LogManager::LogManager()
{
    //mLogger = new FileLogger("OCLExample.log");
}

LogManager::~LogManager()
{
    //delete mLogger;
}

LogManager* LogManager::instancePtr()
{
    return &(instance());
}

LogManager& LogManager::instance()
{
    static LogManager theLogManager;
    return theLogManager;
}

void LogManager::stopLog()
{
    if (mLogger) mLogger->stop();
    mLogger = LoggerPointer();
}

