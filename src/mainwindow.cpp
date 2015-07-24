#include "mainwindow.h"
#include <QLabel>
#include <QMessageBox>
#include <QDebug>

mainWindow::mainWindow(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags),
    ui(new Ui::mainWindowClass)
{
   ui->setupUi(this);

   //控制面板初始化
   ui->logoLable->setPixmap(QPixmap("../data/logo/logo.jpg"));

    //状态栏
    showStateBar();
}

//状态栏
void mainWindow::showStateBar()//状态栏暂时只支持用代码创建
{
    ui->statusBar->showMessage(tr("欢迎使用数字图像处理软件"), 10000);

    QLabel* permanent = new QLabel(this);
    permanent->setFrameStyle(QFrame::Box | QFrame::Sunken);
    //permanent->setText("welcome to my blog: http://blog.csdn.net/learn_sunzhuli");
    permanent->setText(tr("该数字图像处理软件基于QT, ITK, VTK, OPENCV开发！"));
    ui->statusBar->addPermanentWidget(permanent);
}

//帮助菜单栏
void mainWindow::on_action_About_triggered()
{
    QMessageBox::about(this, tr("关于对话框"), tr("<p>数字图像处理软件版本0.1</p> <p>开发者： 孙助力</p><p>时间：2015.5</p>"));//html语言
}

//操作之后的结果都在主窗口中更新， 也可以专门写一个类用于更新图片
void mainWindow::updateResultViewer(char* fileName)
{
  
}

//自动关联信号&槽
//文件菜单栏
void mainWindow::on_action_Open_triggered()
{
    
}

void mainWindow::on_action_OpenDICOM_triggered()
{

}

void mainWindow::on_action_Open_3DImage_triggered()
{

}

void mainWindow::on_action_Save_triggered()
{

}

//点运算菜单栏
void mainWindow::on_action_Color2Gray_triggered()
{
    
}

void mainWindow::on_action_HistgramEqualization_triggered()
{
   
}

void mainWindow::on_action_Interpolation_triggered()
{
   
}

void mainWindow::on_action_GrayHist_triggered()
{
    
}

void mainWindow::on_action_ColorHist_triggered()
{
   
}

void mainWindow::on_action_SingleChannel_triggered()
{
    
}

//图像变化菜单栏
void mainWindow::on_action_Geometry_triggered()
{
    //几何变化菜单栏
}

void mainWindow::on_action_Morphology_triggered()
{
    //形态学变化菜单栏
}

void mainWindow::on_action_Fourier_triggered()
{
    //傅里叶变化菜单栏
}

//图像滤波菜单栏
void mainWindow::on_action_MeanFilter_triggered()
{
    
}

void mainWindow::on_action_GaussianFilter_triggered()
{

}

void mainWindow::on_action_BilateralFilter_triggered()
{

}

void mainWindow::on_action_LaplacianFilter_triggered()
{

}

void mainWindow::on_action_CurvatureAnisotropicDiffusionFilter_triggered()
{

}

//边缘与轮廓菜单栏
void mainWindow::on_action_Edge_triggered()
{

}

//图像分割菜单栏
void mainWindow::on_action_RegionGrowSeg_triggered()
{

}

void mainWindow::on_action_WatershedSeg_triggered()
{

}

void mainWindow::on_action_PEDSeg_triggered()
{

}

//图像配准菜单栏
void mainWindow::on_action_Registration_2D_triggered()
{

}

//图像可视化菜单栏
void mainWindow::on_action_VolumeRender_triggered()
{

}

void mainWindow::on_action_SurfaceRender_triggered()
{

}

//析构函数
mainWindow::~mainWindow()
{
    delete ui;
}
