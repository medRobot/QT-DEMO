#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include <QtGui/QMainWindow>
#include "ui_segmentation.h"
#include <string>
using namespace std;//for string type

class segmentation : public QWidget
{
    Q_OBJECT

public:
    segmentation(QWidget *parent = 0, Qt::WFlags flags = 0);//���캯��
    ~segmentation();//��������

    Ui::segmentation* ui;

    //�Զ����Ա����
private:
    string filePrefix;//�ļ�ǰ׺��
    char outputFileName[100];//string to char*
    char initFileName[100];
    char* initResultFileName;

public:
    char* inputFileName;

signals:
     void returnOutputFileName(char* fileName);
     void returnInternalFileName(char* fileName);//����м���������ԭ������ͼ
     void executeRetinalLayerVisualization();//��������ʵ��
      void executeAbnormalRegionVisualization();

    private slots:
        //�Զ����Ա����
        void on_filterButton_clicked();
        void on_retinalLayerSegButton_clicked();
        void on_retinalLayerVisualizationButton_clicked();
        void on_abnormalRegionSegButton_clicked();
        void on_abnormalRegionVisualizationButton_clicked();
        void on_morphologyOperationButton_clicked();

private:
        void erosionOperation();
};

#endif // SEGMENTATION_H
