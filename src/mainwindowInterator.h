#ifndef MAINWINDOWINTERATOR_H
#define MAINWINDOWINTERATOR_H

#include <QtGui/QMainWindow>
#include "mainwindow.h"
#include "vtkCustomCommands.h"//�Զ���vtk����
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

//����Ԥ������
class vtkImageReader2;
class vtkImageViewer2;
class vtkRenderer;
class vtkRenderWindowInteractor;
class vtkImageReader2Factory;
class vtkRenderWindowInteractor;
class vtkMetaImageReader;

//�Զ�����
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

    //�Զ����Ա����
private:
    QString qstrInputFileName;//��ͷ�ļ��в������ʼ������
    QByteArray ba;
    char* inputFileName;
    char* outputFileName;//�����������ΪͼƬ
    int pixelType;//�����������
    int* imgSize;//��ʾͼ��ߴ�
    int imgDepth;//unsigned char -> 8 bits
    IplImage* img;//��ʾͼ���������
    int dimension;//ά��
    QProgressDialog* progDlg;
    QTimer* timer;
    int currentValue;//���ȶԻ���

    //��Ķ���
    //ָ���������
    dicomProcess* pDicomPro; 
    geometryTransform* pGeomtry;
    morphology* pMorph;
    itkFilter* pFilter;
    pointOperation* pPointOpe;
    fourierTransform* pFourierTrans;
    edgeContour* pEdgeContour;
    segmentation* pSegmentation;
    registration* pRegistration;

    //�������
    //ͬһ���ؼ����ܼ��벻ͬwidget��
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

    //2����ͼ����
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

    //vtk����
    vtkNextSliceCallbk* pCall;
    vtkInteractorStyleImage* style;

    //3D���ӻ�
    vtkMetaImageReader* mhdReader; 
    vtkStructuredPointsReader* vtkReader;//vtkStructuredPointsReader /vtkGenericDataObjectReader / vtkPolyDataReader
    vtkImageReader* rawReader;
    vtkImageFlip* rotate;
    vtkDICOMImageReader*dicomReader;

    //�Զ����Ա����
private:
    void createControlPanel();
    bool isSurpported2DImgType();
    bool isSurpported3DImgType();
    bool isMHDImgType();//��֧��mhd
    void initVisualizationPipeline();

    //ϵͳ��Ԥ�����ź�
    //��ť��clicked(), �Ӳ˵���triggered(), ������valueChanged()
    //�Զ����&�ź�
signals:
    //һ���źŶ�Ӧ����ۣ����ִֻ��ĳһ���ۣ�

    private slots:
        void updateResultViewer(char* fileName);//��ͼ�ؼ���ʾ�������
        void updateOriginalViewer(char* fileName);
        void clearResultViewer();//������ͼ
        void resetControlPanel();//���ÿ������
        void clear();//ɾ���Ѿ�ʵ��������Ķ���
        void retinalLayerVisualization();//�������ڸ����Ӵ�
        void abnormalRegionVisualization();
        void updateProgressDialog();//������ʱ�����½��ȶԻ���

        //�������ѡ��

        //�ļ��˵�
        void on_action_Open_triggered();
        void on_action_Open_3DImage_triggered();
        void on_action_OpenDICOM_triggered();
        void on_action_Save_triggered();
        //������
        void on_action_Color2Gray_triggered();
        void on_action_ColorHist_triggered();
        void on_action_GrayHist_triggered();
        void on_action_Interpolation_triggered();
        void on_action_SingleChannel_triggered();
        void on_action_HistgramEqualization_triggered();
        //ͼ��仯�˵�
        void on_action_Geometry_triggered();//���α仯
        void on_action_Morphology_triggered();//��̬ѧ�仯
        void on_action_Fourier_triggered();//��ɢ����Ҷ�仯
        //ͼ���˲��˵�
        void on_action_MeanFilter_triggered();
        void on_action_GaussianFilter_triggered();
        void on_action_BilateralFilter_triggered();
        void on_action_LaplacianFilter_triggered();
        void on_action_CurvatureAnisotropicDiffusionFilter_triggered();
        //��Ե������
        void on_action_Edge_triggered();
        //���ӻ��˵�
        void on_action_VolumeRender_triggered();
        void on_action_SurfaceRender_triggered();
        //ͼ��ָ�˵���
        void on_action_RegionGrowSeg_triggered();
        void on_action_WatershedSeg_triggered();
        void on_action_PEDSeg_triggered();
        //ͼ����׼�˵���
        void on_action_Registration_2D_triggered();
};
#endif // MAINWINDOWINTERATOR_H
