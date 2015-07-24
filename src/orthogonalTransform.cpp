#include "orthogonalTransform.h"
#include <QMessageBox>

orthogonalTransform::orthogonalTransform(QWidget *parent , Qt::WFlags flags)
    :QWidget(parent, flags)
{
 
}

//输入输出图必须为浮点类型且为单通道或者双通道
//cvConvertImage(inputImage, temp);//目标图必须为8位图
void fourierTransform::discreteFourierTrans(char* fileName)
{
    inputImage = cvLoadImage(fileName, 0);//输入单通道图
    if(inputImage == NULL || inputImage->nChannels == 3)
    {
        QMessageBox::warning(this, tr("提示信息窗口"), tr("输入图片类型不匹配，请重新输入！"),  QMessageBox::Ok);
        return;
    }
    CvMat* mat = cvCreateMat(inputImage->height, inputImage->width, CV_32FC1);
    for(int j = 0; j < mat->rows; j++)
        for(int i = 0; i < mat->cols; i++)
        {
            cvmSet(mat, j, i, double(inputImage->imageData[j * mat->cols + i]));
        }
    outputImage = cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_32F, 1);//输出图

    cvDFT(mat, outputImage, CV_DXT_FORWARD,  0);//第三个参数现在设置为执行正变化
    
    outputFileName = "../data/temp/fourierTransform.jpg";//QString
    cvSaveImage(outputFileName, outputImage);//char*

    //释放资源，腾出内存
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);//发出信号
}

orthogonalTransform::~orthogonalTransform()
{

}