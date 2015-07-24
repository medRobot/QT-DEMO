#ifndef MORPHOLOGY_H
#define MORPHOLOGY_H

#include <QtGui/QMainWindow>
#include <QPixmap>

#include "ui_morphology.h"
#include "highgui.h"
#include "cv.h"

class morphology : public QWidget
{
    Q_OBJECT

public:
    morphology(QWidget *parent = 0, Qt::WFlags flags = 0);//���캯��
    ~morphology();//��������

    Ui::morphology* ui;

    //�Զ����Ա����
private:
   char* outputFileName;
    IplImage* inputImage, *outputImage;
    IplConvKernel* se;//�Զ���ṹԪ��
    int iteration;//��������
    int shape;//�ṹԪ����״
    int kernelColumns, kernelRows;

public:
     char* inputFileName;//��mainwindowInterator.cpp�б�����

    //�Զ����Ա����
private:
    void createConnect();
    void initUI();
    void setKernelShape();
public:
    void resetControlPanel();//���ÿ�����壬��mainwindowInterator.cpp�б�����

signals:
    //�������ڷ����ź�
    void returnOutputFileName(char* fileName);
    void reset();

    private slots:
        //��ť
        void on_dilationButton_clicked();
        void on_erosionButton_clicked();
        void on_openButton_clicked();
        void on_closeButton_clicked();
        void on_hatButton_clicked();
        void on_blackHatButton_clicked();
        void on_resetButton_clicked();
        //����
        void on_iterationSlider_valueChanged(int value);
        //spinBox
        void on_kernelColumnSpinBox_valueChanged(int value);
        void on_kernelRowSpinBox_valueChanged(int value);
};

#endif // MORPHOLOGY_H
