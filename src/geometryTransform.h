#ifndef GEOMETRYTRANSFORM_H
#define GEOMETRYTRANSFORM_H

#include <QtGui/QMainWindow>
#include "ui_geometryTransform.h"
#include <QPixmap>

class geometryTransform : public QWidget
{
    Q_OBJECT

public:
    geometryTransform(QWidget *parent = 0, Qt::WFlags flags = 0);//���캯��
    ~geometryTransform();//��������

    //ö�ٸ��ؼ�
    enum{
        Xslider_scale = 1,  Yslider_scale, Xslider_rotate,  Yslider_rotate, 
    };

    //�Զ����Ա����
public:
    Ui::geometryTransform* ui;
    QPixmap* pix;
    QString* inputFileName;
    char* outputFileName;

private:
    int xFactor, yFactor, rotateFactor;//���մӿؼ����ݵ���ֵ

    //�Զ����Ա����
private:
    void initUI();
    void createConnect();
public:
    void resetControlPanel();//���ÿ�����壬��mainwindowInterator.cpp�б�����

    //�����&�ź�
signals:
    //�������ڷ����ź�
    void returnOutputFileName(char* fileName);
    void reset();

    //һ�㺯��Ҳ���Դ������ݣ�ͨ��ʵ������Ķ��������ú���
    //˽�в۲������������б�����
    private slots:
        //�������ؼ�
        void on_Xslider_translate_valueChanged(int value);
        void on_Yslider_translate_valueChanged(int value);
        void on_Xslider_scale_valueChanged(int value);
        void on_Yslider_scale_valueChanged(int value);
        void on_dial_rotate_valueChanged(int value);

        //��ť�ؼ�
        void on_translateButton_clicked();
        void on_scaleButton_clicked();
        void on_rotateButton_clicked();
        void on_resetButton_clicked();
};

#endif // GEOMETRYTRANSFORM_H
