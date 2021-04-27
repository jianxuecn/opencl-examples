#ifndef __LogManager_h
#define __LogManager_h

#include "Logger.h"
#include <sstream>

class LogManager
{
public:
    static LogManager * instancePtr();
    static LogManager & instance();

    void setLogger(LoggerPointer l) { mLogger = l; }
    Logger* logger() { return mLogger.get(); }

    void appendGenericMessage(char const *msg) { if (mLogger) mLogger->appendGenericMessage(msg); }
    void appendErrorMessage(char const *msg) { if (mLogger) mLogger->appendErrorMessage(msg); }
    void appendWarningMessage(char const *msg) { if (mLogger) mLogger->appendWarningMessage(msg); }
    void appendDebugMessage(char const *msg) { if (mLogger) mLogger->appendDebugMessage(msg); }
    void appendMessage(char const *msg) { if (mLogger) mLogger->appendMessage(msg); }
    void appendSegment(char const *seg) { if (mLogger) mLogger->appendSegment(seg); }

    void stopLog();

protected:
    LogManager();
    virtual ~LogManager();

    LoggerPointer mLogger;
};

#define LOG_INFO(x)    do { std::ostringstream _log_msg; _log_msg << "|    INFO | " << x; LogManager::instance().appendGenericMessage(_log_msg.str().c_str()); } while (0)
#define LOG_DEBUG(x)   do { std::ostringstream _log_msg; _log_msg << "|   DEBUG | in " __FILE__ ", line " << __LINE__ << ": "<< x; LogManager::instance().appendDebugMessage(_log_msg.str().c_str()); } while (0)
#define LOG_ERROR(x)   do { std::ostringstream _log_msg; _log_msg << "|   ERROR | in " __FILE__ ", line " << __LINE__ << ": "<< x; LogManager::instance().appendErrorMessage(_log_msg.str().c_str()); } while (0)
#define LOG_WARNING(x) do { std::ostringstream _log_msg; _log_msg << "| WARNING | in " __FILE__ ", line " << __LINE__ << ": "<< x; LogManager::instance().appendWarningMessage(_log_msg.str().c_str()); } while (0)
#define LOG_SEGMENT(x) do { std::ostringstream _log_msg; _log_msg << x; LogManager::instance().appendSegment(_log_msg.str().c_str()); } while (0)

#define GENERIC_MESSAGE(x) LOG_INFO(x)
#define DEBUG_MESSAGE(x) LOG_DEBUG(x)
#define ERROR_MESSAGE(x) LOG_ERROR(x)
#define WARNING_MESSAGE(x) LOG_WARNING(x)

#endif
