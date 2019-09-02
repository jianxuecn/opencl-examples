#ifndef IMAGEVIEWWIDGET_H
#define IMAGEVIEWWIDGET_H

#include "ToolWindow.h"

namespace Ui {
class ImageViewWidget;
}

class QImage;

class ImageViewWidget : public ToolWindow
{
    Q_OBJECT

public:
    explicit ImageViewWidget(QWidget *parent = 0);
    ~ImageViewWidget();

    void showImage(QImage const &img);

private:
    Ui::ImageViewWidget *ui;
};

#endif // IMAGEVIEWWIDGET_H
