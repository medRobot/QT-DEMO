#include "morphology.h"
#include <QPixmap>

morphology::morphology(QWidget *parent, Qt::WFlags flags)
    :QWidget(parent, flags),
    ui(new Ui::morphology)
{
    ui->setupUi(this);

    //初始化
    iteration = 1;//默认迭代1次， 设置最大迭代次数为20
    shape = CV_SHAPE_RECT;//默认使用 3 x 3 矩形结构元素
    kernelColumns = 3;
    kernelRows = 3;

    initUI();//初始化界面

    //创建连接
    createConnect();
}

//该morphology类支持灰度图，二值图形态学处理
void morphology::initUI()
{
    ui->rectSE->setChecked(true);//默认选择矩形结构元素
    ui->iterationSlider->setValue(1);
    ui->iterationSpinBox->setValue(1);
    ui->kernelColumnSpinBox->setValue(3);
    ui->kernelRowSpinBox->setValue(3);
}

void morphology::createConnect()
{
    connect(ui->iterationSlider, SIGNAL(valueChanged(int)), ui->iterationSpinBox, SLOT(setValue(int)));
    connect(ui->iterationSpinBox, SIGNAL(valueChanged(int)), ui->iterationSlider, SLOT(setValue(int)));
}

void morphology::setKernelShape()
{
    if(ui->rectSE->isChecked())
    {
        shape = CV_SHAPE_RECT;
    }
    if(ui->crossSE->isChecked())
    {
        shape = CV_SHAPE_CROSS;
    }
    if(ui->ellipseSE->isChecked())
    {
        shape = CV_SHAPE_ELLIPSE;
    }
}

void morphology::resetControlPanel()
{
    initUI();
    iteration = 1;
    shape = CV_SHAPE_RECT;
}

void morphology::on_iterationSlider_valueChanged(int value)
{
    iteration = value;
}

void morphology::on_kernelColumnSpinBox_valueChanged(int value)
{
    kernelColumns = value;
}

void morphology::on_kernelRowSpinBox_valueChanged(int value)
{
    kernelRows = value;
}

void morphology::on_dilationButton_clicked()
{
    setKernelShape();//设置核形状
    inputImage = cvLoadImage(inputFileName, 0);//QString to char*， 加载灰度图像
    outputImage = cvCreateImage(cvGetSize(inputImage), inputImage->depth, inputImage->nChannels);
    //自定义结构元素（核）
    se = cvCreateStructuringElementEx(kernelColumns, kernelRows,  kernelColumns / 2, kernelRows / 2, shape);//对非零元素处理后的结果放在结构元素中间
    cvDilate(inputImage, outputImage, se, iteration);//第四个参数设置迭代次数

    outputFileName = "../data/temp/fourierTransform.jpg";//char*
    cvSaveImage(outputFileName, outputImage);//char*
    //释放资源，腾出内存
    cvReleaseImage(&inputImage);

    emit returnOutputFileName(outputFileName);//发出信号
}

void morphology::on_erosionButton_clicked()
{
    setKernelShape();//设置核形状
    inputImage = cvLoadImage(inputFileName, 0);//QString to char*
    outputImage = cvCreateImage(cvGetSize(inputImage), inputImage->depth, inputImage->nChannels);
    //自定义结构元素
    se = cvCreateStructuringElementEx(kernelColumns, kernelRows,  kernelColumns / 2, kernelRows / 2, shape);
    cvErode(inputImage, outputImage, se, iteration);//第四个参数设置迭代次数

    outputFileName = "../data/temp/fourierTransform.jpg";//char*
    cvSaveImage(outputFileName, outputImage);//char*
    //释放资源，腾出内存
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);//发出信号
}

void morphology::on_openButton_clicked()
{
    setKernelShape();//设置核形状
    inputImage = cvLoadImage(inputFileName, 0);//QString to char*
    outputImage = cvCreateImage(cvGetSize(inputImage), inputImage->depth, inputImage->nChannels);
    //自定义结构元素
    CvMat* temp = cvCreateMat(inputImage->height, inputImage->width, CV_8UC1);//对二值图操作
    se = cvCreateStructuringElementEx(kernelColumns, kernelRows,  kernelColumns / 2, kernelRows / 2, shape);
    cvMorphologyEx(inputImage, outputImage, temp, se, CV_MOP_OPEN, iteration);

    outputFileName = "../data/temp/fourierTransform.jpg";//char*
    cvSaveImage(outputFileName, outputImage);//char*
    //释放资源，腾出内存
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);//发出信号
}

void morphology::on_closeButton_clicked()
{
    setKernelShape();//设置核形状
    inputImage = cvLoadImage(inputFileName, 0);//QString to char*
    outputImage = cvCreateImage(cvGetSize(inputImage), inputImage->depth, inputImage->nChannels);
    //自定义结构元素
    CvMat* temp = cvCreateMat(inputImage->height, inputImage->width, CV_8UC1);//对二值图操作
    se = cvCreateStructuringElementEx(kernelColumns, kernelRows,  kernelColumns / 2, kernelRows / 2, shape);
    cvMorphologyEx(inputImage, outputImage, temp, se, CV_MOP_CLOSE, iteration);

    outputFileName = "../data/temp/fourierTransform.jpg";//char*
    cvSaveImage(outputFileName, outputImage);//char*
    //释放资源，腾出内存
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);//发出信号
}

void morphology::on_hatButton_clicked()
{
    setKernelShape();//设置核形状
    inputImage = cvLoadImage(inputFileName, 0);//QString to char*
    outputImage = cvCreateImage(cvGetSize(inputImage), inputImage->depth, inputImage->nChannels);
    //自定义结构元素
    CvMat* temp = cvCreateMat(inputImage->height, inputImage->width, CV_8UC1);//对二值图操作
    se = cvCreateStructuringElementEx(kernelColumns, kernelRows,  kernelColumns / 2, kernelRows / 2, shape);
    cvMorphologyEx(inputImage, outputImage, temp, se, CV_MOP_TOPHAT, iteration);

    outputFileName = "../data/temp/fourierTransform.jpg";//char*
    cvSaveImage(outputFileName, outputImage);//char*
    //释放资源，腾出内存
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);//发出信号
}

void morphology::on_blackHatButton_clicked()
{
    setKernelShape();//设置核形状
    inputImage = cvLoadImage(inputFileName, 0);//QString to char*
    outputImage = cvCreateImage(cvGetSize(inputImage), inputImage->depth, inputImage->nChannels);
    //自定义结构元素
    CvMat* temp = cvCreateMat(inputImage->height, inputImage->width, CV_8UC1);//对二值图操作
    se = cvCreateStructuringElementEx(kernelColumns, kernelRows,  kernelColumns / 2, kernelRows / 2, shape);
    cvMorphologyEx(inputImage, outputImage, temp, se, CV_MOP_BLACKHAT, iteration);

    outputFileName = "../data/temp/fourierTransform.jpg";//char*
    cvSaveImage(outputFileName, outputImage);//char*
    //释放资源，腾出内存
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);//发出信号
}

void morphology::on_resetButton_clicked()
{
    resetControlPanel();//在mainwindowInterator.cpp中被调用
    emit reset();//在主窗口重置视图
}

morphology::~morphology()
{
    delete ui;
}