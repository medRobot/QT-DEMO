#include "mainwindowInterator.h"
#include "geometryTransform.h"
#include "orthogonalTransform.h"
#include "morphology.h"
#include "pointOperation.h"
#include "dicomProcess.h"
#include "itkFilter.h"
#include "edge_contour.h"
#include "segmentation.h"
#include "registration.h"

#include <itkImage.h>//ITK
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkMeanImageFilter.h>
#include <vtkMetaImageReader.h>//VTK
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkImageViewer2.h>//2D, 3D
#include <vtkInteractorStyleImage.h>//尝试不同的交互类型 
#include <vtkRenderer.h>
#include <vtkCommand.h>
#include <vtkImageFlip.h>//图像反转
#include <QVTKWidget.h>
#include <vtkImageActor.h>
#include <vtkImageReader2Factory.h>//支持PNG, BMP, TIFF, JPG
#include <vtkImageReader.h>
#include <vtkVolume16Reader.h>
#include <vtkImageData.h>
#include <vtkImageCast.h>
#include <vtkContourFilter.h>//surface & volume rendering
#include <vtkDecimatePro.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkStripper.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkVolume.h>
#include <vtkProperty.h>
#include <vtkOutlineFilter.h>
#include <vtkMarchingCubes.h>
//体绘制
#include "vtkCamera.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkProperty.h"
#include "vtkPolyDataNormals.h"
#include "vtkImageShiftScale.h"
#include "vtkVolumeRayCastMapper.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkInteractorStyleRubberBand3D.h"
#include <QGridLayout>//QT
#include <QFileDialog>
#include <QTransform>
#include <QMessageBox>
#include <QString>


#include <string>//C++
#include <iostream>
using namespace std;

mainwindowInterator::mainwindowInterator(QWidget *parent, Qt::WFlags flags)
    :mainWindow(parent, flags)
{
    //初始化类的对象
    pGeomtry = NULL;//不在构造函数中实例化该类的对象
    pMorph = NULL;
    pFilter = NULL;
    pPointOpe = NULL;
    pFourierTrans = NULL;
    pEdgeContour = NULL;
    pDicomPro = NULL;
    pSegmentation = NULL;
    pRegistration = NULL;

    //初始化变量
    pCall = NULL;

    //实例化
    originalReader = vtkImageReader2::New();
    originalReaderFactory = vtkImageReader2Factory::New();
    originalVtkViewer = vtkImageViewer2::New();
    originalRenderder = vtkRenderer::New();
    originalRenWinInteractor = vtkRenderWindowInteractor::New();
    resultReader = vtkImageReader2::New();
    resultReaderFactory = vtkImageReader2Factory::New();
    resultVtkViewer = vtkImageViewer2::New();
    resultRenderder = vtkRenderer::New();
    resultRenWinInteractor = vtkRenderWindowInteractor::New();
    ui->originalQVTKViewer->GetRenderWindow()->AddRenderer(originalRenderder);
    ui->resultQVTKViewer->GetRenderWindow()->AddRenderer(resultRenderder);
    ui->resultQVTKViewer->setEnabled(false);

    //控制面板初始化
    createControlPanel();
}

//统一命名
//传递参数用 fileName, pixmap （用指针传递参数，当重新加载图片时，能及时更新）
//定义成员变量用 inputFileName, inputImage, pix
//对变量或者函数命名一定要规范， 防止getQpixmap(QPixmap*) 错误写成 getQPixmap(QPixmap*), 
//一个信号对应多个槽, 发送信号后，对应槽都会执行一遍，如何只执行某一个槽？（一个信号对应一个槽）

void mainwindowInterator::initVisualizationPipeline()//再次打开图像时同时更新
{
    if(originalReader != NULL)//1
    {
        originalReader->Delete();
        originalReader = vtkImageReader2::New();
    }
    if(originalReaderFactory != NULL)//2
    {
        originalReaderFactory->Delete();
        originalReaderFactory = vtkImageReader2Factory::New();
    }
    if(originalVtkViewer != NULL)//3
    {
        //if(pCall != NULL)
        //originalVtkViewer->RemoveObserver(pCall);//从3D到2D，不要翻页
        //originalVtkViewer->Delete();
        //如果删除originalVtkViewer，导致从显示3D图到显示2D图，会进入滚轮交互事件 2015.5.22
        originalVtkViewer = vtkImageViewer2::New();
    }
    if(originalRenderder != NULL)//4
    {
        originalRenderder->Delete();
        originalRenderder = vtkRenderer::New();
    }
    if(originalRenWinInteractor != NULL)//5
    {
        originalRenWinInteractor->Delete();
        originalRenWinInteractor = vtkRenderWindowInteractor::New();
    }
    if(resultReader != NULL)//6
    {
        resultReader->Delete();
        resultReader = vtkImageReader2::New();
    }
    if(resultReaderFactory != NULL)//7
    {
        resultReaderFactory->Delete();
        resultReaderFactory = vtkImageReader2Factory::New();
    }
    if(resultVtkViewer != NULL)//8
    {
        //if(pCall != NULL)
        //resultVtkViewer->RemoveAllObservers();
        //resultVtkViewer->Delete();
        resultVtkViewer = vtkImageViewer2::New();
    }
    if(resultRenderder != NULL)//9
    {
        resultRenderder->Delete();
        resultRenderder = vtkRenderer::New();
    }
    if(resultRenWinInteractor != NULL)//10
    {
        resultRenWinInteractor->Delete();
        resultRenWinInteractor = vtkRenderWindowInteractor::New();
    }
    ui->originalQVTKViewer->GetRenderWindow()->AddRenderer(originalRenderder);//init
    ui->resultQVTKViewer->GetRenderWindow()->AddRenderer(resultRenderder);
    //originalVtkViewer->SetRenderer(originalRenderder);//初始化中增加该代码，报vtk错误
    //resultVtkViewer->SetRenderer(resultRenderder);
}

//主窗口结果视图更新， 也可以专门写一个类用于更新视图
void mainwindowInterator::updateResultViewer(char* fileName)
{
    outputFileName = fileName;//将输出结果另存为图片
    //判断文件后缀
    string fileExtension;
    fileExtension = fileName;//char* to string
    fileExtension = fileExtension.substr(fileExtension.length() - 3, fileExtension.length());
    //用于控制是否执行clearResultViewer()  
    if(!ui->resultQVTKViewer->isEnabled())
        ui->resultQVTKViewer->setEnabled(true);

    //更新视图
    if(fileExtension == "mhd" || fileExtension == "MHD")//3D
    {
        mhdReader = vtkMetaImageReader::New();
        mhdReader->SetFileName(fileName);
        mhdReader->Update(); 
        //统计最大最小灰度值 （或者数据类型），自适应设置窗宽&窗位
        //当灰度值高于该范围的最大值时，均以白影显示；而低于该范围时均显示为黑色
        pixelType =  mhdReader->GetDataScalarType();//获得数据类型
        rotate = vtkImageFlip::New();//图像反转
        rotate->SetInputConnection(mhdReader->GetOutputPort());
        rotate->SetFilteredAxis(1);//y轴反转
        resultVtkViewer->SetInputConnection(rotate->GetOutputPort());

        //交互
        ui->resultQVTKViewer->GetRenderWindow()->AddRenderer(resultRenderder);
        resultVtkViewer->SetRenderer(resultRenderder);//设置为同一个渲染场景
        resultVtkViewer->SetRenderWindow(ui->resultQVTKViewer->GetRenderWindow());//设置为同一个渲染窗口
        pCall = vtkNextSliceCallbk::New();
        style = vtkInteractorStyleImage::New();
        style->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall); //可能与预定义交互方式冲突
        style->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
        resultRenWinInteractor->SetInteractorStyle(style);
        resultRenWinInteractor->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall);//通过交互窗口去监听
        resultRenWinInteractor->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
        pCall->SetInteractorStyle(style);
        pCall->SetViewer(originalVtkViewer, resultVtkViewer);
        pCall->SetInteractor(resultRenWinInteractor);
        ui->resultQVTKViewer->GetRenderWindow()->SetInteractor(resultRenWinInteractor);
        resultVtkViewer->SetupInteractor(ui->resultQVTKViewer->GetRenderWindow()->GetInteractor());//设置交互方式
        resultVtkViewer->SetSliceOrientationToXY(); //设置切片方向
        if(pixelType == VTK_UNSIGNED_SHORT || pixelType == VTK_SHORT)
        {
            imgDepth = 16;//图像深度
            resultVtkViewer->SetColorLevel(32768);//窗位 
            resultVtkViewer->SetColorWindow(65536);//窗宽 
        }
        else//VTK_UNSIGNED_CHAR 
        {
            imgDepth = 8;//图像深度
            resultVtkViewer->SetColorLevel(128); 
            resultVtkViewer->SetColorWindow(256); 
        }
        //自动绘制出图像
        resultVtkViewer->Render();
        resultRenWinInteractor->Initialize();
        resultRenWinInteractor->Start();//该函数会导致下面的代码不会继续执行
        //ui->resultQVTKViewer->GetRenderWindow()->Render();
    }
    else//2D
    {
        resultReader = resultReaderFactory->CreateImageReader2(fileName);
        resultReader->SetFileName(fileName);
        resultReader->Update();
        resultVtkViewer->SetInputConnection(resultReader->GetOutputPort());
        resultVtkViewer->UpdateDisplayExtent();  
        resultVtkViewer->SetRenderWindow(ui->resultQVTKViewer->GetRenderWindow());  
        resultVtkViewer->SetRenderer(resultRenderder);  
        resultVtkViewer->SetupInteractor(ui->resultQVTKViewer->GetRenderWindow()->GetInteractor());  
        //显示彩色图（3通道）不要设置窗宽&窗位
        if(img->nChannels != 3)//img为输入图像
        {
            resultVtkViewer->SetColorLevel(128);//二维图灰度范围0~255
            resultVtkViewer->SetColorWindow(256);//显示2D, 3D图像，切换时要重新设置窗宽&窗位
        }
        resultVtkViewer->SetSliceOrientationToXY(); //默认就是这个方向的
        resultVtkViewer->GetSlice();
        resultVtkViewer->GetImageActor()->InterpolateOff();  
        resultRenderder->ResetCamera();  
        resultRenderder->DrawOn();  
        ui->resultQVTKViewer->GetRenderWindow()->Render();
    }
}

//用于PED分割，对比形态学变化前后结果图
void mainwindowInterator::updateOriginalViewer(char* fileName)
{
    //判断文件后缀
    string fileExtension;
    fileExtension = fileName;//char* to string
    fileExtension = fileExtension.substr(fileExtension.length() - 3, fileExtension.length());

    if(fileExtension == "mhd" || fileExtension == "MHD")
    {
        mhdReader = vtkMetaImageReader::New();
        mhdReader->SetFileName(fileName);
        mhdReader->Update();
        pixelType =  mhdReader->GetDataScalarType();//获得数据类型
        rotate = vtkImageFlip::New();//图像反转
        rotate->SetInputConnection(mhdReader->GetOutputPort());
        rotate->SetFilteredAxis(1);//y轴反转
        originalVtkViewer->SetInputConnection(rotate->GetOutputPort());
    }

    //交互
    ui->originalQVTKViewer->GetRenderWindow()->AddRenderer(originalRenderder);
    originalVtkViewer->SetRenderer(originalRenderder);//设置为同一个渲染场景
    originalVtkViewer->SetRenderWindow(ui->originalQVTKViewer->GetRenderWindow());//设置为同一个渲染窗口

    pCall = vtkNextSliceCallbk::New();
    style = vtkInteractorStyleImage::New();
    style->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall); 
    style->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
    originalRenWinInteractor->SetInteractorStyle(style);

    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall);//通过交互窗口去监听
    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);

    pCall->SetInteractorStyle(style);
    pCall->SetViewer(originalVtkViewer, resultVtkViewer);
    pCall->SetInteractor(originalRenWinInteractor);

    ui->originalQVTKViewer->GetRenderWindow()->SetInteractor(originalRenWinInteractor);
    originalVtkViewer->SetupInteractor(ui->originalQVTKViewer->GetRenderWindow()->GetInteractor());//设置交互方式
    originalVtkViewer->SetSliceOrientationToXY(); //设置切片方向
    //自动绘制出图像
    //不要用start(), 否则下面的语句不能执行
    ui->originalQVTKViewer->GetRenderWindow()->Render();
}

void mainwindowInterator::updateProgressDialog()
{
    currentValue++;  
    if( currentValue == 100 )  
        currentValue = 0;  
   progDlg ->setValue(currentValue);
   QCoreApplication::processEvents();//避免界面冻结
   if(progDlg->wasCanceled()) 
   {
        //timer->stop();
        //progDlg->hide();
        //delete progDlg;
   }
}

//控制面板设置
void mainwindowInterator::createControlPanel()
{
    //用代码创建控件（控件比较少）
    //读2D图
    img2DPropertywidget = new QWidget();
    img2DHeightName = new QLabel();
    img2DHeightNum = new QLabel();
    img2DWidthName = new QLabel();
    img2DWidthNum = new QLabel();
    img2DDepthName = new QLabel();
    img2DDepthNum = new QLabel();
    img2DChannelsName = new QLabel();
    img2DChannelsNum = new QLabel();

    img2DHeightName->setText(tr("图像高度(pix)"));
    img2DWidthName->setText(tr("图像宽度(pix)"));
    img2DDepthName->setText(tr("图像深度(位)"));
    img2DChannelsName->setText(tr("图像通道数(个)"));

    QGridLayout* img2DPropertyLayout = new QGridLayout();
    img2DPropertyLayout->addWidget(img2DWidthName, 0, 0, 0);//最后一个参数设置对齐方式
    img2DPropertyLayout->addWidget(img2DWidthNum, 0, 1, 0); 
    img2DPropertyLayout->addWidget(img2DHeightName, 1, 0, 0); //宽 x 高
    img2DPropertyLayout->addWidget(img2DHeightNum, 1, 1, 0); 
    img2DPropertyLayout->addWidget(img2DDepthName, 2, 0,  0); 
    img2DPropertyLayout->addWidget(img2DDepthNum, 2, 1,  0); 
    img2DPropertyLayout->addWidget(img2DChannelsName, 3, 0,  0); 
    img2DPropertyLayout->addWidget(img2DChannelsNum, 3, 1,  0); 

    img2DPropertywidget->setLayout(img2DPropertyLayout);
    img2DPropertywidget->setFixedSize(260, 110);//大小不好控制，选择UI设计
    img2DPropertywidget->setVisible(false);

    //读3D图
    img3DPropertywidget = new QWidget();
    img3DLengthName = new QLabel();
    img3DLengthNum = new QLabel();
    img3DWidthName = new QLabel();
    img3DWidthNum = new QLabel();
    img3DHeightName = new QLabel();
    img3DHeightNum = new QLabel();
    img3DDepthName = new QLabel();
    img3DDepthNum = new QLabel();

    img3DLengthName->setText(tr("图像长度(pix)"));
    img3DWidthName->setText(tr("图像宽度(pix)"));
    img3DHeightName->setText(tr("图像高度(slice)"));
    img3DDepthName->setText(tr("图像深度(位)"));

    QGridLayout* img3DPropertyLayout = new QGridLayout();
    img3DPropertyLayout->addWidget(img3DLengthName, 0, 0, 0);//长
    img3DPropertyLayout->addWidget(img3DLengthNum, 0, 1, 0); 
    img3DPropertyLayout->addWidget(img3DWidthName, 1, 0, 0); //宽
    img3DPropertyLayout->addWidget(img3DWidthNum, 1, 1, 0); 
    img3DPropertyLayout->addWidget(img3DHeightName, 2, 0, 0); //高
    img3DPropertyLayout->addWidget(img3DHeightNum, 2, 1, 0); 
    img3DPropertyLayout->addWidget(img3DDepthName, 3, 0,  0); //数据类型
    img3DPropertyLayout->addWidget(img3DDepthNum, 3, 1,  0); 

    img3DPropertywidget->setLayout(img3DPropertyLayout);
    img3DPropertywidget->setFixedSize(260, 110);//大小不好控制，选择UI设计
    img3DPropertywidget->setVisible(false);
}

//重新加载图片或者执行其他操作时，清空上一次结果图
void mainwindowInterator::clearResultViewer()  
{
    if(ui->resultQVTKViewer->isEnabled())//新建一个vtkRender，换一个舞台等于清空
    {
        ui->resultQVTKViewer->GetRenderWindow()->RemoveRenderer(resultRenderder);
        resultRenderder = vtkRenderer::New();
        ui->resultQVTKViewer->GetRenderWindow()->AddRenderer(resultRenderder);
        ui->resultQVTKViewer->GetRenderWindow()->Render();
    }
}

//重新加载图片后，重置控制面板
void mainwindowInterator::resetControlPanel() //仅针对有ui对象的类
{
    if(pGeomtry != NULL)
    {
        pGeomtry->resetControlPanel();
    }
    if(pMorph != NULL)
    {
        pMorph->resetControlPanel();
    }
}

void mainwindowInterator::clear()//删除已经实例化的类对象
{
    if(pGeomtry != NULL)//1
    {
        delete pGeomtry;
        pGeomtry = NULL;
    }
    if(pMorph != NULL)//2
    {
        delete pMorph;
        pMorph = NULL;
    }
    if(pPointOpe != NULL)//3
    {
        delete pPointOpe;
        pPointOpe = NULL;
    }
    if(pFilter != NULL)//4
    {
        delete pFilter;
        pFilter = NULL;//当pFilter = NULL, 无法调用该类中成员函数
    }
    if(pFourierTrans != NULL)//5
    {
        delete pFourierTrans;
        pFourierTrans = NULL;
    }
    if(pEdgeContour != NULL)//6
    {
        delete pEdgeContour;
        pEdgeContour = NULL;
    }
    if(pDicomPro != NULL)//7
    {
        delete pDicomPro;
        pDicomPro = NULL;
    }
    if(pSegmentation != NULL)//8
    {
        delete pSegmentation;
        pSegmentation = NULL;
    }
}

//文件菜单栏
void mainwindowInterator::on_action_Open_triggered()
{
    clearResultViewer(); //清空上一次操作的结果图
    resetControlPanel(); //重置控制面板
    initVisualizationPipeline();//重置可视化管线
    //读入图像
    QString filter;
    filter = tr("二维图像 (*.jpg *.JPG *.jpeg *.JPEG *.bmp *.BMP *.png *.PNG *.tiff *.TIFF)");
    qstrInputFileName = QFileDialog::getOpenFileName(this, tr("打开图像"), QDir::currentPath(), filter);//获取当前路径
    if (qstrInputFileName.isEmpty()) 
    {
        QMessageBox::warning(this, tr("提示信息对话框"), tr("输入图片路径为空，请重新选择!"), QMessageBox::Ok);
        return;
    }
    if (!qstrInputFileName.isEmpty()) 
    {
        ba = qstrInputFileName.toLatin1(); //QString to char*
        inputFileName = ba.data();
        originalReader = originalReaderFactory->CreateImageReader2(inputFileName);
        if (originalReader)
        {
            originalReader->SetFileName(inputFileName);
            originalReader->Update();
            pixelType = originalReader->GetDataScalarType();//数据类型
            dimension = 2;//维度
        }
        else
        {
            QMessageBox::warning(this, tr("提示信息对话框"),
                tr("输入图片格式暂不支持，请重新选择!目前支持的图片格式为JPEG, JPG, PNG, BMP, TIFF"), 
                QMessageBox::Ok);
            return;
        }
        //更新当前图片基本数据
        img = cvLoadImage(inputFileName, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
        img2DWidthNum->setText(QString::number(img->width, 10));//int ->QString 
        img2DHeightNum->setText(QString::number(img->height, 10));//int ->QString
        img2DDepthNum->setText(QString::number(img->depth, 10));//int ->QString
        img2DChannelsNum->setText(QString::number(img->nChannels, 10));//OpenCV对通道数封装
        if(!img2DPropertywidget->isVisible())//如果控件已经可见，则直接更新
        {
            ui->controlPanelLayout->addWidget(img2DPropertywidget);//显示图片基本属性
            img2DPropertywidget->setVisible(true);
        }
        originalVtkViewer->UpdateDisplayExtent();//更新
        originalVtkViewer->SetInputConnection(originalReader->GetOutputPort());
        originalVtkViewer->SetRenderWindow(ui->originalQVTKViewer->GetRenderWindow());  
        originalVtkViewer->SetRenderer(originalRenderder);  
        originalVtkViewer->SetupInteractor(ui->originalQVTKViewer->GetRenderWindow()->GetInteractor());  
        //显示彩色图，不能设置窗位&窗宽
        if(img->nChannels != 3)//灰度图设置窗位&窗宽
        {
            originalVtkViewer->SetColorLevel(128);//二维图灰度范围0~255
            originalVtkViewer->SetColorWindow(256);//显示2D, 3D图像，切换时要重新设置窗宽&窗位
        }
        originalVtkViewer->SetSliceOrientationToXY(); //默认就是这个方向的  
        originalVtkViewer->GetImageActor()->InterpolateOff();  
        originalRenderder->ResetCamera();  
        originalRenderder->DrawOn(); 
        //originalVtkViewer->Render();//图像为放大状态
        ui->originalQVTKViewer->GetRenderWindow()->Render();
    }
    //删除已经实例化的类对象
    clear();//如果该语句放在前面，则可能出现控制面板先消失再出现的情况，应避免
    //删除logo
    if(ui->logoLable != NULL)
    {
        delete ui->logoLable;
        ui->logoLable = NULL;
    }
    //删除3D图片基本属性控件
    if(img3DPropertywidget->isVisible())
    {
        img3DPropertywidget->setVisible(false);
        ui->controlPanelLayout->removeWidget(img3DPropertywidget);//don't work
        //ui->controlPanelLayout->removeItem(ui->controlPanelLayout);//删除布局
        ui->controlPanelLayout->update();
    }
}

void mainwindowInterator::on_action_Open_3DImage_triggered()
{
    clearResultViewer(); //清空上一次操作的结果图
    resetControlPanel(); //重置控制面板
    initVisualizationPipeline();

    //读入图像
    QString filter;
    filter = tr("三维图像 (*.mhd *.raw *.vtk)");
    QString fileName = QFileDialog::getOpenFileName( this, QString(tr("打开图像")), QDir::currentPath() , filter );
    if ( fileName.isEmpty() == true ) return;
    // QString to char*
    ba = fileName.toLocal8Bit();
    inputFileName = ba.data();
    //判断文件后缀
    string fileExtension;
    fileExtension = inputFileName;//char* to string
    fileExtension = fileExtension.substr(fileExtension.length() - 3, fileExtension.length());

    if(fileExtension == "mhd" || fileExtension == "MHD")
    {
        mhdReader = vtkMetaImageReader::New();
        mhdReader->SetFileName(inputFileName);
        mhdReader->Update();//cannot delete
        //统计最大最小灰度值 （或者数据类型），自适应设置窗宽&窗位
        //当灰度值高于该范围的最大值时，均以白影显示；而低于该范围时均显示为黑色
        pixelType =  mhdReader->GetDataScalarType();//获得数据类型
        //获得数据维度
        dimension = mhdReader->GetFileDimensionality();
        imgSize = mhdReader->GetOutput()->GetExtent();//显示图像尺寸
        rotate = vtkImageFlip::New();//图像反转
        rotate->SetInputConnection(mhdReader->GetOutputPort());
        rotate->SetFilteredAxis(1);//y轴反转
        originalVtkViewer->SetInputConnection(rotate->GetOutputPort());
    }
    else  if(fileExtension == "vtk" || fileExtension == "VTK")//切换切片过程中出现堆栈溢出
    {
        vtkReader = vtkStructuredPointsReader::New();
        vtkReader->SetFileName(inputFileName);
        vtkReader->Update();//cannot delete
        //显示图像尺寸, 没成功
        int dim[6] = {0, 180, 0, 216, 0, 180};
        imgSize = dim;
        pixelType = VTK_UNSIGNED_CHAR;//不知道vtk数据类型，但灰度值范围一般为0~255
        dimension = 3;
        //不需要反转
        originalVtkViewer->SetInputConnection(vtkReader->GetOutputPort());
    }
    else  if(fileExtension == "raw" || fileExtension == "RAW")//raw数据读进来后必须进行维度，尺寸等设置
    {
        rawReader = vtkImageReader::New();
        rawReader->SetFileName(inputFileName);
        rawReader->Update();//cannot delete
        pixelType =  rawReader->GetDataScalarType();//获得数据类型
        rawReader->SetFileDimensionality(3); //设置显示图像的维数
        rawReader->SetDataScalarType(pixelType);
        rawReader->SetDataExtent(0, 511, 0, 479, 0, 63);//要手动设置尺寸，则没有普遍实用性
        rawReader->SetDataSpacing(1, 1, 1);
        imgSize = rawReader->GetDataExtent();
        dimension = rawReader->GetFileDimensionality();
        originalVtkViewer->SetInputConnection(rawReader->GetOutputPort());
    }
    else
    {
        QMessageBox::warning(this, tr("提示信息对话框"),
            tr("输入图片格式暂不支持，请重新选择!目前支持的图片格式为MHD, RAW, VTK"), QMessageBox::Ok);
        return;
    }

    //交互
    originalVtkViewer->UpdateDisplayExtent();//更新, 没效果
    ui->originalQVTKViewer->GetRenderWindow()->AddRenderer(originalRenderder);
    originalVtkViewer->SetRenderer(originalRenderder);//设置为同一个渲染场景
    originalVtkViewer->SetRenderWindow(ui->originalQVTKViewer->GetRenderWindow());//设置为同一个渲染窗口
    pCall = vtkNextSliceCallbk::New();
    style = vtkInteractorStyleImage::New();
    style->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall); //可能与预定义交互方式冲突
    style->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
    originalRenWinInteractor->SetInteractorStyle(style);
    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall);//通过交互窗口去监听
    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
    pCall->SetInteractorStyle(style);
    pCall->SetViewer(originalVtkViewer, resultVtkViewer);
    pCall->SetInteractor(originalRenWinInteractor);
    ui->originalQVTKViewer->GetRenderWindow()->SetInteractor(originalRenWinInteractor);
    originalVtkViewer->SetupInteractor(ui->originalQVTKViewer->GetRenderWindow()->GetInteractor());//设置交互方式
    originalVtkViewer->SetSliceOrientationToXY(); //设置切片方向
    if(pixelType == VTK_UNSIGNED_SHORT || pixelType == VTK_SHORT)
    {
        imgDepth = 16;//图像深度
        originalVtkViewer->SetColorLevel(32768);//窗位 
        originalVtkViewer->SetColorWindow(65536);//窗宽 
    }
    else//VTK_UNSIGNED_CHAR， 但是vtk数据类型不知道, AtlasSegmentation.vtk灰度值为1， 2， 3
    {
        imgDepth = 8;//图像深度
        originalVtkViewer->SetColorLevel(128); 
        originalVtkViewer->SetColorWindow(256); 
    }
    //更新当前图片基本数据
    img3DLengthNum->setText(QString::number(imgSize[1] + 1, 10));//int ->QString 
    img3DWidthNum->setText(QString::number(imgSize[3] + 1, 10));//int ->QString
    img3DHeightNum->setText(QString::number(imgSize[5] + 1, 10));//int ->QString
    img3DDepthNum->setText(QString::number(imgDepth, 10));//int ->QString
    if(!img3DPropertywidget->isVisible())//如果控件已经可见，则直接更新
    {
        ui->controlPanelLayout->addWidget(img3DPropertywidget);//显示图片基本属性
        img3DPropertywidget->setVisible(true);
    }
    //删除已经实例化的类对象
    clear();//如果该语句放在前面，则可能出现控制面板先消失再出现的情况，应避免
    //删除logo
    if(ui->logoLable != NULL)
    {
        delete ui->logoLable;
        ui->logoLable = NULL;
    }
    //删除2D图片基本属性控件
    if(img2DPropertywidget->isVisible())
    {
        img2DPropertywidget->setVisible(false);
        ui->controlPanelLayout->removeWidget(img2DPropertywidget);//没效果，要修改2015.5.15
        ui->controlPanelLayout->update();
    }
    //自动绘制出图像

    originalRenderder->ResetCamera();
    originalRenderder->Render();
    //originalRenderder->DrawOn();
    ui->originalQVTKViewer->GetRenderWindow()->Render();
}

void mainwindowInterator::on_action_OpenDICOM_triggered()
{
    //暂时只支持读入dicom数据并与数据库交互，没有对数据处理
    clearResultViewer(); //清空上一次操作的结果图
    resetControlPanel(); //重置控制面板
    initVisualizationPipeline();
    //读入序列图像，获取当前文件夹路径
    QString fileName = QFileDialog::getExistingDirectory( this, QString(tr("选择文件夹")), QDir::currentPath());
    if ( fileName.isEmpty() == true ) return;
    // QString to char*
    ba = fileName.toLocal8Bit();
    inputFileName = ba.data();
    dicomReader = vtkDICOMImageReader::New();
    dicomReader->SetDirectoryName(inputFileName);//读序列切片
    dicomReader->Update();
    dimension = dicomReader->GetFileDimensionality();//dcm图片2D
    pixelType = dicomReader->GetDataScalarType();
    originalVtkViewer->SetInputConnection(dicomReader->GetOutputPort());//可视化管线输入
    originalVtkViewer->UpdateDisplayExtent();//更新

    //交互
    originalVtkViewer->SetRenderer(originalRenderder);//设置为同一个渲染场景
    originalVtkViewer->SetRenderWindow(ui->originalQVTKViewer->GetRenderWindow());//设置为同一个渲染窗口
    pCall = vtkNextSliceCallbk::New();
    style = vtkInteractorStyleImage::New();
    style->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall); //可能与预定义交互方式冲突
    style->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
    originalRenWinInteractor->SetInteractorStyle(style);
    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall);//通过交互窗口去监听
    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
    pCall->SetInteractorStyle(style);
    pCall->SetViewer(originalVtkViewer, resultVtkViewer);
    pCall->SetInteractor(originalRenWinInteractor);
    ui->originalQVTKViewer->GetRenderWindow()->SetInteractor(originalRenWinInteractor);
    originalVtkViewer->SetupInteractor(ui->originalQVTKViewer->GetRenderWindow()->GetInteractor());//设置交互方式
    originalVtkViewer->SetSliceOrientationToXY(); //设置切片方向
    originalVtkViewer->SetColorLevel(128); 
    originalVtkViewer->SetColorWindow(256); 
    //删除logo
    if(ui->logoLable != NULL)
    {
        delete ui->logoLable;
        ui->logoLable = NULL;
    }
    //删除2D & 3D图片基本属性控件
    if(img2DPropertywidget->isVisible())
    {
        img2DPropertywidget->setVisible(false);
        ui->controlPanelLayout->removeWidget(img2DPropertywidget);//没效果，要修改2015.5.15
        ui->controlPanelLayout->update();
    }
    if(img3DPropertywidget->isVisible())//如果控件已经可见，则移走控件
    {
        img3DPropertywidget->setVisible(false);
        ui->controlPanelLayout->removeWidget(img3DPropertywidget);//没效果，要修改2015.5.15
        ui->controlPanelLayout->update();
    }
    clear();//删除已经实例化的类的对象，避免重复加载
    pDicomPro = new dicomProcess(this);
    ui->controlPanelLayout->addWidget(pDicomPro);//设置控制面板
    pDicomPro->setFixedSize(330, 600);//控制面板尺寸
    pDicomPro->inputFileName = this->inputFileName;//ITK读取数据
    pDicomPro->showImgProperty(dicomReader); //更新当前图片基本数据
    //自动绘制出图像
    originalVtkViewer->Render();
    //ui->originalQVTKViewer->GetRenderWindow()->Render();
}

//判断读入图片类型(判断文件后缀)，如果对应操作不支持该图片格式，则给出提示
bool mainwindowInterator::isSurpported2DImgType()
{
    string fileExtension;
    fileExtension = inputFileName;//char* to string
    fileExtension = fileExtension.substr(fileExtension.length() - 3, fileExtension.length());
    if(fileExtension != "jpg" && fileExtension != "peg" && fileExtension != "bmp" 
        && fileExtension != "png" && fileExtension != "iff")//"peg"->"jpeg", "iff"->"tiff"
    {
        QMessageBox::warning(this, tr("提示信息对话框"),
            tr("<p>该操作暂时不支持该图片格式，请重新选择二维图片!</p>"
            " <p>目前支持二维图片格式有：jpg, jpeg, bmp, tiff, png</p>"),  QMessageBox::Ok);
        return false;
    }
    return true;
}

bool mainwindowInterator::isSurpported3DImgType()
{
    string fileExtension;
    fileExtension = inputFileName;//char* to string
    fileExtension = fileExtension.substr(fileExtension.length() - 3, fileExtension.length());
    if(fileExtension != "mhd" && fileExtension != "raw" && fileExtension != "dcm" 
        && fileExtension != "vtk" )
    {
        QMessageBox::warning(this, tr("提示信息对话框"),
            tr("<p>该操作暂时不支持该图片格式，请重新选择三维图片!</p>"
            " <p>目前支持三维图片格式有：mhd, raw, dcm, vtk</p>"),  QMessageBox::Ok);
        return false;
    }
    return true;
}

bool mainwindowInterator::isMHDImgType()
{
    string fileExtension;
    fileExtension = inputFileName;//char* to string
    fileExtension = fileExtension.substr(fileExtension.length() - 3, fileExtension.length());
    if(fileExtension != "mhd" || fileExtension != "MHD")
    {
        QMessageBox::warning(this, tr("提示信息对话框"),
            tr("<p>该操作暂时不支持该图片格式，请重新选择三维图片!</p>"
            " <p>目前支持MHD三维图片格式</p>"),  QMessageBox::Ok);
        return false;
    }
    return true;
}

void mainwindowInterator::on_action_Save_triggered()
{
    //用类名而不是类的对象调用函数， 要区分
    //如果用OpenCV存储图片，则需要指定类型
    QString outputFileName = QFileDialog::getSaveFileName(this, tr("保存文件"), "F:", tr("图片文件(*png *jpg *bmp *tiff )"));
    QPixmap* pix = new QPixmap();
    QString qstr= QString(QLatin1String(inputFileName));//从updateResultViewer()中获取
    pix->load(qstr);
    pix->save(outputFileName);
}

//点运算菜单栏
void mainwindowInterator::on_action_Color2Gray_triggered()
{
    //如果不支持该图片类型，则返回
    if(!isSurpported2DImgType())
    {
        return;
    }
    //通过调用成员函数传递参数，与信号&槽区分
    clear();//删除已经实例化的类的对象，避免重复加载
    pPointOpe = new pointOperation();
    //建立连接要在发送信号之前
    connect(pPointOpe, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));//返回主窗口更新视图
    pPointOpe->color2Gray(inputFileName);
}

void mainwindowInterator::on_action_HistgramEqualization_triggered()
{
    //如果不支持该图片类型，则返回
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();//删除已经实例化的类的对象，避免重复加载
    pPointOpe = new pointOperation();
    //建立连接要在发送信号之前
    connect(pPointOpe, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));//返回主窗口更新视图
    pPointOpe->histgramEqualization(inputFileName);
}

void mainwindowInterator::on_action_Interpolation_triggered()
{
    //如果不支持该图片类型，则返回
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();//删除已经实例化的类的对象，避免重复加载
    pPointOpe = new pointOperation();
    //建立连接
    connect(pPointOpe, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));//返回主窗口更新视图
    pPointOpe->interpolation(inputFileName);
}

void mainwindowInterator::on_action_GrayHist_triggered()
{
    //如果不支持该图片类型，则返回
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();//删除已经实例化的类的对象，避免重复加载
    pPointOpe = new pointOperation();
    //建立连接
    connect(pPointOpe, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));//返回主窗口更新视图
    pPointOpe->grayHistgram(inputFileName);
}

void mainwindowInterator::on_action_ColorHist_triggered()
{
    //如果不支持该图片类型，则返回
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();//删除已经实例化的类的对象，避免重复加载
    pPointOpe = new pointOperation();
    //建立连接
    connect(pPointOpe, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));//返回主窗口更新视图
    pPointOpe->colorHistgram(inputFileName);
}

void mainwindowInterator::on_action_SingleChannel_triggered()
{
    //如果不支持该图片类型，则返回
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();//删除已经实例化的类的对象，避免重复加载
    pPointOpe = new pointOperation();
    //建立连接
    connect(pPointOpe, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));//返回主窗口更新视图
    pPointOpe->getSingleChannelImg(inputFileName);
}

//几何变化菜单栏
void mainwindowInterator::on_action_Geometry_triggered() 
{
    //如果不支持该图片类型，则返回
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();//删除已经实例化的类的对象，避免重复加载
    pGeomtry = new geometryTransform(this);
    ui->controlPanelLayout->addWidget(pGeomtry);
    pGeomtry->setVisible(true);
    pGeomtry->inputFileName = &qstrInputFileName;//传递当前打开的图片到geometryTransform类中, QString

    //在控制面板中设置具体操作
    connect(pGeomtry, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));//更新视图
    connect(pGeomtry, SIGNAL(reset()), this, SLOT(clearResultViewer()));//重置，左边窗口重新加载视图，右边窗口清空
}

//正交变化菜单栏
void mainwindowInterator::on_action_Fourier_triggered()//离散傅里叶变化
{
    //如果不支持该图片类型，则返回
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();//删除已经实例化的类的对象，避免重复加载
    pFourierTrans = new fourierTransform();
    //建立连接
    connect(pFourierTrans, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pFourierTrans->discreteFourierTrans(inputFileName);
}

//形态学变化菜单栏
//点击菜单，控制面板更新
void mainwindowInterator::on_action_Morphology_triggered()
{
    //如果不支持该图片类型，则返回
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();//删除已经实例化的类的对象，避免重复加载
    pMorph = new morphology(this);
    ui->controlPanelLayout->addWidget(pMorph);
    pMorph->setVisible(true);
    pMorph->inputFileName = inputFileName;//用指针传递当前打开的图片到morphology类中

    //在控制面板中设置具体操作
    connect(pMorph, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));//更新视图
    connect(pMorph, SIGNAL(reset()), this, SLOT(clearResultViewer()));//重置，左边窗口重新加载视图，右边窗口清空
}

//图像增强菜单栏
//滤波函数都用DLLf封装，不要UI
//点击菜单，执行滤波操作
void mainwindowInterator::on_action_MeanFilter_triggered()
{
    clear();//删除已经实例化的类的对象，避免重复加载
    pFilter = new itkFilter();
    //建立连接
    connect(pFilter, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pFilter->meanFilter(inputFileName, dimension, pixelType);
}

void mainwindowInterator::on_action_GaussianFilter_triggered()
{
    clear();
    pFilter = new itkFilter();
    //建立连接
    connect(pFilter, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pFilter->gaussianFilter(inputFileName, dimension, pixelType);
}

void mainwindowInterator::on_action_BilateralFilter_triggered()
{
    clear();
    pFilter = new itkFilter();
    //建立连接
    connect(pFilter, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pFilter->bilateralFilter(inputFileName, dimension, pixelType);
}

void mainwindowInterator::on_action_LaplacianFilter_triggered()
{
    clear();
    pFilter = new itkFilter();
    //建立连接
    connect(pFilter, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pFilter->laplacianFilter(inputFileName, dimension, pixelType);
}

void mainwindowInterator::on_action_CurvatureAnisotropicDiffusionFilter_triggered()
{ 
    clear();
    pFilter = new itkFilter();
    //建立连接
    connect(pFilter, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pFilter->curvatureAnisotropicDiffusionFilter(inputFileName, dimension, pixelType);   
}

//边缘与轮廓菜单栏
void mainwindowInterator::on_action_Edge_triggered()
{
    //如果不支持该图片类型，则返回
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();//清空上一次控制面板
    pEdgeContour = new edgeContour();
    //建立连接
    connect(pEdgeContour, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pEdgeContour->sobelOperator(inputFileName);
}

//图像分割菜单栏
void mainwindowInterator::on_action_RegionGrowSeg_triggered()
{
    QMessageBox::warning(this, tr("提示信息对话框"), tr("该功能暂未实现!"), QMessageBox::Ok);
    return;
}

void mainwindowInterator::on_action_WatershedSeg_triggered()
{
    QMessageBox::warning(this, tr("提示信息对话框"), tr("该功能暂未实现!"), QMessageBox::Ok);
    return;
}

void mainwindowInterator::on_action_PEDSeg_triggered()
{
    clear();//删除已经实例化的类的对象，避免重复加载
    pSegmentation = new segmentation();
    pSegmentation->setFixedSize(270, 300);//该控件要稍微大些
    ui->controlPanelLayout->addWidget(pSegmentation);
    pSegmentation->inputFileName = inputFileName;//用指针传递当前打开的图片到morphology类中
    connect(pSegmentation, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));//更新结果视图
    connect(pSegmentation, SIGNAL(returnInternalFileName(char*)), this, SLOT(updateOriginalViewer(char*)));//更新初始视图
    connect(pSegmentation, SIGNAL(executeRetinalLayerVisualization()), this, SLOT(retinalLayerVisualization()));//视网膜分层
    connect(pSegmentation, SIGNAL(executeAbnormalRegionVisualization()), this, SLOT(abnormalRegionVisualization()));//病变区域
}

void mainwindowInterator::retinalLayerVisualization()
{
    progDlg = new QProgressDialog();
    progDlg->setWindowTitle("Please wait...");  
    progDlg->setRange(0, 100);
    progDlg->setFixedSize(350, 100);
    progDlg->show();
    timer = new QTimer();
    currentValue = 0;
    progDlg->setValue(currentValue);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateProgressDialog()));
    timer->start(100);//开启一个没有终点的定时器

    //新建可视化管线
    resultRenderder = vtkRenderer::New();
    vtkRenderWindow* renwin = vtkRenderWindow::New();
    renwin->AddRenderer(resultRenderder);
    resultRenWinInteractor = vtkRenderWindowInteractor::New();
    resultRenWinInteractor->SetRenderWindow(renwin);
    ui->resultQVTKViewer->GetRenderWindow()->AddRenderer(resultRenderder);//必须有
    ui->resultQVTKViewer->GetRenderWindow()->SetInteractor(resultRenWinInteractor);

    int surface[64][512];
    int dims[3]= {512, 480, 64};
    double R[4],G[4],B[4];//give color to retina surface
    R[0]=1;R[1]=0;R[2]=0;R[3]=1;
    G[0]=0;G[1]=1;G[2]=0;G[3]=1;
    B[0]=0;B[1]=0;B[2]=1;B[3]=0;

    ifstream fin;
    string str1[4] = {"1", "7", "11", "12"};
    string  str2=".txt";
    for(int slice = 0; slice < 4; slice++)
    {
        string  str ="F:/qt/project_try/imageProcess_2_3/data/ped/txt/15715_";
        str = str + str1[slice] + str2;
        fin.open(str,ios::in);
        if(fin.fail())
        {
            cout<<"File";
            exit(1);
        }
        for(int k=0;k<dims[2];k++)
            for(int i=0;i<dims[0];i++)
            {
                fin>>surface[k][i];
            }
            fin.close(); 
            fin.clear(); 

            vtkImageData *retinalSurfaceData = vtkImageData::New();
            int dimsSp[3];
            dimsSp[0]=512/2;
            dimsSp[1]=480/2;
            dimsSp[2]=64*4;
            retinalSurfaceData->SetDimensions(dimsSp);
            retinalSurfaceData->SetSpacing(1,1,1);
            retinalSurfaceData->SetScalarTypeToUnsignedChar();

            for(int k=0;k<256;k++)
                for(int j=0;j<240;j++)
                    for(int i=0;i<256;i++)
                    {
                        unsigned char *pixel = (unsigned char*)(retinalSurfaceData->GetScalarPointer(i,j,k));
                        *pixel = 0;
                    }//must initiate a new memory

                    //插值算法重新写
                    for(int k=0;k<dims[2]-1;k++)//z axis
                        for(int i=0;i<dims[0]/2;i++)//y axis
                        {
                            int z1 = surface[k][i*2]/2+slice*10;
                            int z2 = surface[k+1][i*2]/2+slice*10;
                            double zD = (double(z2)-double(z1))/4;
                            for(int j=0;j<4;j++)
                            {
                                unsigned char *pixel = (unsigned char*)(retinalSurfaceData->GetScalarPointer(i,(z1+zD*j),(k*4+j)));
                                *pixel = 2;//give all pixels a same intensity
                                unsigned char *pixel1 = (unsigned char*)(retinalSurfaceData->GetScalarPointer(i,(z1+zD*j+1),(k*4+j)));
                                *pixel1 = 2;
                                unsigned char *pixel2 = (unsigned char*)(retinalSurfaceData->GetScalarPointer(i,(z1+zD*j-1),(k*4+j)));
                                *pixel2 = 2;
                            }
                        }

                        vtkMarchingCubes *retinalSurfaceExtractor = vtkMarchingCubes::New();
                        retinalSurfaceExtractor->SetInput(retinalSurfaceData);
                        retinalSurfaceExtractor->SetValue(slice, 2);//设置阈值，大于或者等于2为1 小于2为0 二值图像重建.
                        vtkDecimatePro *deci=vtkDecimatePro::New();//减少mesh的三角面片. 
                        deci->SetInputConnection(retinalSurfaceExtractor->GetOutputPort());  
                        deci->SetTargetReduction(0.7);//将原先的三角面片减少到原来的百分之七十.  
                        vtkSmoothPolyDataFilter *smooth=vtkSmoothPolyDataFilter::New();   
                        smooth->SetInputConnection(deci->GetOutputPort());  
                        smooth->SetNumberOfIterations(100) ;//设置Laplace平滑的迭代次数 
                        vtkPolyDataNormals *retinalSurfaceNormals = vtkPolyDataNormals::New();
                        retinalSurfaceNormals->SetInputConnection(smooth->GetOutputPort());
                        retinalSurfaceNormals->SetFeatureAngle(60.0);
                        vtkStripper *stripper=vtkStripper::New();   
                        stripper->SetInput(retinalSurfaceNormals->GetOutput());  
                        vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
                        mapper->SetInputConnection(stripper->GetOutputPort());
                        mapper->ScalarVisibilityOff();
                        vtkActor *retinalSurface = vtkActor::New();
                        retinalSurface->SetMapper(mapper);
                        retinalSurface->GetProperty()->SetDiffuseColor(R[slice],G[slice],B[slice]);//give a specific color of surface
                        resultRenderder->AddActor(retinalSurface);
                        //及时释放资源
                        retinalSurfaceExtractor->Delete();
                        mapper->Delete();
                        retinalSurfaceNormals->Delete();
                        retinalSurface->Delete();
    }

    vtkCamera *aCamera = vtkCamera::New();
    aCamera->SetViewUp(0,-5,0);//设视角位置 
    aCamera->SetPosition(0,0,1);//设观察对象位
    aCamera->SetFocalPoint(0,0,0);
    aCamera->ComputeViewPlaneNormal();
    resultRenderder->SetActiveCamera(aCamera);
    resultRenderder->ResetCamera();//特别耗时
    aCamera->Dolly(1.5);
    resultRenderder->SetBackground(0.0, 0.0, 0.0);//黑色背景色
    resultRenderder->ResetCameraClippingRange();

    //没有实现交互
    //resultRenWinInteractor->Initialize();
    //resultRenWinInteractor->Start();
    ui->resultQVTKViewer->GetRenderWindow()->Render();
    //timer->stop();
    delete progDlg;
}

void mainwindowInterator::abnormalRegionVisualization()
{
    //新建可视化管线
    resultRenderder = vtkRenderer::New();
    vtkRenderWindow* renwin = vtkRenderWindow::New();
    renwin->AddRenderer(resultRenderder);
    resultRenWinInteractor = vtkRenderWindowInteractor::New();
    resultRenWinInteractor->SetRenderWindow(renwin);
    ui->resultQVTKViewer->GetRenderWindow()->AddRenderer(resultRenderder);//必须有
    ui->resultQVTKViewer->GetRenderWindow()->SetInteractor(resultRenWinInteractor);

    mhdReader = vtkMetaImageReader::New();
    mhdReader->SetFileName("../data/ped/15715_3d_result.mhd");
    //定义颜色
    double R[3],G[3],B[3];
    R[0]=1;R[1]=0;R[2]=0; 
    G[0]=0;G[1]=1;G[2]=0; 
    B[0]=0;B[1]=0;B[2]=1; 
     
    //提取等值面
    for(int i = 1; i < 3; i++)//error
    {
        vtkMarchingCubes *retinalSurfaceExtractor = vtkMarchingCubes::New();
        retinalSurfaceExtractor->SetInputConnection(mhdReader->GetOutputPort());
        retinalSurfaceExtractor->SetValue(i, i + 1);//设置阈值，大于或者等于2为1 小于2为0 二值图像重建.
        vtkDecimatePro *deci=vtkDecimatePro::New();//减少mesh的三角面片. 
        deci->SetInputConnection(retinalSurfaceExtractor->GetOutputPort());  
        deci->SetTargetReduction(0.7);//将原先的三角面片减少到原来的百分之七十.  
        vtkSmoothPolyDataFilter *smooth=vtkSmoothPolyDataFilter::New();   
        smooth->SetInputConnection(deci->GetOutputPort());  
        smooth->SetNumberOfIterations(100) ;//设置Laplace平滑的迭代次数 
        vtkPolyDataNormals *retinalSurfaceNormals = vtkPolyDataNormals::New();
        retinalSurfaceNormals->SetInputConnection(smooth->GetOutputPort());
        retinalSurfaceNormals->SetFeatureAngle(60.0);
        vtkStripper *stripper=vtkStripper::New();   
        stripper->SetInput(retinalSurfaceNormals->GetOutput());  
        vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
        mapper->SetInputConnection(stripper->GetOutputPort());
        mapper->ScalarVisibilityOff();
        vtkActor *retinalSurface = vtkActor::New();
        retinalSurface->SetMapper(mapper);
        retinalSurface->GetProperty()->SetDiffuseColor(R[i],G[i],B[i]);//give a specific color of surface
        resultRenderder->AddActor(retinalSurface);
        //及时释放资源
        retinalSurfaceExtractor->Delete();
        mapper->Delete();
        retinalSurfaceNormals->Delete();
        retinalSurface->Delete();
    }

    //没有实现交互 2015.4.23
    vtkCamera *aCamera = vtkCamera::New();
    aCamera->SetViewUp(0, -5, 0);//设视角位置 
    aCamera->SetPosition(0, 0, 1);//设观察对象位
    aCamera->SetFocalPoint(0, 0, 0);
    aCamera->ComputeViewPlaneNormal();
    resultRenderder->SetActiveCamera(aCamera);
    //resultRenderder->ResetCamera(); 
    aCamera->Dolly(1.5);
    resultRenderder->SetBackground(0.0, 0.0, 0.0);//黑色背景色
    resultRenderder->ResetCameraClippingRange();
    ui->resultQVTKViewer->GetRenderWindow()->Render();
    //resultRenWinInteractor->Initialize();
    //resultRenWinInteractor->Start();
}

//图像配准菜单栏
void mainwindowInterator::on_action_Registration_2D_triggered()
{
    //如果不支持该图片类型，则返回
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();//清空上一次控制面板
    QString filter;
    filter = tr("二维图像 (*.jpg *.JPG *.jpeg *.JPEG *.bmp *.BMP *.png *.PNG *.tiff *.TIFF)");
    QString qstrInputFileName2 = QFileDialog::getOpenFileName(this,
        tr("请选择校准操作所需的第二张图像"), QDir::currentPath(), filter);//获取当前路径
    if (qstrInputFileName2.isEmpty()) 
    {
        QMessageBox::warning(this, tr("提示信息对话框"), tr("输入图片路径为空，请重新选择!"), QMessageBox::Ok);
        return;
    }
    char* movingImageName;
    QByteArray ba = qstrInputFileName2.toLatin1(); //QString to char*
    movingImageName = ba.data();
    pRegistration = new registration();
    //建立连接
    connect(pRegistration, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    //第一张为固定图像，第二张为移动图像，第二张参照第一张进行配准
    pRegistration->itkDemonsRegistration(inputFileName, movingImageName);
}

//图像可视化菜单栏
//体绘制会伴随颜色渐变情况（如果想要单一颜色标识某区域，则避免用体绘制）
void mainwindowInterator::on_action_VolumeRender_triggered() 
{
    isMHDImgType();//仅支持mhd
    //新建可视化管线
    resultRenderder = vtkRenderer::New();
    vtkRenderWindow* renwin = vtkRenderWindow::New();
    renwin->AddRenderer(resultRenderder);
    resultRenWinInteractor = vtkRenderWindowInteractor::New();
    resultRenWinInteractor->SetRenderWindow(renwin);
    ui->resultQVTKViewer->GetRenderWindow()->AddRenderer(resultRenderder);//必须有
    ui->resultQVTKViewer->GetRenderWindow()->SetInteractor(resultRenWinInteractor);

    mhdReader = vtkMetaImageReader::New();
    mhdReader->SetFileName(inputFileName);

    // 创建标量到不透明度的转移函数。
    vtkPiecewiseFunction *opacityTransferFunction=vtkPiecewiseFunction::New();
    opacityTransferFunction->AddPoint( 20, 0.0);
    opacityTransferFunction->AddPoint(255, 0.7);

    // 创建标量的颜色的转移函数。
    vtkColorTransferFunction *colorTransferFunction=vtkColorTransferFunction::New();
    colorTransferFunction->AddRGBPoint(  0.0, 0.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint( 64.0, 1.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(128.0, 0.0, 0.0, 1.0);
    colorTransferFunction->AddRGBPoint(192.0, 0.0, 1.0, 0.0);
    colorTransferFunction->AddRGBPoint(255.0, 0.0, 0.2, 0.0);

    // 设置属性，其中包括两个映射函数。
    vtkVolumeProperty *volumeProperty=vtkVolumeProperty::New();
    volumeProperty->SetColor(colorTransferFunction);
    volumeProperty->SetScalarOpacity(opacityTransferFunction);
    volumeProperty->ShadeOn();
    volumeProperty->SetInterpolationTypeToLinear();

    // 光线投射映射器知道如何渲染数据。
    vtkVolumeRayCastCompositeFunction  *compositeFunction=
        vtkVolumeRayCastCompositeFunction::New();
    vtkVolumeRayCastMapper *volumeMapper=vtkVolumeRayCastMapper::New();
    volumeMapper->SetVolumeRayCastFunction(compositeFunction);
    volumeMapper->SetInputConnection(mhdReader->GetOutputPort());//只能输入unsigned char or unsigned short

    // vtkVolume掌管映射器和属性对象，可控制体数据的位置和朝向
    vtkVolume *volume=vtkVolume::New();
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);
    resultRenderder->AddVolume(volume);

    //没有实现交互 2015.5.24
    //如果不设置camera, 则图像倒置显示
    vtkCamera *aCamera = vtkCamera::New(); 
    aCamera->SetViewUp(0,-5,0);//设视角位置 
    aCamera->SetPosition(0,0,1);//设观察对象位
    aCamera->SetFocalPoint(0,0,0);
    aCamera->ComputeViewPlaneNormal();
    resultRenderder->SetActiveCamera(aCamera);
    resultRenderder->ResetCamera();//特别耗时
    aCamera->Dolly(1.5);
    resultRenderder->SetBackground(0.0, 0.0, 0.0);//黑色背景色
    resultRenderder->ResetCameraClippingRange();
    ui->resultQVTKViewer->GetRenderWindow()->Render();
}

void mainwindowInterator::on_action_SurfaceRender_triggered()//未完成
{
    progDlg = new QProgressDialog();
    progDlg->setWindowTitle("Please wait...");  
    progDlg->setRange(0, 100);
    progDlg->show();
    timer = new QTimer();
    currentValue = 0;
    progDlg->setValue(currentValue);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateProgressDialog()));
    timer->start(100);//开启一个没有终点的定时器

    //显示logo
    if(ui->logoLable == NULL)
    {
        QLabel* logo = new QLabel(this);
        logo->setPixmap(QPixmap("../data/logo/logo.jpg"));
        logo->setFixedSize(260, 300);
        ui->controlPanelLayout->addWidget(logo);
    }
    clear();//删除已经实例化的类的对象
    //面绘制时间长，长时间显示上一个操作结果不合适
    clearResultViewer();
    //删除2D & 3D图片基本属性控件
    if(img2DPropertywidget->isVisible())
    {
        img2DPropertywidget->setVisible(false);
        ui->controlPanelLayout->removeWidget(img2DPropertywidget);//没效果，要修改2015.5.15
        ui->controlPanelLayout->update();
    }
    if(img3DPropertywidget->isVisible())//如果控件已经可见，则移走控件
    {
        img3DPropertywidget->setVisible(false);
        ui->controlPanelLayout->removeWidget(img3DPropertywidget);//没效果，要修改2015.5.15
        ui->controlPanelLayout->update();
    }
    //输入面绘制图像
    initVisualizationPipeline();
    vtkVolume16Reader *v16 = vtkVolume16Reader::New();
    v16->SetDataDimensions (64, 64);			//设置切片的尺寸
    v16->SetImageRange (1, 93);				//设置带读取的文件范围
    v16->SetDataByteOrderToLittleEndian();	//设置为小端字节序
    v16->SetFilePrefix ("../data/headsq/quarter");//设置文件名前缀
    v16->SetDataSpacing (3.2, 3.2, 1.5);		//设置数据间距
    //显示原始数据
    originalVtkViewer->SetInputConnection(v16->GetOutputPort());//可视化管线输入
    originalVtkViewer->SetRenderer(originalRenderder);//设置为同一个渲染场景
    originalVtkViewer->SetRenderWindow(ui->originalQVTKViewer->GetRenderWindow());//设置为同一个渲染窗口
    vtkNextSliceCallbk2* pCall = vtkNextSliceCallbk2::New();
    style = vtkInteractorStyleImage::New();
    style->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall); //可能与预定义交互方式冲突
    style->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
    originalRenWinInteractor->SetInteractorStyle(style);
    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall);//通过交互窗口去监听
    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
    pCall->SetInteractorStyle(style);
    pCall->SetViewer(originalVtkViewer);
    pCall->SetInteractor(originalRenWinInteractor);
    ui->originalQVTKViewer->GetRenderWindow()->SetInteractor(originalRenWinInteractor);
    originalVtkViewer->SetupInteractor(ui->originalQVTKViewer->GetRenderWindow()->GetInteractor());//设置交互方式
    originalVtkViewer->SetSliceOrientationToXY(); //设置切片方向
    originalVtkViewer->SetColorLevel(128); 
    originalVtkViewer->SetColorWindow(256); 
    originalVtkViewer->Render();//数据处于放大状态
    ui->originalQVTKViewer->GetRenderWindow()->Render();

    //对输入数据处理
    vtkContourFilter *skinExtractor = vtkContourFilter::New();//提取等值面
    skinExtractor->SetInputConnection(v16->GetOutputPort());
    skinExtractor->SetValue(0, 500);
    vtkPolyDataNormals *skinNormals = vtkPolyDataNormals::New();//产生法向量，使表面着色更光滑
    skinNormals->SetInputConnection(skinExtractor->GetOutputPort());
    skinNormals->SetFeatureAngle(60.0);
    vtkStripper *stripper = vtkStripper::New(); //产生三角面片，加速渲染过程 
    stripper->SetInput(skinNormals->GetOutput());  
    vtkPolyDataMapper *skinMapper = vtkPolyDataMapper::New();//可视化管线输入
    skinMapper->SetInputConnection(stripper->GetOutputPort());
    skinMapper->ScalarVisibilityOff();
    vtkActor *skin = vtkActor::New();
    skin->SetMapper(skinMapper);
    // 产生一个围绕着数据的框架。
    vtkOutlineFilter *outlineData = vtkOutlineFilter::New();
    outlineData->SetInputConnection(v16->GetOutputPort());
    vtkPolyDataMapper *mapOutline = vtkPolyDataMapper::New();
    mapOutline->SetInputConnection(outlineData->GetOutputPort());
    vtkActor *outline = vtkActor::New();
    outline->SetMapper(mapOutline);
    outline->GetProperty()->SetColor(0,0,0);
    // 设置相机。
    vtkCamera *aCamera = vtkCamera::New();
    aCamera->SetViewUp (0, 0, -1);
    aCamera->SetPosition (0, 1, 0);
    aCamera->SetFocalPoint (0, 0, 0);
    aCamera->ComputeViewPlaneNormal();
    resultRenderder->AddActor(skin);//添加演员
    resultRenderder->AddActor(outline);//添加演员
    resultRenderder->SetActiveCamera(aCamera);
    resultRenderder->ResetCamera ();
    aCamera->Dolly(1.5);// 将相机靠近焦点，以放大图像。
    // 由于Dolly()方法移动了相机，所以要重设相机的剪切范围。
    resultRenderder->ResetCameraClippingRange ();
    resultVtkViewer->GetRenderWindow()->SetInteractor(resultRenWinInteractor);
    resultVtkViewer->Render();
    //timer->stop();
    //delete progDlg;
}

//析构函数
mainwindowInterator::~mainwindowInterator()
{
    delete pGeomtry;
    delete pFourierTrans;
    delete pMorph;
    delete pFilter;
    delete pEdgeContour;
    delete pDicomPro;
    delete pSegmentation;
}