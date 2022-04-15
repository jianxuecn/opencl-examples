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
#include "ImageProcessingDialog.h"
#include "ui_ImageProcessingDialog.h"
#include "ImageViewWidget.h"
#include "LogWidget.h"
#include "LogManager.h"
#include "OCLUtils.h"
#include "Timer.h"

#include <QFileDialog>
#include <QDir>

#include <math.h>

double const OCLEXAMPLE_PI = 3.14159265358979323846;

inline bool is_pow2(unsigned int x)
{
	return ((x&(x - 1)) == 0);
}

inline unsigned int next_pow2(unsigned int x) {
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return ++x;
}

template <typename FloatT>
bool generate_gaussian_kernel(unsigned int size, FloatT sx, FloatT sy, FloatT *k)
{
    if (size<=0 || k==0) return false;
    if (size%2==0) {
        //ERROR_MESSAGE("The size of gaussian kernel must be odd!");
        return false;
    }

    int half_size = size >> 1;
    FloatT sumval = 0;
    for (int x=-half_size; x<=half_size; ++x) {
        for (int y=-half_size; y<=half_size; ++y) {
            int idx = (y+half_size)*size +(x+half_size);
            k[idx] = (FloatT)(exp(-(x*x*0.5/(sx*sx)+y*y*0.5/(sy*sy)))
                /(2.0*OCLEXAMPLE_PI*sx*sy));
            sumval += k[idx];
        }
    }

    for (unsigned int i=0; i<size*size; ++i) k[i] /= sumval;

    return true;
}

ImageProcessingDialog::ImageProcessingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageProcessingDialog)
{
    ui->setupUi(this);
    ui->toolBox->setCurrentWidget(ui->pageSmooth);
    this->move(10, 10);
    mIsUpdatingUI = false;

    mSourceWindow = new ImageViewWidget(tr("Source Image"));
    //mSourceWindow->move(this->x()+this->frameGeometry().width(), this->y());

    mResultWindow = new ImageViewWidget(tr("Result Image"));
    //mResultWindow->move(mSourceWindow->x()+mSourceWindow->frameGeometry().width(), mSourceWindow->y());

    mLogWindow = new LogWidget();
    //mLogWindow->move(mSourceWindow->x(), mSourceWindow->y()+mSourceWindow->frameGeometry().height());
    //mLogWidget->show();

    connect(ui->pushButtonOpen, SIGNAL(clicked()), this, SLOT(openSourceFile()));
    connect(ui->pushButtonSave, SIGNAL(clicked()), this, SLOT(saveResultFile()));
    connect(ui->checkBoxSource, SIGNAL(toggled(bool)), this, SLOT(showSourceWindow(bool)));
    connect(ui->checkBoxResult, SIGNAL(toggled(bool)), this, SLOT(showResultWindow(bool)));
    connect(ui->checkBoxLog, SIGNAL(toggled(bool)), this, SLOT(showLogWindow(bool)));

    connect(mLogWindow, SIGNAL(windowClosed()), this, SLOT(logWindowClosed()));
    connect(mSourceWindow, SIGNAL(windowClosed()), this, SLOT(sourceWindowClosed()));
    connect(mResultWindow, SIGNAL(windowClosed()), this, SLOT(resultWindowClosed()));

    connect(ui->spinBoxGKRadius, SIGNAL(valueChanged(int)), this, SLOT(gaussKernelRadiusChanged(int)));
    connect(ui->doubleSpinBoxGKSigmaX, SIGNAL(valueChanged(double)), this, SLOT(gaussKernelSigmaXChanged(double)));
    connect(ui->doubleSpinBoxGKSigmaY, SIGNAL(valueChanged(double)), this, SLOT(gaussKernelSigmaYChanged(double)));
    connect(ui->pushButtonRunSmoothOpenCL, SIGNAL(clicked()), this, SLOT(runOpenCLSmooth()));
    connect(ui->pushButtonRunSmoothNormal, SIGNAL(clicked()), this, SLOT(runNormalSmooth()));

    connect(ui->pushButtonRunGradientOpenCL, SIGNAL(clicked()), this, SLOT(runOpenCLGradient()));
    connect(ui->pushButtonRunGradientNormal, SIGNAL(clicked()), this, SLOT(runNormalGradient()));

	connect(ui->pushButtonRunSobelOpenCL, SIGNAL(clicked()), this, SLOT(runOpenCLSobel()));
	connect(ui->pushButtonRunSobelNormal, SIGNAL(clicked()), this, SLOT(runNormalSobel()));

    // for opencl
    mContext = 0;
    mDeviceId = 0;
    mImageDataIn = 0;
    mImageIn = 0;
    mImageDataOut = 0;
	mSobelMiddleResult = 0;
	mMinValues = 0;
	mMaxValues = 0;
    mGaussMask = 0;
    mCommandQueue = 0;
    mProgram = 0;
    mSmoothKernel = 0;
    mSmoothImageKernel = 0;
    mGradientKernel = 0;
    mGradientImageKernel = 0;
	mSobelKernel = 0;
	mMinMaxKernel = 0;
	mScalePixelsKernel = 0;

    mImageBytes = 0;
    mGKRadius = 0;
    mGKMask = 0;
    mSupportBAS = false;
    mGKModified = true;
}

ImageProcessingDialog::~ImageProcessingDialog()
{
    this->clearGK();
    this->clearOCLContext();
    delete mSourceWindow;
    delete mResultWindow;
    delete mLogWindow;
    delete ui;
}

void ImageProcessingDialog::clearGK()
{
    delete []mGKMask;
    mGKMask = 0;
    mGKRadius = 0;
    
    if (mGaussMask) clReleaseMemObject(mGaussMask);
    mGaussMask = 0;
}

bool ImageProcessingDialog::updateGK()
{
    if (mGKModified) {
        this->clearGK();
        
        mGKRadius = ui->spinBoxGKRadius->value();
        unsigned int size = (mGKRadius<<1)+1;
        mGKMask = new float[size*size];
        float sx = ui->doubleSpinBoxGKSigmaX->value();
        float sy = ui->doubleSpinBoxGKSigmaY->value();
        if (!generate_gaussian_kernel(size, sx, sy, mGKMask)) {
            return false;
        }
        
        cl_int retCode;
        size_t bufferBytes = sizeof(float) * size * size;
        mGaussMask = clCreateBuffer(mContext, CL_MEM_READ_ONLY, bufferBytes, 0, &retCode);
        if (retCode != CL_SUCCESS) {
            ERROR_MESSAGE("Create buffer for gauss kernel mask failed! Error code = " << retCode);
            return false;
        }
        
        retCode = clEnqueueWriteBuffer(mCommandQueue, mGaussMask,
                                       CL_TRUE, 0, bufferBytes, mGKMask, 0, NULL, NULL);
        if (retCode != CL_SUCCESS) {
            ERROR_MESSAGE("Write gauss kernel mask to buffer failed! Error code = " << retCode);
            return false;
        }
        
        mGKModified = false;
    }
    
    return true;
}

void ImageProcessingDialog::openSourceFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Source Image", QString(), "Images (*.png *.bmp *.jpg *.tif)");
    if (fileName.isEmpty()) return;
    QImage image(fileName);
    if (image.format() == QImage::Format_RGB32) {
        mSourceImage = image;
    } else {
        mSourceImage = image.convertToFormat(QImage::Format_RGB32);
    }

    mResultImage = mSourceImage.copy();
    
    mSourceWindow->showImage(mSourceImage);
    ui->checkBoxSource->setChecked(true);
    //mSourceWidget->show();

}

void ImageProcessingDialog::showSourceWindow(bool s)
{
    if (mIsUpdatingUI) return;
    if (s == mSourceWindow->isVisible()) return;
    mSourceWindow->move(this->x()+this->frameGeometry().width(), this->y());
    mSourceWindow->setVisible(s);
}

void ImageProcessingDialog::showResultWindow(bool s)
{
    if (mIsUpdatingUI) return;
    if (s == mResultWindow->isVisible()) return;
    mResultWindow->move(mSourceWindow->x()+30, mSourceWindow->y()+20);
    mResultWindow->setVisible(s);
}

void ImageProcessingDialog::showLogWindow(bool s)
{
    if (mIsUpdatingUI) return;
    if (s == mLogWindow->isVisible()) return;
    mLogWindow->move(this->x(), this->y()+this->frameGeometry().height());
    mLogWindow->setVisible(s);
}

void ImageProcessingDialog::sourceWindowClosed()
{
    mIsUpdatingUI = true;
    ui->checkBoxSource->setChecked(false);
    mIsUpdatingUI = false;
}

void ImageProcessingDialog::resultWindowClosed()
{
    mIsUpdatingUI = true;
    ui->checkBoxResult->setChecked(false);
    mIsUpdatingUI = false;
}

void ImageProcessingDialog::logWindowClosed()
{
    mIsUpdatingUI = true;
    ui->checkBoxLog->setChecked(false);
    mIsUpdatingUI = false;
}

bool ImageProcessingDialog::initOCLContext()
{
    LOG_INFO("Initialize OpenCL environment.");

    cl_platform_id platformID;
    cl_int retCode;
    char stringBuffer[1024];

    bool isNV = false;
    if (get_platform_id(platformID, isNV) != CL_SUCCESS) return false;
    
    //list_opencl_devices(platformID);
    cl_device_id deviceID;
    if (get_device_id(platformID, deviceID, isNV) != CL_SUCCESS) return false;

    mDeviceId = deviceID;
    cl_context_properties prop[] = { 
        CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platformID), 
        0 
    };
    //mContext = clCreateContextFromType(prop, CL_DEVICE_TYPE_GPU, NULL, NULL, &retCode);
    mContext = clCreateContext(prop, 1, &mDeviceId, NULL, NULL, &retCode);

    if (retCode != CL_SUCCESS) {
        ERROR_MESSAGE("Cannot create OpenCL context! Error code = " << retCode);
        return false;
    }

    //mDeviceId = get_max_flops_device(mContext);
    show_opencl_device_info(mDeviceId);
    clGetDeviceInfo(mDeviceId, CL_DEVICE_EXTENSIONS, sizeof(stringBuffer), &stringBuffer, NULL);

    mSupportBAS = false;
    if (strstr(stringBuffer, "cl_khr_byte_addressable_store") != NULL) {
        mSupportBAS = true;
        LOG_INFO("cl_khr_byte_addressable_store supported!");
    }
    LOG_INFO("Use byte-addressable-store: " << (mSupportBAS ? "TRUE" : "FALSE"));

    mCommandQueue = clCreateCommandQueue(mContext, mDeviceId, 0, &retCode);
    if (retCode != CL_SUCCESS) {
        ERROR_MESSAGE("Create command queue failed! Error code = " << retCode);
        return false;
    }

    cl_program program;
    QDir filterDir(QApplication::applicationDirPath()+QDir::separator()+OCL_FILTERS_SUB_DIR);
    if (load_cl_source_from_file(mContext, filterDir.absoluteFilePath("image_processing.cl").toLocal8Bit().constData(), program)) {
        mProgram = program;
    } else {
        return false;
    }

    std::string buildOpts = "-cl-fast-relaxed-math";
    retCode = clBuildProgram(mProgram, 1, &(mDeviceId), buildOpts.c_str(), NULL, NULL);
    if (retCode != CL_SUCCESS) {
        char buildLog[10240];
        clGetProgramBuildInfo(mProgram, mDeviceId, CL_PROGRAM_BUILD_LOG, sizeof(buildLog), buildLog, NULL);
        ERROR_MESSAGE("Build OpenCL program failed: " << buildLog);
        return false;
    }
    LOG_INFO("OpenCL program build successful!");

    mSmoothKernel = clCreateKernel(mProgram, "k_smooth_gaussian_uchar4", &retCode);
    if (retCode != CL_SUCCESS) {
        ERROR_MESSAGE("Cannot create smooth kernel! Error code = " << retCode);
        return false;
    }

    mSmoothImageKernel = clCreateKernel(mProgram, "k_smooth_gaussian_image", &retCode);
    if (retCode != CL_SUCCESS) {
        ERROR_MESSAGE("Cannot create smooth kernel (input use image)! Error code = " << retCode);
        return false;
    }

    mGradientKernel = clCreateKernel(mProgram, "k_gradient_uchar4", &retCode);
    if (retCode != CL_SUCCESS) {
        ERROR_MESSAGE("Cannot create gradient kernel! Error code = " << retCode);
        return false;
    }

    mGradientImageKernel = clCreateKernel(mProgram, "k_gradient_image", &retCode);
    if (retCode != CL_SUCCESS) {
        ERROR_MESSAGE("Cannot create gradient kernel (input use image)! Error code = " << retCode);
        return false;
    }

	mSobelKernel = clCreateKernel(mProgram, "k_sobel", &retCode);
	if (retCode != CL_SUCCESS) {
		ERROR_MESSAGE("Cannot create sobel kernel! Error code = " << retCode);
		return false;
	}

	mMinMaxKernel = clCreateKernel(mProgram, "k_min_max", &retCode);
	if (retCode != CL_SUCCESS) {
		ERROR_MESSAGE("Cannot create minmax kernel! Error code = " << retCode);
		return false;
	}

	mScalePixelsKernel = clCreateKernel(mProgram, "k_scale_pixels", &retCode);
	if (retCode != CL_SUCCESS) {
		ERROR_MESSAGE("Cannot create scale pixels kernel! Error code = " << retCode);
		return false;
	}

    size_t localSize;
    retCode = clGetKernelWorkGroupInfo(mSmoothKernel, mDeviceId, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &localSize, NULL);
    if (retCode != CL_SUCCESS) {
        ERROR_MESSAGE("Get kernel work group size of smooth kernel failed! Error code = " << retCode);
        return false;
    } else {
        LOG_INFO("Smooth kernel work group size: " << localSize);
    }

	retCode = clGetKernelWorkGroupInfo(mMinMaxKernel, mDeviceId, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &localSize, NULL);
	if (retCode != CL_SUCCESS) {
		ERROR_MESSAGE("Get kernel work group size of minmax kernel failed! Error code = " << retCode);
		return false;
	}
	else {
		LOG_INFO("Minmax kernel work group size: " << localSize);
	}

	mMinMaxKernelLocalSize = (unsigned int)localSize;
	if (!is_pow2(mMinMaxKernelLocalSize)) {
		mMinMaxKernelLocalSize = next_pow2((mMinMaxKernelLocalSize + 2) >> 1);
		LOG_INFO("Reduce minmax kernel work group size to (power of 2): " << mMinMaxKernelLocalSize);
	}

    LOG_INFO("OpenCL context successfully initialized!");
    return true;
}

void ImageProcessingDialog::clearOCLContext()
{
    if (mSmoothKernel) { clReleaseKernel(mSmoothKernel); mSmoothKernel = 0; }
    if (mSmoothImageKernel) { clReleaseKernel(mSmoothImageKernel); mSmoothImageKernel = 0; }
    if (mGradientKernel) { clReleaseKernel(mGradientKernel); mGradientKernel = 0; }
    if (mGradientImageKernel) { clReleaseKernel(mGradientImageKernel); mGradientImageKernel = 0; }
	if (mSobelKernel) { clReleaseKernel(mSobelKernel); mSobelKernel = 0; }
	if (mMinMaxKernel) { clReleaseKernel(mMinMaxKernel); mMinMaxKernel = 0; }
	if (mScalePixelsKernel) { clReleaseKernel(mScalePixelsKernel); mScalePixelsKernel = 0; }
	if (mProgram) { clReleaseProgram(mProgram); mProgram = 0; }
    if (mImageDataIn) { clReleaseMemObject(mImageDataIn); mImageDataIn = 0; }
    if (mImageIn) { clReleaseMemObject(mImageIn); mImageIn = 0; }
    if (mImageDataOut) { clReleaseMemObject(mImageDataOut); mImageDataOut = 0; }
	if (mSobelMiddleResult) { clReleaseMemObject(mSobelMiddleResult); mSobelMiddleResult = 0; }
	if (mMinValues) { clReleaseMemObject(mMinValues); mMinValues = 0; }
	if (mMaxValues) { clReleaseMemObject(mMaxValues); mMaxValues = 0; }
	if (mGaussMask) { clReleaseMemObject(mGaussMask); mGaussMask = 0; }
	if (mCommandQueue) { clReleaseCommandQueue(mCommandQueue); mCommandQueue = 0; }
    if (mContext) { clReleaseContext(mContext); mContext = 0; }
}

bool ImageProcessingDialog::buildOCLMemoryObjects()
{
    cl_int retCode;

    if (mImageBytes != mSourceImage.sizeInBytes()) {
        if (mImageDataIn) { clReleaseMemObject(mImageDataIn); mImageDataIn = 0; }
        if (mImageIn) { clReleaseMemObject(mImageIn); mImageIn = 0; }
        if (mImageDataOut) { clReleaseMemObject(mImageDataOut); mImageDataOut = 0; }
		if (mSobelMiddleResult) { clReleaseMemObject(mSobelMiddleResult); mSobelMiddleResult = 0; }
		if (mMinValues) { clReleaseMemObject(mMinValues); mMinValues = 0; }
		if (mMaxValues) { clReleaseMemObject(mMaxValues); mMaxValues = 0; }

        mImageBytes = mSourceImage.sizeInBytes();

        mImageDataIn = clCreateBuffer(mContext,
            CL_MEM_READ_ONLY, mImageBytes, 0, &retCode);
        if (retCode != CL_SUCCESS) {
            ERROR_MESSAGE("Create buffer for input image failed! Error code = " << retCode);
            return false;
        }

        size_t imgWidth = mSourceImage.width();
        size_t imgHeight = mSourceImage.height();
        cl_image_format imgFormat;
        imgFormat.image_channel_order = CL_BGRA;
        imgFormat.image_channel_data_type = CL_UNORM_INT8;
        //mImageIn = clCreateImage2D(mContext, CL_MEM_READ_ONLY, &imgFormat, imgWidth, imgHeight, 0, NULL, &retCode);
        cl_image_desc imgDesc;
        memset(&imgDesc, 0, sizeof(cl_image_desc)); // !!IMPORTANT!!
        imgDesc.image_type = CL_MEM_OBJECT_IMAGE2D;
        imgDesc.image_width = imgWidth;
        imgDesc.image_height = imgHeight;
        imgDesc.image_row_pitch = 0;
        imgDesc.image_slice_pitch = 0;
        imgDesc.num_mip_levels = 0;
        imgDesc.num_samples = 0;
        imgDesc.buffer = NULL;
        mImageIn = clCreateImage(mContext, CL_MEM_READ_ONLY, &imgFormat, &imgDesc, NULL, &retCode);
        if (retCode != CL_SUCCESS) {
            ERROR_MESSAGE("Create image 2D for input image failed! Image size = " << imgWidth << "x" << imgHeight << ". " <<  "Error code = " << retCode);
            return false;
        }

        mImageDataOut = clCreateBuffer(mContext,
            CL_MEM_WRITE_ONLY, mImageBytes, NULL, &retCode);
        if (retCode != CL_SUCCESS) {
            ERROR_MESSAGE("Create buffer for output image failed! Error code = " << retCode);
            return false;
        }

		size_t pixelNum = imgWidth*imgHeight;
		mSobelMiddleResult = clCreateBuffer(mContext,
			CL_MEM_WRITE_ONLY, pixelNum *sizeof(float), NULL, &retCode);
		if (retCode != CL_SUCCESS) {
			ERROR_MESSAGE("Create buffer for sobel middle result failed! Error code = " << retCode);
			return false;
		}

		mMinMaxThreadNumPerBlock = (pixelNum < mMinMaxKernelLocalSize) ? next_pow2(static_cast<unsigned int>(pixelNum)) : mMinMaxKernelLocalSize;
		mMinMaxBlockNum = static_cast<unsigned int>(pixelNum + mMinMaxThreadNumPerBlock - 1) / mMinMaxThreadNumPerBlock;
		if (mMinMaxBlockNum == 0) {
			ERROR_MESSAGE("Block number of minmax kernel is 0!");
			return false;
		}

		mMinValues = clCreateBuffer(mContext,
			CL_MEM_WRITE_ONLY, mMinMaxBlockNum * sizeof(float), NULL, &retCode);
		if (retCode != CL_SUCCESS) {
			ERROR_MESSAGE("Create buffer for min results failed! Error code = " << retCode);
			return false;
		}

		mMaxValues = clCreateBuffer(mContext,
			CL_MEM_WRITE_ONLY, mMinMaxBlockNum * sizeof(float), NULL, &retCode);
		if (retCode != CL_SUCCESS) {
			ERROR_MESSAGE("Create buffer for max results failed! Error code = " << retCode);
			return false;
		}
	}

    if (mGKModified) {
        return this->updateGK();
    }

    return true;

}

void ImageProcessingDialog::gaussKernelRadiusChanged(int val)
{
    mGKModified = true;
}

void ImageProcessingDialog::gaussKernelSigmaXChanged(double val)
{
    mGKModified = true;
}

void ImageProcessingDialog::gaussKernelSigmaYChanged(double val)
{
    mGKModified = true;
}

void ImageProcessingDialog::runOpenCLSmooth()
{
    Timer timer;
    timer.start();

    if (!this->loadSource()) {
        return;
    }

    cl_int retCode;

    cl_kernel smoothKernel = mSmoothKernel;
    if (ui->checkBoxInputUseImage->isChecked())  smoothKernel = mSmoothImageKernel;

    int imageWidth = mSourceImage.width();
    int imageHeight = mSourceImage.height();
    retCode = clSetKernelArg(smoothKernel, 0, sizeof(cl_mem), &(mImageDataOut));
    if (ui->checkBoxInputUseImage->isChecked()) {
        retCode |= clSetKernelArg(smoothKernel, 1, sizeof(cl_mem), &(mImageIn));
    } else {
        retCode |= clSetKernelArg(smoothKernel, 1, sizeof(cl_mem), &(mImageDataIn));
    }
    retCode |= clSetKernelArg(smoothKernel, 2, sizeof(cl_mem), &(mGaussMask));
    retCode |= clSetKernelArg(smoothKernel, 3, sizeof(int), &(imageWidth));
    retCode |= clSetKernelArg(smoothKernel, 4, sizeof(int), &(imageHeight));
    retCode |= clSetKernelArg(smoothKernel, 5, sizeof(int), &(mGKRadius));
    if (retCode != CL_SUCCESS) {
        ERROR_MESSAGE("Set smooth kernel arguments failed! Error code = " << retCode);
        return;
    }

    //size_t localDim[2] = { 256, 1 };
    size_t globalDim[2] = { static_cast<size_t>(imageWidth), static_cast<size_t>(imageHeight) };
    retCode = clEnqueueNDRangeKernel(mCommandQueue, smoothKernel,
        2, NULL, globalDim, NULL, 0, NULL, NULL);
    if (retCode != CL_SUCCESS) {
        ERROR_MESSAGE("Enqueue smooth kernel failed! Error code = " << retCode);
        return;
    }
    clFinish(mCommandQueue);

    if (!this->storeResult()) {
        return;
    }

    LOG_INFO("OpenCL smooth kernel execution time: " << timer.getLastElapsedTime() << "s");

    mResultWindow->showImage(mResultImage);
    //LOG_INFO("showImage execution time: " << ccounter.getLastTime() << "s");

}

void ImageProcessingDialog::runNormalSmooth()
{
    Timer timer;
    timer.start();

    if (mGKModified) {
        if (!this->updateGK()) {
            return;
        }
    }
    
    int img_width = mSourceImage.width();
    int img_height = mSourceImage.height();
    int line_bytes = mSourceImage.bytesPerLine();
    uchar const *data_in = mSourceImage.bits();
    uchar *data_out = mResultImage.bits();
    
    int gksize = (mGKRadius<<1) + 1;
    for (int y=0; y<img_height; ++y) {
        uchar *pout = data_out + y*line_bytes;
        for (int x=0; x<img_width; ++x) {
            float sum[4] = { 0, 0, 0, 0 };
            for (int ky=-mGKRadius; ky<=mGKRadius; ++ky) {
                for (int kx=-mGKRadius; kx<=mGKRadius; ++kx) {
                    int inx = x + kx;
                    int iny = y + ky;
                    if (inx<0 || inx>=img_width) continue;
                    if (iny<0 || iny>=img_height) continue;
                    uchar const *pin = data_in + iny*line_bytes + inx*4;
                    float gkvalue = mGKMask[(ky+mGKRadius)*gksize + kx + mGKRadius];
                    sum[0] += pin[0] * gkvalue;
                    sum[1] += pin[1] * gkvalue;
                    sum[2] += pin[2] * gkvalue;
                    sum[3] += pin[3] * gkvalue;
                }
            }
            pout[0] = (uchar)(sum[0]);
            pout[1] = (uchar)(sum[1]);
            pout[2] = (uchar)(sum[2]);
            pout[3] = (uchar)(sum[3]);
            pout += 4;
        }
    }
    
    LOG_INFO("Normal smooth execution time: " << timer.getLastElapsedTime() << "s");
    
    mResultWindow->showImage(mResultImage);
    
}

void ImageProcessingDialog::runOpenCLGradient()
{
    Timer timer;
    timer.start();

    if (!this->loadSource()) {
        return;
    }

    cl_int retCode;

    cl_kernel gradientKernel = mGradientKernel;
    if (ui->checkBoxInputUseImage->isChecked())  gradientKernel = mGradientImageKernel;

    int imageWidth = mSourceImage.width();
    int imageHeight = mSourceImage.height();
    retCode = clSetKernelArg(gradientKernel, 0, sizeof(cl_mem), &(mImageDataOut));
    if (ui->checkBoxInputUseImage->isChecked()) {
        retCode |= clSetKernelArg(gradientKernel, 1, sizeof(cl_mem), &(mImageIn));
    } else {
        retCode |= clSetKernelArg(gradientKernel, 1, sizeof(cl_mem), &(mImageDataIn));
    }
    retCode |= clSetKernelArg(gradientKernel, 2, sizeof(int), &(imageWidth));
    retCode |= clSetKernelArg(gradientKernel, 3, sizeof(int), &(imageHeight));
    if (retCode != CL_SUCCESS) {
        ERROR_MESSAGE("Set gradient kernel arguments failed! Error code = " << retCode);
        return;
    }

    //size_t localDim[2] = { 32, 8 };
    size_t globalDim[2] = { static_cast<size_t>(imageWidth), static_cast<size_t>(imageHeight) };
    retCode = clEnqueueNDRangeKernel(mCommandQueue, gradientKernel,
        2, NULL, globalDim, NULL, 0, NULL, NULL);
    if (retCode != CL_SUCCESS) {
        ERROR_MESSAGE("Enqueue gradient kernel failed! Error code = " << retCode);
        return;
    }

    if (!this->storeResult()) {
        return;
    }

    clFinish(mCommandQueue);
    LOG_INFO("OpenCL gradient kernel execution time: " << timer.getLastElapsedTime() << "s");

    mResultWindow->showImage(mResultImage);
}

void ImageProcessingDialog::runNormalGradient()
{
    Timer timer;
    timer.start();
    
    int img_width = mSourceImage.width();
    int img_height = mSourceImage.height();
    int line_bytes = mSourceImage.bytesPerLine();
    uchar const *data_in = mSourceImage.bits();
    uchar *data_out = mResultImage.bits();
    
    for (int y=0; y<img_height; ++y) {
        uchar *pout = data_out + y*line_bytes;
        for (int x=0; x<img_width; ++x) {
            int x1 = (x+1)%img_width;
            int y1 = (y+1)%img_height;
            uchar const *pin0 = data_in + y*line_bytes + x*4;
            uchar const *pin1 = data_in + y*line_bytes + x1*4;
            uchar const *pin2 = data_in + y1*line_bytes + x*4;
            uchar const *pin3 = data_in + y1*line_bytes + x1*4;
            float gradx[4] = { static_cast<float>(pin3[0]-pin0[0]), static_cast<float>(pin3[1]-pin0[1]), static_cast<float>(pin3[2]-pin0[2]), static_cast<float>(pin3[3]-pin0[3]) };
            float grady[4] = { static_cast<float>(pin1[0]-pin2[0]), static_cast<float>(pin1[1]-pin2[1]), static_cast<float>(pin1[2]-pin2[2]), static_cast<float>(pin1[3]-pin2[3]) };
            pout[0] = (uchar)(sqrt(gradx[0]*gradx[0]+grady[0]*grady[0]));
            pout[1] = (uchar)(sqrt(gradx[1]*gradx[1]+grady[1]*grady[1]));
            pout[2] = (uchar)(sqrt(gradx[2]*gradx[2]+grady[2]*grady[2]));
            pout[3] = 255;//(uchar)(sqrt(gradx[3]*gradx[3]+grady[3]*grady[3]));
            pout += 4;
        }
    }
    
    LOG_INFO("Normal gradient execution time: " << timer.getLastElapsedTime() << "s");
    
    mResultWindow->showImage(mResultImage);
}

void ImageProcessingDialog::runOpenCLSobel()
{
	Timer timer;
	timer.start();

	if (!this->loadSource()) {
		return;
	}

	cl_int retCode;

	// Pass 1: do sobel filtering
	int imageWidth = mSourceImage.width();
	int imageHeight = mSourceImage.height();
	retCode = clSetKernelArg(mSobelKernel, 0, sizeof(cl_mem), &(mSobelMiddleResult));
	retCode |= clSetKernelArg(mSobelKernel, 1, sizeof(cl_mem), &(mImageDataIn));
	retCode |= clSetKernelArg(mSobelKernel, 2, sizeof(int), &(imageWidth));
	retCode |= clSetKernelArg(mSobelKernel, 3, sizeof(int), &(imageHeight));
	if (retCode != CL_SUCCESS) {
		ERROR_MESSAGE("Set sobel kernel arguments failed! Error code = " << retCode);
		return;
	}

    size_t globalDim[2] = { static_cast<size_t>(imageWidth), static_cast<size_t>(imageHeight) };
	retCode = clEnqueueNDRangeKernel(mCommandQueue, mSobelKernel,
		2, NULL, globalDim, NULL, 0, NULL, NULL);
	if (retCode != CL_SUCCESS) {
		ERROR_MESSAGE("Enqueue sobel kernel failed! Error code = " << retCode);
		return;
	}

	// Pass 2: get min & max pixel value
	unsigned int pixelNum = imageWidth * imageHeight;
	retCode = clSetKernelArg(mMinMaxKernel, 0, sizeof(cl_mem), &(mMinValues));
	retCode |= clSetKernelArg(mMinMaxKernel, 1, sizeof(cl_mem), &(mMaxValues));
	retCode |= clSetKernelArg(mMinMaxKernel, 2, sizeof(cl_mem), &(mSobelMiddleResult));
	retCode |= clSetKernelArg(mMinMaxKernel, 3, sizeof(unsigned int), &(pixelNum));
	retCode |= clSetKernelArg(mMinMaxKernel, 4, mMinMaxThreadNumPerBlock * sizeof(float), NULL);
	retCode |= clSetKernelArg(mMinMaxKernel, 5, mMinMaxThreadNumPerBlock * sizeof(float), NULL);
	if (retCode != CL_SUCCESS) {
		ERROR_MESSAGE("Set minmax kernel arguments failed! Error code = " << retCode);
		return;
	}

	globalDim[0] = mMinMaxBlockNum * mMinMaxThreadNumPerBlock;
	size_t localDim[1] = { mMinMaxThreadNumPerBlock };
	retCode = clEnqueueNDRangeKernel(mCommandQueue, mMinMaxKernel,
		1, NULL, globalDim, localDim, 0, NULL, NULL);
	if (retCode != CL_SUCCESS) {
		ERROR_MESSAGE("Enqueue minmax kernel failed! Error code = " << retCode);
		return;
	}

	// copy min/max values from device to host
	std::vector<float> minValues(mMinMaxBlockNum, 0);
	retCode = clEnqueueReadBuffer(mCommandQueue, mMinValues, CL_TRUE, 0, mMinMaxBlockNum * sizeof(float), minValues.data(), 0, NULL, NULL);
	if (retCode != CL_SUCCESS) {
		ERROR_MESSAGE("Enqueue read min values failed! Error code = " << retCode);
		return;
	}
	std::vector<float> maxValues(mMinMaxBlockNum, 0);
	retCode = clEnqueueReadBuffer(mCommandQueue, mMaxValues, CL_TRUE, 0, mMinMaxBlockNum * sizeof(float), maxValues.data(), 0, NULL, NULL);
	if (retCode != CL_SUCCESS) {
		ERROR_MESSAGE("Enqueue read min values failed! Error code = " << retCode);
		return;
	}

	float minVal, maxVal;
	minVal = minValues[0];
	maxVal = maxValues[0];
	for (size_t i = 1; i < mMinMaxBlockNum; ++i) {
		if (minValues[i] < minVal) minVal = minValues[i];
		if (maxValues[i] > maxVal) maxVal = maxValues[i];
	}

	// Pass 3: scale pixel to [0, 255]
	retCode = clSetKernelArg(mScalePixelsKernel, 0, sizeof(cl_mem), &(mImageDataOut));
	retCode |= clSetKernelArg(mScalePixelsKernel, 1, sizeof(cl_mem), &(mSobelMiddleResult));
	retCode |= clSetKernelArg(mScalePixelsKernel, 2, sizeof(unsigned int), &(pixelNum));
	retCode |= clSetKernelArg(mScalePixelsKernel, 3, sizeof(float), &(minVal));
	retCode |= clSetKernelArg(mScalePixelsKernel, 4, sizeof(float), &(maxVal));
	if (retCode != CL_SUCCESS) {
		ERROR_MESSAGE("Set scale pixels kernel arguments failed! Error code = " << retCode);
		return;
	}

	globalDim[0] = pixelNum;
	retCode = clEnqueueNDRangeKernel(mCommandQueue, mScalePixelsKernel,
		1, NULL, globalDim, NULL, 0, NULL, NULL);
	if (retCode != CL_SUCCESS) {
		ERROR_MESSAGE("Enqueue scale pixels kernel failed! Error code = " << retCode);
		return;
	}

	if (!this->storeResult()) {
		return;
	}

	clFinish(mCommandQueue);
	LOG_INFO("OpenCL sobel execution time: " << timer.getLastElapsedTime() << "s");

	mResultWindow->showImage(mResultImage);

	LOG_INFO("Minimum pixel value: " << minVal);
	LOG_INFO("Maximum pixel value: " << maxVal);
}

/* Definition of Sobel filter in horizontal direction */
int const sobel_weight_x[3][3] = {
	{ -1, 0, 1 },
	{ -2, 0, 2 },
	{ -1, 0, 1 }
};

/* Definition of Sobel filter in vertical direction */
int const sobel_weight_y[3][3] = {
	{ -1, -2, -1 },
	{ 0, 0, 0 },
	{ 1, 2, 1 }
};

void ImageProcessingDialog::runNormalSobel()
{
	Timer timer;
	timer.start();

	int img_width = mSourceImage.width();
	int img_height = mSourceImage.height();
	int line_bytes = mSourceImage.bytesPerLine();
	uchar const *data_in = mSourceImage.bits();

	// Pass 1: do sobel filtering
	std::vector<float> data_middle(img_width*img_height, 0);
	for (int y = 0; y < img_height; ++y) {
		for (int x = 0; x < img_width; ++x) {
			float temp = 0.0f;
			float hSum[3] = { 0.0f, 0.0f, 0.0f };
			float vSum[3] = { 0.0f, 0.0f, 0.0f };

			int pixOffset = (y - 1) * img_width + x - 1;

			// NW
			if (x > 0 && y > 0) {
				hSum[0] += (float)data_in[pixOffset*4+0];    // horizontal gradient of Red
				hSum[1] += (float)data_in[pixOffset*4+1];    // horizontal gradient of Green
				hSum[2] += (float)data_in[pixOffset*4+2];    // horizontal gradient of Blue
				vSum[0] -= (float)data_in[pixOffset*4+0];    // vertical gradient of Red
				vSum[1] -= (float)data_in[pixOffset*4+1];    // vertical gradient of Green
				vSum[2] -= (float)data_in[pixOffset*4+2];    // vertical gradient of Blue   
			}
			pixOffset++;

			// N
			if (y > 0) {
				vSum[0] -= (float)(data_in[pixOffset*4+0] << 1);  // vertical gradient of Red
				vSum[1] -= (float)(data_in[pixOffset*4+1] << 1);  // vertical gradient of Green
				vSum[2] -= (float)(data_in[pixOffset*4+2] << 1);  // vertical gradient of Blue
			}
			pixOffset++;

			// NE
			if (y > 0 && x < img_width - 1) {
				hSum[0] -= (float)data_in[pixOffset*4+0];    // horizontal gradient of Red
				hSum[1] -= (float)data_in[pixOffset*4+1];    // horizontal gradient of Green
				hSum[2] -= (float)data_in[pixOffset*4+2];    // horizontal gradient of Blue
				vSum[0] -= (float)data_in[pixOffset*4+0];    // vertical gradient of Red
				vSum[1] -= (float)data_in[pixOffset*4+1];    // vertical gradient of Green
				vSum[2] -= (float)data_in[pixOffset*4+2];    // vertical gradient of Blue
			}
			pixOffset += img_width - 2;

			// W
			if (x > 0) {
				hSum[0] += (float)(data_in[pixOffset*4+0] << 1);  // vertical gradient of Red
				hSum[1] += (float)(data_in[pixOffset*4+1] << 1);  // vertical gradient of Green
				hSum[2] += (float)(data_in[pixOffset*4+2] << 1);  // vertical gradient of Blue
			}
			pixOffset++;

			// C
			pixOffset++;

			// E
			if (x < img_width - 1) {
				hSum[0] -= (float)(data_in[pixOffset*4+0] << 1);  // vertical gradient of Red
				hSum[1] -= (float)(data_in[pixOffset*4+1] << 1);  // vertical gradient of Green
				hSum[2] -= (float)(data_in[pixOffset*4+2] << 1);  // vertical gradient of Blue
			}
			pixOffset += img_width - 2;

			// SW
			if (x > 0 && y < img_height - 1) {
				hSum[0] += (float)data_in[pixOffset*4+0];    // horizontal gradient of Red
				hSum[1] += (float)data_in[pixOffset*4+1];    // horizontal gradient of Green
				hSum[2] += (float)data_in[pixOffset*4+2];    // horizontal gradient of Blue
				vSum[0] += (float)data_in[pixOffset*4+0];    // vertical gradient of Red
				vSum[1] += (float)data_in[pixOffset*4+1];    // vertical gradient of Green
				vSum[2] += (float)data_in[pixOffset*4+2];    // vertical gradient of Blue
			}
			pixOffset++;

			// S
			if (y < img_height - 1) {
				vSum[0] += (float)(data_in[pixOffset*4+0] << 1);  // vertical gradient of Red
				vSum[1] += (float)(data_in[pixOffset*4+1] << 1);  // vertical gradient of Green
				vSum[2] += (float)(data_in[pixOffset*4+2] << 1);  // vertical gradient of Blue
			}
			pixOffset++;

			// SE
			if (x < img_width - 1 && y < img_height - 1) {
				hSum[0] -= (float)data_in[pixOffset*4+0];    // horizontal gradient of Red
				hSum[1] -= (float)data_in[pixOffset*4+1];    // horizontal gradient of Green
				hSum[2] -= (float)data_in[pixOffset*4+2];    // horizontal gradient of Blue
				vSum[0] += (float)data_in[pixOffset*4+0];    // vertical gradient of Red
				vSum[1] += (float)data_in[pixOffset*4+1];    // vertical gradient of Green
				vSum[2] += (float)data_in[pixOffset*4+2];    // vertical gradient of Blue
			}

			// Weighted combination of Root-Sum-Square per-color-band H & V gradients for each of RGB
			temp = 0.30f * sqrt((hSum[0] * hSum[0]) + (vSum[0] * vSum[0]));
			temp += 0.55f * sqrt((hSum[1] * hSum[1]) + (vSum[1] * vSum[1]));
			temp += 0.15f * sqrt((hSum[2] * hSum[2]) + (vSum[2] * vSum[2]));

			int outIdx = y * img_width + x;
			data_middle[outIdx] = temp;
		}
	}

	// Pass 2: get min & max pixel value
	float minVal, maxVal;
	minVal = data_middle[0];
	maxVal = data_middle[0];
	for (size_t i = 1; i < img_width*img_height; ++i) {
		if (data_middle[i] < minVal) minVal = data_middle[i];
		if (data_middle[i] > maxVal) maxVal = data_middle[i];
	}

	// Pass 3: scale pixel to [0, 255]
	uchar *data_out = mResultImage.bits();
	for (size_t i = 0; i < img_width*img_height; ++i) {
		float temp = data_middle[i];
		temp = 255.0f * (temp - minVal) / (maxVal - minVal);
		uchar *pout = data_out + i*4;
		pout[0] = pout[1] = pout[2] = (uchar)(temp);
		pout[3] = 255;
	}

	LOG_INFO("Normal sobel execution time: " << timer.getLastElapsedTime() << "s");

	mResultWindow->showImage(mResultImage);

	LOG_INFO("Minimum pixel value: " << minVal);
	LOG_INFO("Maximum pixel value: " << maxVal);
}

bool ImageProcessingDialog::loadSource()
{
    if (!this->buildOCLMemoryObjects()) {
        return false;
    }

    cl_int retCode;
    if (ui->checkBoxInputUseImage->isChecked()) {
        size_t const origin[3] = { 0, 0, 0 };
        size_t const region[3] = { static_cast<size_t>(mSourceImage.width()), static_cast<size_t>(mSourceImage.height()), 1 };
        retCode = clEnqueueWriteImage(mCommandQueue, mImageIn,
            CL_TRUE, origin, region,
            mSourceImage.bytesPerLine(), 0,
            mSourceImage.bits(), 0, NULL, NULL);
        if (retCode != CL_SUCCESS) {
            ERROR_MESSAGE("Load input image failed! Error code = " << retCode);
            LOG_INFO("Size: " << region[0] << " x " << region[1]);        
            return false;
        }

    } else {
        retCode = clEnqueueWriteBuffer(mCommandQueue, mImageDataIn,
            CL_TRUE, 0, mImageBytes, mSourceImage.bits(), 0, NULL, NULL);
        if (retCode != CL_SUCCESS) {
            ERROR_MESSAGE("Write source image to OpenCL memory object failed! Error code = " << retCode);
            return false;
        }
    }

    return true;
}

bool ImageProcessingDialog::storeResult()
{
    cl_int retCode;
    retCode = clEnqueueReadBuffer(mCommandQueue, mImageDataOut,
        CL_TRUE, 0, mImageBytes, mResultImage.bits(), 0, NULL, NULL);
    if (retCode != CL_SUCCESS) {
        ERROR_MESSAGE("Read result image from OpenCL memory object failed! Error code = " << retCode);
        return false;
    }

    return true;
}

void ImageProcessingDialog::saveResultFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Result Image", QString(), "Images (*.png)");
    if (fileName.isEmpty()) return;
    if (!mResultImage.save(fileName)) {
        LOG_ERROR("Save result image failed!");
    } else {
        LOG_INFO("Result image successfully saved!");
    }
}
