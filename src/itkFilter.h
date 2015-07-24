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
    itkFilter(QWidget *parent = 0, Qt::WFlags flags = 0);//构造函数
    ~itkFilter();//析构函数

    //自定义成员变量
private:
    char outputFileName[100];//string to char*，刚开始就开辟空间不是很合适

signals:
   void returnOutputFileName(char* fileName);

public:
    //自定义成员函数
    void meanFilter(char* fileName, int dimension, int pixelType);
    void gaussianFilter(char* fileName, int dimension, int pixelType);
    void bilateralFilter(char* fileName, int dimension, int pixelType);
    void laplacianFilter(char* fileName, int dimension, int pixelType);
    void curvatureAnisotropicDiffusionFilter(char* fileName, int dimension, int pixelType);
};

#endif // ITKFILTER_H
