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
    dicomProcess(QWidget *parent = 0, Qt::WFlags flags = 0);//���캯��
    ~dicomProcess();//��������

    Ui::dicomProcess* ui;

    //�Զ����Ա����
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
    char* inputFileName;//��ITK��DICOM

    //�Զ����Ա����
public:
    void showImgProperty(vtkDICOMImageReader* reader);

    //�Զ����ź�&��
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
