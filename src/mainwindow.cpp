#include "mainwindow.h"
#include <QLabel>
#include <QMessageBox>
#include <QDebug>

mainWindow::mainWindow(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags),
    ui(new Ui::mainWindowClass)
{
   ui->setupUi(this);

   //��������ʼ��
   ui->logoLable->setPixmap(QPixmap("../data/logo/logo.jpg"));

    //״̬��
    showStateBar();
}

//״̬��
void mainWindow::showStateBar()//״̬����ʱֻ֧���ô��봴��
{
    ui->statusBar->showMessage(tr("��ӭʹ������ͼ�������"), 10000);

    QLabel* permanent = new QLabel(this);
    permanent->setFrameStyle(QFrame::Box | QFrame::Sunken);
    //permanent->setText("welcome to my blog: http://blog.csdn.net/learn_sunzhuli");
    permanent->setText(tr("������ͼ�����������QT, ITK, VTK, OPENCV������"));
    ui->statusBar->addPermanentWidget(permanent);
}

//�����˵���
void mainWindow::on_action_About_triggered()
{
    QMessageBox::about(this, tr("���ڶԻ���"), tr("<p>����ͼ��������汾0.1</p> <p>�����ߣ� ������</p><p>ʱ�䣺2015.5</p>"));//html����
}

//����֮��Ľ�������������и��£� Ҳ����ר��дһ�������ڸ���ͼƬ
void mainWindow::updateResultViewer(char* fileName)
{
  
}

//�Զ������ź�&��
//�ļ��˵���
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

//������˵���
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

//ͼ��仯�˵���
void mainWindow::on_action_Geometry_triggered()
{
    //���α仯�˵���
}

void mainWindow::on_action_Morphology_triggered()
{
    //��̬ѧ�仯�˵���
}

void mainWindow::on_action_Fourier_triggered()
{
    //����Ҷ�仯�˵���
}

//ͼ���˲��˵���
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

//��Ե�������˵���
void mainWindow::on_action_Edge_triggered()
{

}

//ͼ��ָ�˵���
void mainWindow::on_action_RegionGrowSeg_triggered()
{

}

void mainWindow::on_action_WatershedSeg_triggered()
{

}

void mainWindow::on_action_PEDSeg_triggered()
{

}

//ͼ����׼�˵���
void mainWindow::on_action_Registration_2D_triggered()
{

}

//ͼ����ӻ��˵���
void mainWindow::on_action_VolumeRender_triggered()
{

}

void mainWindow::on_action_SurfaceRender_triggered()
{

}

//��������
mainWindow::~mainWindow()
{
    delete ui;
}
