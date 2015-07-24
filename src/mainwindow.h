#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_imageProcess.h"

class mainWindow : public QMainWindow
{
    Q_OBJECT

public:
    mainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~mainWindow();

public:
    Ui::mainWindowClass* ui;//uiΪmainWindow��ĳ�Ա����

    //�Զ����Ա����
private:
    void showStateBar();
    void setAboutMenu();

    //���干�вۣ��������п�����д����
    //������ڻ����ж����麯����
    public slots:
        virtual void updateResultViewer(char* fileName);//��ͼ�ؼ���ʾ�������

        //�������ѡ��
        //�ļ��˵�
        virtual void on_action_Open_triggered();
        virtual void on_action_Open_3DImage_triggered();
        virtual void on_action_OpenDICOM_triggered();
        virtual void on_action_Save_triggered();
        //�����˵�
        virtual void on_action_About_triggered();
        //������
        virtual void on_action_Color2Gray_triggered();
        virtual void on_action_ColorHist_triggered();
        virtual void on_action_GrayHist_triggered();
        virtual void on_action_Interpolation_triggered();
        virtual void on_action_SingleChannel_triggered();
        virtual void on_action_HistgramEqualization_triggered();
        //ͼ��仯�˵�
        virtual void on_action_Geometry_triggered();
        virtual void on_action_Morphology_triggered();
        virtual void on_action_Fourier_triggered();
        //ͼ���˲��˵�
        virtual void on_action_MeanFilter_triggered();
        virtual void on_action_GaussianFilter_triggered();
        virtual void on_action_BilateralFilter_triggered();
        virtual void on_action_LaplacianFilter_triggered();
        virtual void on_action_CurvatureAnisotropicDiffusionFilter_triggered();
        //��Ե������
        virtual void on_action_Edge_triggered();
        //���ӻ��˵�
        virtual void on_action_VolumeRender_triggered();
        virtual void on_action_SurfaceRender_triggered();
        //ͼ��ָ�˵���
        virtual void on_action_RegionGrowSeg_triggered();
        virtual void on_action_WatershedSeg_triggered();
        virtual void on_action_PEDSeg_triggered();
        //ͼ����׼�˵���
        virtual void on_action_Registration_2D_triggered();
};

#endif // MAINWINDOW_H
