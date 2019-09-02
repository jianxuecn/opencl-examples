#include "ExampleLogger.h"
#include "LogManager.h"

ExampleLogger::ExampleLogger( char const *logfilename, LogWidget *logviewer )
    : FileLogger(logfilename)
{
    mLogViewer = logviewer;
}

ExampleLogger::~ExampleLogger()
{
    mLogViewer = 0;
    //LOG_DEBUG("Destroy ExampleLogger.");
}

void ExampleLogger::appendGenericMessage( char const *msg )
{
    FileLogger::appendGenericMessage(msg);
    if (mLogViewer) mLogViewer->appendGenericMessage(QString::fromUtf8(msg));
}

void ExampleLogger::appendErrorMessage( char const *msg )
{
    FileLogger::appendErrorMessage(msg);
    if (mLogViewer) mLogViewer->appendErrorMessage(QString::fromUtf8(msg));
}

void ExampleLogger::appendWarningMessage( char const *msg )
{
    FileLogger::appendWarningMessage(msg);
    if (mLogViewer) mLogViewer->appendWarningMessage(QString::fromUtf8(msg));
}

void ExampleLogger::appendDebugMessage( char const *msg )
{
    FileLogger::appendDebugMessage(msg);
    if (mLogViewer) mLogViewer->appendGenericMessage(QString::fromUtf8(msg));
}

void ExampleLogger::stop()
{
    mLogViewer = 0;
}

