#ifndef ITKFILTER_H
#define ITKFILTER_H

#include <QtGui/QMainWindow>
#include <QPixmap>
#include "ui_itkFilter.h"
#include "highgui.h"
#include "cv.h"

class itkFilter : public QWidget
{
    Q_OBJECT

public:
    itkFilter(QWidget *parent = 0, Qt::WFlags flags = 0);//���캯��
    ~itkFilter();//��������

    //�Զ����Ա����
private:
    char outputFileName[100];//string to char*���տ�ʼ�Ϳ��ٿռ䲻�Ǻܺ���

signals:
   void returnOutputFileName(char* fileName);

public:
    //�Զ����Ա����
    void meanFilter(char* fileName, int dimension, int pixelType);
    void gaussianFilter(char* fileName, int dimension, int pixelType);
    void bilateralFilter(char* fileName, int dimension, int pixelType);
    void laplacianFilter(char* fileName, int dimension, int pixelType);
    void curvatureAnisotropicDiffusionFilter(char* fileName, int dimension, int pixelType);
};

#endif // ITKFILTER_H
