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
