#ifndef __ToolWindow_h
#define __ToolWindow_h

#include <QWidget>

class ToolWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ToolWindow(QWidget *parent = 0);
    ~ToolWindow();

signals:
    void windowClosed();

protected:
    virtual void closeEvent(QCloseEvent *event);

};


#endif
