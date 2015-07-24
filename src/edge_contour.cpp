#include "edge_contour.h"

edgeContour::edgeContour(QWidget *parent , Qt::WFlags flags)
    :QWidget(parent, flags)
{

}

void edgeContour::sobelOperator(char* fileName)
{
    inputImage = cvLoadImage(fileName, 0);//���ػҶ�ͼ
    //���Դͼ��Ϊ8λ��Ϊ���������Ŀ��ͼ�����Ϊ IPL_DEPTH_16S
    outputImage = cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_16S, 1);
    cvSobel(inputImage, outputImage, 1, 1, 3);//x��������y���������׾���С
    outputFileName = ("../data/temp/sobelEdgeDetection.jpg");
    cvSaveImage(outputFileName, outputImage);
    //�ͷ���Դ���ڳ��ڴ�
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);//���ݲ�����ָ��
}

edgeContour::~edgeContour()
{

}