#ifndef DICOMPROCESS_H
#define DICOMPROCESS_H

#include <QtGui/QMainWindow>
#include "ui_dicomProcess.h"
#include <vtkDICOMImageReader.h>
#include <QSqlDatabase>

class dicomProcess : public QWidget
{
    Q_OBJECT

public:
    dicomProcess(QWidget *parent = 0, Qt::WFlags flags = 0);//构造函数
    ~dicomProcess();//析构函数

    Ui::dicomProcess* ui;

    //自定义成员变量
private:
    bool isConnected;
    QSqlDatabase dbSQL;
    char* nameInfo;
    char* ID;
    char* time;
    char* manufacture;
    char* modility;
    char* hospital;

public:
    char* inputFileName;//由ITK读DICOM

    //自定义成员函数
public:
    void showImgProperty(vtkDICOMImageReader* reader);

    //自定义信号&槽
signals:

    private slots:
        void on_addButtonSQL_clicked();
        void on_deleteButtonSQL_clicked();
        void on_modifyButtonSQL_clicked();
        void on_queryButtonSQL_clicked();
        void on_clearButtonSQL_clicked();
        void on_showAllTableButtonSQL_clicked();
};

#endif // DICOMPROCESS_H
