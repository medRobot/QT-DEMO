#ifndef MAINWINDOWINTERATOR_H
#define MAINWINDOWINTERATOR_H

#include <QtGui/QMainWindow>
#include "mainwindow.h"
#include "vtkCustomCommands.h"//自定义vtk交互
#include <QLabel>
#include <QPixmap>
#include <QProgressDialog>
#include <QTimer>
#include <vtkMetaImageReader.h>
#include <vtkDICOMImageReader.h>
#include <vtkStructuredPointsReader.h>//read vtk
#include <vtkGenericDataObjectReader.h>
#include <vtkPolyDataReader.h>
#include <vtkImageReader.h>//read raw
#include <vtkImageFlip.h>
#include <cv.h>

//库中预定义类
class vtkImageReader2;
class vtkImageViewer2;
class vtkRenderer;
class vtkRenderWindowInteractor;
class vtkImageReader2Factory;
class vtkRenderWindowInteractor;
class vtkMetaImageReader;

//自定义类
class dicomProcess;
class geometryTransform;
class pointOperation;
class itkFilter;
class morphology;
class fourierTransform;
class edgeContour;
class segmentation;
class registration;

class mainwindowInterator : public mainWindow
{
    Q_OBJECT

public:
    mainwindowInterator(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~mainwindowInterator();

    //自定义成员变量
private:
    QString qstrInputFileName;//类头文件中不允许初始化变量
    QByteArray ba;
    char* inputFileName;
    char* outputFileName;//将输出结果另存为图片
    int pixelType;//获得数据类型
    int* imgSize;//显示图像尺寸
    int imgDepth;//unsigned char -> 8 bits
    IplImage* img;//显示图像基本属性
    int dimension;//维度
    QProgressDialog* progDlg;
    QTimer* timer;
    int currentValue;//进度对话框

    //类的对象
    //指针变量命名
    dicomProcess* pDicomPro; 
    geometryTransform* pGeomtry;
    morphology* pMorph;
    itkFilter* pFilter;
    pointOperation* pPointOpe;
    fourierTransform* pFourierTrans;
    edgeContour* pEdgeContour;
    segmentation* pSegmentation;
    registration* pRegistration;

    //控制面板
    //同一个控件不能加入不同widget中
    QWidget* img2DPropertywidget ;
    QWidget* img3DPropertywidget ;
    QLabel* img2DHeightName, *img3DHeightName;
    QLabel* img2DHeightNum, *img3DHeightNum;
    QLabel* img2DWidthName, *img3DWidthName;
    QLabel* img2DWidthNum, *img3DWidthNum;
    QLabel* img3DLengthName;
    QLabel* img3DLengthNum;
    QLabel* img2DDepthName, *img3DDepthName;
    QLabel* img2DDepthNum, *img3DDepthNum;
    QLabel* img2DChannelsName;
    QLabel* img2DChannelsNum;

    //2个视图窗口
    vtkImageReader2* originalReader;
    vtkImageViewer2* originalVtkViewer;
    vtkRenderer* originalRenderder;
    vtkRenderWindowInteractor* originalRenWinInteractor; 
    vtkImageReader2Factory* originalReaderFactory;

    vtkImageReader2* resultReader;
    vtkImageViewer2* resultVtkViewer;
    vtkRenderer* resultRenderder;
    vtkRenderWindowInteractor* resultRenWinInteractor; 
    vtkImageReader2Factory* resultReaderFactory;

    //vtk交互
    vtkNextSliceCallbk* pCall;
    vtkInteractorStyleImage* style;

    //3D可视化
    vtkMetaImageReader* mhdReader; 
    vtkStructuredPointsReader* vtkReader;//vtkStructuredPointsReader /vtkGenericDataObjectReader / vtkPolyDataReader
    vtkImageReader* rawReader;
    vtkImageFlip* rotate;
    vtkDICOMImageReader*dicomReader;

    //自定义成员函数
private:
    void createControlPanel();
    bool isSurpported2DImgType();
    bool isSurpported3DImgType();
    bool isMHDImgType();//仅支持mhd
    void initVisualizationPipeline();

    //系统中预定义信号
    //按钮用clicked(), 子菜单用triggered(), 滑条用valueChanged()
    //自定义槽&信号
signals:
    //一个信号对应多个槽，如何只执行某一个槽？

    private slots:
        void updateResultViewer(char* fileName);//视图控件显示结果更新
        void updateOriginalViewer(char* fileName);
        void clearResultViewer();//重置视图
        void resetControlPanel();//重置控制面板
        void clear();//删除已经实例化的类的对象
        void retinalLayerVisualization();//在主窗口更新视窗
        void abnormalRegionVisualization();
        void updateProgressDialog();//借助定时器更新进度对话框

        //控制面板选择

        //文件菜单
        void on_action_Open_triggered();
        void on_action_Open_3DImage_triggered();
        void on_action_OpenDICOM_triggered();
        void on_action_Save_triggered();
        //点运算
        void on_action_Color2Gray_triggered();
        void on_action_ColorHist_triggered();
        void on_action_GrayHist_triggered();
        void on_action_Interpolation_triggered();
        void on_action_SingleChannel_triggered();
        void on_action_HistgramEqualization_triggered();
        //图像变化菜单
        void on_action_Geometry_triggered();//几何变化
        void on_action_Morphology_triggered();//形态学变化
        void on_action_Fourier_triggered();//离散傅里叶变化
        //图像滤波菜单
        void on_action_MeanFilter_triggered();
        void on_action_GaussianFilter_triggered();
        void on_action_BilateralFilter_triggered();
        void on_action_LaplacianFilter_triggered();
        void on_action_CurvatureAnisotropicDiffusionFilter_triggered();
        //边缘与轮廓
        void on_action_Edge_triggered();
        //可视化菜单
        void on_action_VolumeRender_triggered();
        void on_action_SurfaceRender_triggered();
        //图像分割菜单栏
        void on_action_RegionGrowSeg_triggered();
        void on_action_WatershedSeg_triggered();
        void on_action_PEDSeg_triggered();
        //图像配准菜单栏
        void on_action_Registration_2D_triggered();
};
#endif // MAINWINDOWINTERATOR_H
