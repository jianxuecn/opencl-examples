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
#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include "ToolWindow.h"

namespace Ui {
class LogWidget;
}

class LogWidget : public ToolWindow
{
    Q_OBJECT

public:
    explicit LogWidget(QWidget *parent = 0);
    ~LogWidget();

    void appendGenericMessage(QString const &msg);
    void appendErrorMessage(QString const &msg);
    void appendWarningMessage(QString const &msg);

private:
    Ui::LogWidget *ui;
    int mErrorCount;
    int mWarningCount;
};

#endif // LOGWIDGET_H
