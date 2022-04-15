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
#ifndef IMAGEPROCESSINGDIALOG_H
#define IMAGEPROCESSINGDIALOG_H

#include <QDialog>
#include <QImage>

#if defined(__APPLE__)
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

namespace Ui {
class ImageProcessingDialog;
}

class ImageViewWidget;
class LogWidget;

class ImageProcessingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageProcessingDialog(QWidget *parent = 0);
    ~ImageProcessingDialog();

    LogWidget* logWidget() { return mLogWindow; }

    bool initOCLContext();

private slots:
    void openSourceFile();
    void saveResultFile();
    void showSourceWindow(bool s);
    void showResultWindow(bool s);
    void showLogWindow(bool s);
    void sourceWindowClosed();
    void resultWindowClosed();
    void logWindowClosed();
    void gaussKernelRadiusChanged(int val);
    void gaussKernelSigmaXChanged(double val);
    void gaussKernelSigmaYChanged(double val);
    void runOpenCLSmooth();
    void runOpenCLGradient();
	void runOpenCLSobel();
    void runNormalSmooth();
    void runNormalGradient();
	void runNormalSobel();

private:
    bool buildOCLMemoryObjects();
    void clearOCLContext();
    void clearGK();
    bool updateGK();
    bool loadSource();
    bool storeResult();

private:
    Ui::ImageProcessingDialog *ui;
    ImageViewWidget *mSourceWindow;
    ImageViewWidget *mResultWindow;
    LogWidget *mLogWindow;
    QImage mSourceImage;
    QImage mResultImage;
    bool mIsUpdatingUI;

    // for opencl
    cl_context mContext;
    cl_device_id mDeviceId;
    cl_mem mImageDataIn;
    cl_mem mImageIn;
    cl_mem mImageDataOut;
    cl_mem mGaussMask;
	cl_mem mSobelMiddleResult;
	cl_mem mMinValues;
	cl_mem mMaxValues;
    cl_command_queue mCommandQueue;
    cl_program mProgram;
    cl_kernel mSmoothKernel;
    cl_kernel mSmoothImageKernel;
    cl_kernel mGradientKernel;
    cl_kernel mGradientImageKernel;
	cl_kernel mSobelKernel;
	cl_kernel mMinMaxKernel;
	cl_kernel mScalePixelsKernel;

    unsigned int mImageBytes;
	unsigned int mMinMaxKernelLocalSize;
	unsigned int mMinMaxThreadNumPerBlock;
	unsigned int mMinMaxBlockNum;
    int mGKRadius;
    float *mGKMask;
    bool mSupportBAS;
    bool mGKModified;
};

#endif // IMAGEPROCESSINGDIALOG_H
