#ifndef POINTOPERATION_H
#define POINTOPERATION_H

#include <QtGui/QMainWindow>
#include <QPixmap>

#include "ui_pointOperation.h"
#include "highgui.h"
#include "cv.h"

class pointOperation : public QWidget
{
    Q_OBJECT

public:
    pointOperation(QWidget *parent = 0, Qt::WFlags flags = 0);//构造函数
    ~pointOperation();//析构函数

    //自定义成员变量
private:
    char* outputFileName;
    //OpenCV
    IplImage *inputImage, *outputImage;

    //自定义成员函数
public:
    //调用OpenCV中API接口
    void color2Gray(char* fileName);
    void interpolation(char* fileName);
    void colorHistgram(char* fileName);
    void grayHistgram(char* fileName);
    void histgramEqualization(char* fileName);
    void getSingleChannelImg(char* fileName);

    //自定义信号&槽
signals:
    void returnOutputFileName(char* fileName);
};

#endif // POINTOPERATION_H
