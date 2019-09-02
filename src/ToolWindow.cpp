#include "ToolWindow.h"
#include <QCloseEvent>

ToolWindow::ToolWindow(QWidget *parent /*= 0*/) :
    QWidget(parent)
{
    this->setWindowFlags(Qt::Tool);
}

ToolWindow::~ToolWindow()
{

}

void ToolWindow::closeEvent(QCloseEvent *event)
{
    emit windowClosed();
    QWidget::closeEvent(event);
}

