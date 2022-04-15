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
#include "LogWidget.h"
#include "ui_LogWidget.h"
#include "LogManager.h"

LogWidget::LogWidget(QWidget *parent) :
    ToolWindow(parent),
    ui(new Ui::LogWidget)
{
    ui->setupUi(this);
    ui->tabWidgetLog->setCurrentWidget(ui->tabLogInfo);
    mErrorCount = 0;
    mWarningCount = 0;
}

LogWidget::~LogWidget()
{
    delete ui;
    //LOG_DEBUG("Destroy LogWidget.");
}

void LogWidget::appendGenericMessage( QString const &msg )
{
    ui->plainTextEditLogInfo->appendPlainText(msg);
}

void LogWidget::appendErrorMessage( QString const &msg )
{
    ui->tabWidgetLog->setTabText(ui->tabWidgetLog->indexOf(ui->tabLogError), QString("Error (%1)").arg(++mErrorCount));
    ui->plainTextEditLogError->appendPlainText(msg);
}

void LogWidget::appendWarningMessage( QString const &msg )
{
    ui->tabWidgetLog->setTabText(ui->tabWidgetLog->indexOf(ui->tabLogWarning), QString("Warning (%1)").arg(++mWarningCount));
    ui->plainTextEditLogWarning->appendPlainText(msg);
}
