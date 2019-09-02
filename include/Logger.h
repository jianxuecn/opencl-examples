#ifndef __Logger_h
#define __Logger_h

#if defined(__APPLE__)
#include <memory>
#else
#include <memory>
#endif

class Logger
{
public:
    Logger();
    virtual ~Logger();
    virtual void appendGenericMessage(char const *msg);
    virtual void appendErrorMessage(char const *msg);
    virtual void appendWarningMessage(char const *msg);
    virtual void appendDebugMessage(char const *msg);

    virtual void appendMessage(char const *msg);
    virtual void appendSegment(char const *seg);

    virtual void stop() {}

protected:
};

#if defined(__APPLE__)
typedef std::shared_ptr<Logger> LoggerPointer;
#else
typedef std::tr1::shared_ptr<Logger> LoggerPointer;
#endif

#endif
