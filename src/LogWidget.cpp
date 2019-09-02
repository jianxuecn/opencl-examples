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
