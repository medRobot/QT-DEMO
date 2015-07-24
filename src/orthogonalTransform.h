#ifndef ORTHOGONALTRANSFORM_H
#define ORTHOGONALTRANSFORM_H

#include <QtGui/QMainWindow>
#include "ui_orthogonalTransform.h"
#include "highgui.h"
#include "cv.h"
#include <QPixmap>

//�������
class orthogonalTransform : public QWidget
{
    Q_OBJECT

public:
    orthogonalTransform(QWidget *parent = 0, Qt::WFlags flags = 0);//���캯��
    ~orthogonalTransform();//��������

    //�Զ����Ա����
public:
    char* outputFileName;
    //OpenCV
    IplImage *inputImage, *outputImage;

    //�Զ����Ա����
private:

signals:
       void returnOutputFileName(char* fileName);

    private slots:
};

//ÿ�ֲ�����Ӧһ���࣬����ʵ�����������
class fourierTransform : public orthogonalTransform
{
    Q_OBJECT

        //������Ĭ�Ϲ��캯��

        //�Զ����Ա����
public:
    void discreteFourierTrans(char* fileName);
};
#endif // ORTHOGONALTRANSFORM_H
