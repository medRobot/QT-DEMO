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
    pointOperation(QWidget *parent = 0, Qt::WFlags flags = 0);//���캯��
    ~pointOperation();//��������

    //�Զ����Ա����
private:
    char* outputFileName;
    //OpenCV
    IplImage *inputImage, *outputImage;

    //�Զ����Ա����
public:
    //����OpenCV��API�ӿ�
    void color2Gray(char* fileName);
    void interpolation(char* fileName);
    void colorHistgram(char* fileName);
    void grayHistgram(char* fileName);
    void histgramEqualization(char* fileName);
    void getSingleChannelImg(char* fileName);

    //�Զ����ź�&��
signals:
    void returnOutputFileName(char* fileName);
};

#endif // POINTOPERATION_H
