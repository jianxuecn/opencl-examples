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
