#ifndef __ExampleLogger_h
#define __ExampleLogger_h

#include "FileLogger.h"
#include "LogWidget.h"

class ExampleLogger : public FileLogger
{
public:
    ExampleLogger(char const *logfilename, LogWidget *logviewer);
    virtual ~ExampleLogger();

    void setLogViewer(LogWidget *logviewer) { mLogViewer = logviewer; }

    virtual void appendGenericMessage(char const *msg);
    virtual void appendErrorMessage(char const *msg);
    virtual void appendWarningMessage(char const *msg);
    virtual void appendDebugMessage(char const *msg);

    virtual void stop();

private:
    LogWidget *mLogViewer;
};


#endif

