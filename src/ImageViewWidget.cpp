#include "ImageViewWidget.h"
#include "ui_ImageViewWidget.h"
#include <QPixmap>
#include <QImage>

ImageViewWidget::ImageViewWidget(QString const &title, QWidget *parent) :
    ToolWindow(parent),
    ui(new Ui::ImageViewWidget)
{
    ui->setupUi(this);
    //ui->labelImage->setScaledContents(true);
    mTitle = title;
    this->setWindowTitle(mTitle);
}

ImageViewWidget::~ImageViewWidget()
{
    delete ui;
}

void ImageViewWidget::showImage(QImage const &img)
{
    this->setWindowTitle(QString("%1 (%2 x %3)").arg(mTitle).arg(img.width()).arg(img.height()));
    if (img.width()>1024 || img.height()>1024) {
        ui->labelImage->setPixmap(QPixmap::fromImage(img.scaled(1024, 1024, Qt::KeepAspectRatio)));
    } else {
        ui->labelImage->setPixmap(QPixmap::fromImage(img));
    }
    
}
