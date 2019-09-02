#include "ImageViewWidget.h"
#include "ui_ImageViewWidget.h"
#include <QPixmap>
#include <QImage>

ImageViewWidget::ImageViewWidget(QWidget *parent) :
    ToolWindow(parent),
    ui(new Ui::ImageViewWidget)
{
    ui->setupUi(this);
}

ImageViewWidget::~ImageViewWidget()
{
    delete ui;
}

void ImageViewWidget::showImage(QImage const &img)
{
    ui->labelImage->setPixmap(QPixmap::fromImage(img));
}
