#include "orthogonalTransform.h"
#include <QMessageBox>

orthogonalTransform::orthogonalTransform(QWidget *parent , Qt::WFlags flags)
    :QWidget(parent, flags)
{
 
}

//�������ͼ����Ϊ����������Ϊ��ͨ������˫ͨ��
//cvConvertImage(inputImage, temp);//Ŀ��ͼ����Ϊ8λͼ
void fourierTransform::discreteFourierTrans(char* fileName)
{
    inputImage = cvLoadImage(fileName, 0);//���뵥ͨ��ͼ
    if(inputImage == NULL || inputImage->nChannels == 3)
    {
        QMessageBox::warning(this, tr("��ʾ��Ϣ����"), tr("����ͼƬ���Ͳ�ƥ�䣬���������룡"),  QMessageBox::Ok);
        return;
    }
    CvMat* mat = cvCreateMat(inputImage->height, inputImage->width, CV_32FC1);
    for(int j = 0; j < mat->rows; j++)
        for(int i = 0; i < mat->cols; i++)
        {
            cvmSet(mat, j, i, double(inputImage->imageData[j * mat->cols + i]));
        }
    outputImage = cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_32F, 1);//���ͼ

    cvDFT(mat, outputImage, CV_DXT_FORWARD,  0);//������������������Ϊִ�����仯
    
    outputFileName = "../data/temp/fourierTransform.jpg";//QString
    cvSaveImage(outputFileName, outputImage);//char*

    //�ͷ���Դ���ڳ��ڴ�
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);//�����ź�
}

orthogonalTransform::~orthogonalTransform()
{

}