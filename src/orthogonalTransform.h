#ifndef ORTHOGONALTRANSFORM_H
#define ORTHOGONALTRANSFORM_H

#include <QtGui/QMainWindow>
#include "ui_orthogonalTransform.h"
#include "highgui.h"
#include "cv.h"
#include <QPixmap>

//定义基类
class orthogonalTransform : public QWidget
{
    Q_OBJECT

public:
    orthogonalTransform(QWidget *parent = 0, Qt::WFlags flags = 0);//构造函数
    ~orthogonalTransform();//析构函数

    //自定义成员变量
public:
    char* outputFileName;
    //OpenCV
    IplImage *inputImage, *outputImage;

    //自定义成员函数
private:

signals:
       void returnOutputFileName(char* fileName);

    private slots:
};

//每种操作对应一个类，真正实现面向对象编程
class fourierTransform : public orthogonalTransform
{
    Q_OBJECT

        //子类用默认构造函数

        //自定义成员函数
public:
    void discreteFourierTrans(char* fileName);
};
#endif // ORTHOGONALTRANSFORM_H
