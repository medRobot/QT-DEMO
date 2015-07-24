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
    edgeContour(QWidget *parent = 0, Qt::WFlags flags = 0);//���캯��
    ~edgeContour();//��������

    //�Զ����Ա����
private:
    char* outputFileName;
    char* inputFileName;
    QByteArray ba;   
    //OpenCV
    IplImage *inputImage, *outputImage;

    //�Զ����Ա����
public:
    void sobelOperator(char* fileName);

    //�Զ����ź�&��
signals:
    void returnOutputFileName(char* fileName);
};

#endif // EDGECONTOUR_H
