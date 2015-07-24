#include "edge_contour.h"

edgeContour::edgeContour(QWidget *parent , Qt::WFlags flags)
    :QWidget(parent, flags)
{

}

void edgeContour::sobelOperator(char* fileName)
{
    inputImage = cvLoadImage(fileName, 0);//加载灰度图
    //如果源图像为8位，为避免溢出，目标图像必须为 IPL_DEPTH_16S
    outputImage = cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_16S, 1);
    cvSobel(inputImage, outputImage, 1, 1, 3);//x方向导数，y方向导数，孔径大小
    outputFileName = ("../data/temp/sobelEdgeDetection.jpg");
    cvSaveImage(outputFileName, outputImage);
    //释放资源，腾出内存
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);//传递参数用指针
}

edgeContour::~edgeContour()
{

}