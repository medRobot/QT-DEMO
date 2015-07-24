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
    morphology(QWidget *parent = 0, Qt::WFlags flags = 0);//构造函数
    ~morphology();//析构函数

    Ui::morphology* ui;

    //自定义成员变量
private:
   char* outputFileName;
    IplImage* inputImage, *outputImage;
    IplConvKernel* se;//自定义结构元素
    int iteration;//迭代次数
    int shape;//结构元素形状
    int kernelColumns, kernelRows;

public:
     char* inputFileName;//在mainwindowInterator.cpp中被调用

    //自定义成员函数
private:
    void createConnect();
    void initUI();
    void setKernelShape();
public:
    void resetControlPanel();//重置控制面板，在mainwindowInterator.cpp中被调用

signals:
    //向主窗口发送信号
    void returnOutputFileName(char* fileName);
    void reset();

    private slots:
        //按钮
        void on_dilationButton_clicked();
        void on_erosionButton_clicked();
        void on_openButton_clicked();
        void on_closeButton_clicked();
        void on_hatButton_clicked();
        void on_blackHatButton_clicked();
        void on_resetButton_clicked();
        //滑条
        void on_iterationSlider_valueChanged(int value);
        //spinBox
        void on_kernelColumnSpinBox_valueChanged(int value);
        void on_kernelRowSpinBox_valueChanged(int value);
};

#endif // MORPHOLOGY_H
