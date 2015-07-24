#ifndef EDGECONTOUR_H
#define EDGECONTOUR_H

#include <QtGui/QMainWindow>
#include <QPixmap>
#include "ui_edge_contour.h"
#include "highgui.h"
#include "cv.h"

class edgeContour : public QWidget
{
    Q_OBJECT

public:
    edgeContour(QWidget *parent = 0, Qt::WFlags flags = 0);//构造函数
    ~edgeContour();//析构函数

    //自定义成员变量
private:
    char* outputFileName;
    char* inputFileName;
    QByteArray ba;   
    //OpenCV
    IplImage *inputImage, *outputImage;

    //自定义成员函数
public:
    void sobelOperator(char* fileName);

    //自定义信号&槽
signals:
    void returnOutputFileName(char* fileName);
};

#endif // EDGECONTOUR_H
