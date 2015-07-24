#include "morphology.h"
#include <QPixmap>

morphology::morphology(QWidget *parent, Qt::WFlags flags)
    :QWidget(parent, flags),
    ui(new Ui::morphology)
{
    ui->setupUi(this);

    //��ʼ��
    iteration = 1;//Ĭ�ϵ���1�Σ� ��������������Ϊ20
    shape = CV_SHAPE_RECT;//Ĭ��ʹ�� 3 x 3 ���νṹԪ��
    kernelColumns = 3;
    kernelRows = 3;

    initUI();//��ʼ������

    //��������
    createConnect();
}

//��morphology��֧�ֻҶ�ͼ����ֵͼ��̬ѧ����
void morphology::initUI()
{
    ui->rectSE->setChecked(true);//Ĭ��ѡ����νṹԪ��
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
    setKernelShape();//���ú���״
    inputImage = cvLoadImage(inputFileName, 0);//QString to char*�� ���ػҶ�ͼ��
    outputImage = cvCreateImage(cvGetSize(inputImage), inputImage->depth, inputImage->nChannels);
    //�Զ���ṹԪ�أ��ˣ�
    se = cvCreateStructuringElementEx(kernelColumns, kernelRows,  kernelColumns / 2, kernelRows / 2, shape);//�Է���Ԫ�ش����Ľ�����ڽṹԪ���м�
    cvDilate(inputImage, outputImage, se, iteration);//���ĸ��������õ�������

    outputFileName = "../data/temp/fourierTransform.jpg";//char*
    cvSaveImage(outputFileName, outputImage);//char*
    //�ͷ���Դ���ڳ��ڴ�
    cvReleaseImage(&inputImage);

    emit returnOutputFileName(outputFileName);//�����ź�
}

void morphology::on_erosionButton_clicked()
{
    setKernelShape();//���ú���״
    inputImage = cvLoadImage(inputFileName, 0);//QString to char*
    outputImage = cvCreateImage(cvGetSize(inputImage), inputImage->depth, inputImage->nChannels);
    //�Զ���ṹԪ��
    se = cvCreateStructuringElementEx(kernelColumns, kernelRows,  kernelColumns / 2, kernelRows / 2, shape);
    cvErode(inputImage, outputImage, se, iteration);//���ĸ��������õ�������

    outputFileName = "../data/temp/fourierTransform.jpg";//char*
    cvSaveImage(outputFileName, outputImage);//char*
    //�ͷ���Դ���ڳ��ڴ�
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);//�����ź�
}

void morphology::on_openButton_clicked()
{
    setKernelShape();//���ú���״
    inputImage = cvLoadImage(inputFileName, 0);//QString to char*
    outputImage = cvCreateImage(cvGetSize(inputImage), inputImage->depth, inputImage->nChannels);
    //�Զ���ṹԪ��
    CvMat* temp = cvCreateMat(inputImage->height, inputImage->width, CV_8UC1);//�Զ�ֵͼ����
    se = cvCreateStructuringElementEx(kernelColumns, kernelRows,  kernelColumns / 2, kernelRows / 2, shape);
    cvMorphologyEx(inputImage, outputImage, temp, se, CV_MOP_OPEN, iteration);

    outputFileName = "../data/temp/fourierTransform.jpg";//char*
    cvSaveImage(outputFileName, outputImage);//char*
    //�ͷ���Դ���ڳ��ڴ�
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);//�����ź�
}

void morphology::on_closeButton_clicked()
{
    setKernelShape();//���ú���״
    inputImage = cvLoadImage(inputFileName, 0);//QString to char*
    outputImage = cvCreateImage(cvGetSize(inputImage), inputImage->depth, inputImage->nChannels);
    //�Զ���ṹԪ��
    CvMat* temp = cvCreateMat(inputImage->height, inputImage->width, CV_8UC1);//�Զ�ֵͼ����
    se = cvCreateStructuringElementEx(kernelColumns, kernelRows,  kernelColumns / 2, kernelRows / 2, shape);
    cvMorphologyEx(inputImage, outputImage, temp, se, CV_MOP_CLOSE, iteration);

    outputFileName = "../data/temp/fourierTransform.jpg";//char*
    cvSaveImage(outputFileName, outputImage);//char*
    //�ͷ���Դ���ڳ��ڴ�
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);//�����ź�
}

void morphology::on_hatButton_clicked()
{
    setKernelShape();//���ú���״
    inputImage = cvLoadImage(inputFileName, 0);//QString to char*
    outputImage = cvCreateImage(cvGetSize(inputImage), inputImage->depth, inputImage->nChannels);
    //�Զ���ṹԪ��
    CvMat* temp = cvCreateMat(inputImage->height, inputImage->width, CV_8UC1);//�Զ�ֵͼ����
    se = cvCreateStructuringElementEx(kernelColumns, kernelRows,  kernelColumns / 2, kernelRows / 2, shape);
    cvMorphologyEx(inputImage, outputImage, temp, se, CV_MOP_TOPHAT, iteration);

    outputFileName = "../data/temp/fourierTransform.jpg";//char*
    cvSaveImage(outputFileName, outputImage);//char*
    //�ͷ���Դ���ڳ��ڴ�
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);//�����ź�
}

void morphology::on_blackHatButton_clicked()
{
    setKernelShape();//���ú���״
    inputImage = cvLoadImage(inputFileName, 0);//QString to char*
    outputImage = cvCreateImage(cvGetSize(inputImage), inputImage->depth, inputImage->nChannels);
    //�Զ���ṹԪ��
    CvMat* temp = cvCreateMat(inputImage->height, inputImage->width, CV_8UC1);//�Զ�ֵͼ����
    se = cvCreateStructuringElementEx(kernelColumns, kernelRows,  kernelColumns / 2, kernelRows / 2, shape);
    cvMorphologyEx(inputImage, outputImage, temp, se, CV_MOP_BLACKHAT, iteration);

    outputFileName = "../data/temp/fourierTransform.jpg";//char*
    cvSaveImage(outputFileName, outputImage);//char*
    //�ͷ���Դ���ڳ��ڴ�
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);//�����ź�
}

void morphology::on_resetButton_clicked()
{
    resetControlPanel();//��mainwindowInterator.cpp�б�����
    emit reset();//��������������ͼ
}

morphology::~morphology()
{
    delete ui;
}