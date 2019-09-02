#include "Logger.h"
#include <iostream>

Logger::Logger()
{

}

Logger::~Logger()
{

}

void Logger::appendMessage( char const *msg )
{
    std::cout << msg << std::endl;
}

void Logger::appendSegment( char const *seg )
{
    std::cout << seg;
}

void Logger::appendGenericMessage( char const *msg )
{
    this->appendMessage(msg);
}

void Logger::appendErrorMessage( char const *msg )
{
    this->appendMessage(msg);
}

void Logger::appendWarningMessage( char const *msg )
{
    this->appendMessage(msg);
}

void Logger::appendDebugMessage( char const *msg )
{
    this->appendMessage(msg);
}
