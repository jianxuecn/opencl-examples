/**
 * -------------------------------------------------------------------------------
 * This source file is part of opencl-examples, which is developed for
 * the Cloud Computing Course and the Computer Graphics Course at the School
 * of Engineering Science (SES), University of Chinese Academy of Sciences (UCAS).
 * Copyright (C) 2020-2022 Xue Jian (xuejian@ucas.ac.cn)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * -------------------------------------------------------------------------------
 */
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

