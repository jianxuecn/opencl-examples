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
#include "ImageProcessingDialog.h"
#include "LogUtils.h"
#include "ExampleLogger.h"
#include "AppInfo.h"

#include <QApplication>
#include <QDir>
#include <QLocale>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName(APP_ORG_NAME);
    QCoreApplication::setOrganizationDomain(APP_ORG_DOMAIN);
    QCoreApplication::setApplicationName(APP_NAME);
    //QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    QDir::setCurrent(QApplication::applicationDirPath());

    ImageProcessingDialog mainDlg;
    mainDlg.show();

    int retCode = -1;
    QDir logDir(QDir::homePath() + QDir::separator() + ".OCLExample");
    if (!logDir.exists()) {
        if (!logDir.mkpath(logDir.absolutePath())) {
            QMessageBox::critical(0, "Error", QString("Fail to access the directory: %1").arg(logDir.absolutePath()));
            return retCode;
        }
    }
    LoggerPointer logger(new ExampleLogger(logDir.absoluteFilePath("OCLExample.log").toLocal8Bit().constData(), mainDlg.logWidget()));
    LogManager::instance().setLogger(logger);

    QString arch;
    if (sizeof(void*) == 4) arch = "x86";
    else if (sizeof(void*) == 8) arch = "x64";
    else arch = "unknown arch";
    QString appinfo = QString("Start %1 %2.%3.%4 (%5)")
        .arg(APP_NAME)
        .arg(APP_VERSION_MAJOR)
        .arg(APP_VERSION_MINOR)
        .arg(APP_VERSION_PATCH)
        .arg(arch);
    log_info(appinfo);
    log_info(QString("Working dir: %1").arg(QCoreApplication::applicationDirPath()));

    if (mainDlg.initOCLContext()) {    }
    retCode = app.exec();
    LogManager::instance().stopLog();
    
    return retCode;
}
