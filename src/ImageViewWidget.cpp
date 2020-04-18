#include "ImageViewWidget.h"
#include "ui_ImageViewWidget.h"
#include <QPixmap>
#include <QImage>

int const IMAGE_DISPLAY_WIDTH = 800;
int const IMAGE_DISPLAY_HEIGHT = 600;

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
    if (img.width()> IMAGE_DISPLAY_WIDTH || img.height()> IMAGE_DISPLAY_HEIGHT) {
        ui->labelImage->setPixmap(QPixmap::fromImage(img.scaled(IMAGE_DISPLAY_WIDTH, IMAGE_DISPLAY_HEIGHT, Qt::KeepAspectRatio)));
    } else {
        ui->labelImage->setPixmap(QPixmap::fromImage(img));
    }
    
}
