#ifndef GEOMETRYTRANSFORM_H
#define GEOMETRYTRANSFORM_H

#include <QtGui/QMainWindow>
#include "ui_geometryTransform.h"
#include <QPixmap>

class geometryTransform : public QWidget
{
    Q_OBJECT

public:
    geometryTransform(QWidget *parent = 0, Qt::WFlags flags = 0);//构造函数
    ~geometryTransform();//析构函数

    //枚举各控件
    enum{
        Xslider_scale = 1,  Yslider_scale, Xslider_rotate,  Yslider_rotate, 
    };

    //自定义成员变量
public:
    Ui::geometryTransform* ui;
    QPixmap* pix;
    QString* inputFileName;
    char* outputFileName;

private:
    int xFactor, yFactor, rotateFactor;//接收从控件传递的数值

    //自定义成员函数
private:
    void initUI();
    void createConnect();
public:
    void resetControlPanel();//重置控制面板，在mainwindowInterator.cpp中被调用

    //定义槽&信号
signals:
    //向主窗口发送信号
    void returnOutputFileName(char* fileName);
    void reset();

    //一般函数也可以传递数据，通过实例化类的对象来调用函数
    //私有槽不能在其他类中被调用
    private slots:
        //滑动条控件
        void on_Xslider_translate_valueChanged(int value);
        void on_Yslider_translate_valueChanged(int value);
        void on_Xslider_scale_valueChanged(int value);
        void on_Yslider_scale_valueChanged(int value);
        void on_dial_rotate_valueChanged(int value);

        //按钮控件
        void on_translateButton_clicked();
        void on_scaleButton_clicked();
        void on_rotateButton_clicked();
        void on_resetButton_clicked();
};

#endif // GEOMETRYTRANSFORM_H
