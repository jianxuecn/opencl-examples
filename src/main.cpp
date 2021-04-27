#include "ImageProcessingDialog.h"
#include "LogManager.h"
#include "ExampleLogger.h"

#include <QApplication>
#include <QDir>
#include <QLocale>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("UCAS");
    QCoreApplication::setOrganizationDomain("ucas.ac.cn");
    QCoreApplication::setApplicationName("OCLExample");
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

    LOG_INFO("Start!");

    if (mainDlg.initOCLContext()) {    }
    retCode = app.exec();
    LogManager::instance().stopLog();
    
    return retCode;
}
