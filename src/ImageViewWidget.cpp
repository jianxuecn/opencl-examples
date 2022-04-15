/**
 * -------------------------------------------------------------------------------
 * This source file is part of opencl-examples, which is developed for
 * the Cloud Computing Course and the Computer Graphics Course at the School
 * of Engineering Science (SES), University of Chinese Academy of Sciences (UCAS).
 * Copyright (C) 2020-2022 Xue Jian (xuejian@ucas.ac.cn)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * -------------------------------------------------------------------------------
 */
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
