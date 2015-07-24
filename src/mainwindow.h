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
    Ui::mainWindowClass* ui;//ui为mainWindow类的成员变量

    //自定义成员函数
private:
    void showStateBar();
    void setAboutMenu();

    //定义共有槽，在子类中可以重写功能
    //如果不在基类中定义虚函数？
    public slots:
        virtual void updateResultViewer(char* fileName);//视图控件显示结果更新

        //控制面板选择
        //文件菜单
        virtual void on_action_Open_triggered();
        virtual void on_action_Open_3DImage_triggered();
        virtual void on_action_OpenDICOM_triggered();
        virtual void on_action_Save_triggered();
        //帮助菜单
        virtual void on_action_About_triggered();
        //点运算
        virtual void on_action_Color2Gray_triggered();
        virtual void on_action_ColorHist_triggered();
        virtual void on_action_GrayHist_triggered();
        virtual void on_action_Interpolation_triggered();
        virtual void on_action_SingleChannel_triggered();
        virtual void on_action_HistgramEqualization_triggered();
        //图像变化菜单
        virtual void on_action_Geometry_triggered();
        virtual void on_action_Morphology_triggered();
        virtual void on_action_Fourier_triggered();
        //图像滤波菜单
        virtual void on_action_MeanFilter_triggered();
        virtual void on_action_GaussianFilter_triggered();
        virtual void on_action_BilateralFilter_triggered();
        virtual void on_action_LaplacianFilter_triggered();
        virtual void on_action_CurvatureAnisotropicDiffusionFilter_triggered();
        //边缘与轮廓
        virtual void on_action_Edge_triggered();
        //可视化菜单
        virtual void on_action_VolumeRender_triggered();
        virtual void on_action_SurfaceRender_triggered();
        //图像分割菜单栏
        virtual void on_action_RegionGrowSeg_triggered();
        virtual void on_action_WatershedSeg_triggered();
        virtual void on_action_PEDSeg_triggered();
        //图像配准菜单栏
        virtual void on_action_Registration_2D_triggered();
};

#endif // MAINWINDOW_H
