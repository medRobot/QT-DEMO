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
    segmentation(QWidget *parent = 0, Qt::WFlags flags = 0);//构造函数
    ~segmentation();//析构函数

    Ui::segmentation* ui;

    //自定义成员变量
private:
    string filePrefix;//文件前缀名
    char outputFileName[100];//string to char*
    char initFileName[100];
    char* initResultFileName;

public:
    char* inputFileName;

signals:
     void returnOutputFileName(char* fileName);
     void returnInternalFileName(char* fileName);//输出中间结果，更新原窗口视图
     void executeRetinalLayerVisualization();//到主窗口实现
      void executeAbnormalRegionVisualization();

    private slots:
        //自定义成员函数
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
