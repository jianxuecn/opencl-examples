#include "ImageProcessingDialog.h"
#include "LogManager.h"
#include "ExampleLogger.h"

#include <QApplication>
#include <QDir>
#include <QLocale>

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

    LoggerPointer logger(new ExampleLogger("OCLExample.log", mainDlg.logWidget()));
    LogManager::instance().setLogger(logger);

    LOG_INFO("Start!");

    int retCode = -1;
    if (mainDlg.initOCLContext()) {    }
    retCode = app.exec();
    LogManager::instance().stopLog();
    
    return retCode;
}
