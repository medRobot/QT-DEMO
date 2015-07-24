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
#include <vtkInteractorStyleImage.h>//���Բ�ͬ�Ľ������� 
#include <vtkRenderer.h>
#include <vtkCommand.h>
#include <vtkImageFlip.h>//ͼ��ת
#include <QVTKWidget.h>
#include <vtkImageActor.h>
#include <vtkImageReader2Factory.h>//֧��PNG, BMP, TIFF, JPG
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
//�����
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
    //��ʼ����Ķ���
    pGeomtry = NULL;//���ڹ��캯����ʵ��������Ķ���
    pMorph = NULL;
    pFilter = NULL;
    pPointOpe = NULL;
    pFourierTrans = NULL;
    pEdgeContour = NULL;
    pDicomPro = NULL;
    pSegmentation = NULL;
    pRegistration = NULL;

    //��ʼ������
    pCall = NULL;

    //ʵ����
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

    //��������ʼ��
    createControlPanel();
}

//ͳһ����
//���ݲ����� fileName, pixmap ����ָ�봫�ݲ����������¼���ͼƬʱ���ܼ�ʱ���£�
//�����Ա������ inputFileName, inputImage, pix
//�Ա������ߺ�������һ��Ҫ�淶�� ��ֹgetQpixmap(QPixmap*) ����д�� getQPixmap(QPixmap*), 
//һ���źŶ�Ӧ�����, �����źź󣬶�Ӧ�۶���ִ��һ�飬���ִֻ��ĳһ���ۣ���һ���źŶ�Ӧһ���ۣ�

void mainwindowInterator::initVisualizationPipeline()//�ٴδ�ͼ��ʱͬʱ����
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
        //originalVtkViewer->RemoveObserver(pCall);//��3D��2D����Ҫ��ҳ
        //originalVtkViewer->Delete();
        //���ɾ��originalVtkViewer�����´���ʾ3Dͼ����ʾ2Dͼ���������ֽ����¼� 2015.5.22
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
    //originalVtkViewer->SetRenderer(originalRenderder);//��ʼ�������Ӹô��룬��vtk����
    //resultVtkViewer->SetRenderer(resultRenderder);
}

//�����ڽ����ͼ���£� Ҳ����ר��дһ�������ڸ�����ͼ
void mainwindowInterator::updateResultViewer(char* fileName)
{
    outputFileName = fileName;//�����������ΪͼƬ
    //�ж��ļ���׺
    string fileExtension;
    fileExtension = fileName;//char* to string
    fileExtension = fileExtension.substr(fileExtension.length() - 3, fileExtension.length());
    //���ڿ����Ƿ�ִ��clearResultViewer()  
    if(!ui->resultQVTKViewer->isEnabled())
        ui->resultQVTKViewer->setEnabled(true);

    //������ͼ
    if(fileExtension == "mhd" || fileExtension == "MHD")//3D
    {
        mhdReader = vtkMetaImageReader::New();
        mhdReader->SetFileName(fileName);
        mhdReader->Update(); 
        //ͳ�������С�Ҷ�ֵ �������������ͣ�������Ӧ���ô���&��λ
        //���Ҷ�ֵ���ڸ÷�Χ�����ֵʱ�����԰�Ӱ��ʾ�������ڸ÷�Χʱ����ʾΪ��ɫ
        pixelType =  mhdReader->GetDataScalarType();//�����������
        rotate = vtkImageFlip::New();//ͼ��ת
        rotate->SetInputConnection(mhdReader->GetOutputPort());
        rotate->SetFilteredAxis(1);//y�ᷴת
        resultVtkViewer->SetInputConnection(rotate->GetOutputPort());

        //����
        ui->resultQVTKViewer->GetRenderWindow()->AddRenderer(resultRenderder);
        resultVtkViewer->SetRenderer(resultRenderder);//����Ϊͬһ����Ⱦ����
        resultVtkViewer->SetRenderWindow(ui->resultQVTKViewer->GetRenderWindow());//����Ϊͬһ����Ⱦ����
        pCall = vtkNextSliceCallbk::New();
        style = vtkInteractorStyleImage::New();
        style->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall); //������Ԥ���彻����ʽ��ͻ
        style->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
        resultRenWinInteractor->SetInteractorStyle(style);
        resultRenWinInteractor->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall);//ͨ����������ȥ����
        resultRenWinInteractor->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
        pCall->SetInteractorStyle(style);
        pCall->SetViewer(originalVtkViewer, resultVtkViewer);
        pCall->SetInteractor(resultRenWinInteractor);
        ui->resultQVTKViewer->GetRenderWindow()->SetInteractor(resultRenWinInteractor);
        resultVtkViewer->SetupInteractor(ui->resultQVTKViewer->GetRenderWindow()->GetInteractor());//���ý�����ʽ
        resultVtkViewer->SetSliceOrientationToXY(); //������Ƭ����
        if(pixelType == VTK_UNSIGNED_SHORT || pixelType == VTK_SHORT)
        {
            imgDepth = 16;//ͼ�����
            resultVtkViewer->SetColorLevel(32768);//��λ 
            resultVtkViewer->SetColorWindow(65536);//���� 
        }
        else//VTK_UNSIGNED_CHAR 
        {
            imgDepth = 8;//ͼ�����
            resultVtkViewer->SetColorLevel(128); 
            resultVtkViewer->SetColorWindow(256); 
        }
        //�Զ����Ƴ�ͼ��
        resultVtkViewer->Render();
        resultRenWinInteractor->Initialize();
        resultRenWinInteractor->Start();//�ú����ᵼ������Ĵ��벻�����ִ��
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
        //��ʾ��ɫͼ��3ͨ������Ҫ���ô���&��λ
        if(img->nChannels != 3)//imgΪ����ͼ��
        {
            resultVtkViewer->SetColorLevel(128);//��άͼ�Ҷȷ�Χ0~255
            resultVtkViewer->SetColorWindow(256);//��ʾ2D, 3Dͼ���л�ʱҪ�������ô���&��λ
        }
        resultVtkViewer->SetSliceOrientationToXY(); //Ĭ�Ͼ�����������
        resultVtkViewer->GetSlice();
        resultVtkViewer->GetImageActor()->InterpolateOff();  
        resultRenderder->ResetCamera();  
        resultRenderder->DrawOn();  
        ui->resultQVTKViewer->GetRenderWindow()->Render();
    }
}

//����PED�ָ�Ա���̬ѧ�仯ǰ����ͼ
void mainwindowInterator::updateOriginalViewer(char* fileName)
{
    //�ж��ļ���׺
    string fileExtension;
    fileExtension = fileName;//char* to string
    fileExtension = fileExtension.substr(fileExtension.length() - 3, fileExtension.length());

    if(fileExtension == "mhd" || fileExtension == "MHD")
    {
        mhdReader = vtkMetaImageReader::New();
        mhdReader->SetFileName(fileName);
        mhdReader->Update();
        pixelType =  mhdReader->GetDataScalarType();//�����������
        rotate = vtkImageFlip::New();//ͼ��ת
        rotate->SetInputConnection(mhdReader->GetOutputPort());
        rotate->SetFilteredAxis(1);//y�ᷴת
        originalVtkViewer->SetInputConnection(rotate->GetOutputPort());
    }

    //����
    ui->originalQVTKViewer->GetRenderWindow()->AddRenderer(originalRenderder);
    originalVtkViewer->SetRenderer(originalRenderder);//����Ϊͬһ����Ⱦ����
    originalVtkViewer->SetRenderWindow(ui->originalQVTKViewer->GetRenderWindow());//����Ϊͬһ����Ⱦ����

    pCall = vtkNextSliceCallbk::New();
    style = vtkInteractorStyleImage::New();
    style->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall); 
    style->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
    originalRenWinInteractor->SetInteractorStyle(style);

    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall);//ͨ����������ȥ����
    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);

    pCall->SetInteractorStyle(style);
    pCall->SetViewer(originalVtkViewer, resultVtkViewer);
    pCall->SetInteractor(originalRenWinInteractor);

    ui->originalQVTKViewer->GetRenderWindow()->SetInteractor(originalRenWinInteractor);
    originalVtkViewer->SetupInteractor(ui->originalQVTKViewer->GetRenderWindow()->GetInteractor());//���ý�����ʽ
    originalVtkViewer->SetSliceOrientationToXY(); //������Ƭ����
    //�Զ����Ƴ�ͼ��
    //��Ҫ��start(), �����������䲻��ִ��
    ui->originalQVTKViewer->GetRenderWindow()->Render();
}

void mainwindowInterator::updateProgressDialog()
{
    currentValue++;  
    if( currentValue == 100 )  
        currentValue = 0;  
   progDlg ->setValue(currentValue);
   QCoreApplication::processEvents();//������涳��
   if(progDlg->wasCanceled()) 
   {
        //timer->stop();
        //progDlg->hide();
        //delete progDlg;
   }
}

//�����������
void mainwindowInterator::createControlPanel()
{
    //�ô��봴���ؼ����ؼ��Ƚ��٣�
    //��2Dͼ
    img2DPropertywidget = new QWidget();
    img2DHeightName = new QLabel();
    img2DHeightNum = new QLabel();
    img2DWidthName = new QLabel();
    img2DWidthNum = new QLabel();
    img2DDepthName = new QLabel();
    img2DDepthNum = new QLabel();
    img2DChannelsName = new QLabel();
    img2DChannelsNum = new QLabel();

    img2DHeightName->setText(tr("ͼ��߶�(pix)"));
    img2DWidthName->setText(tr("ͼ����(pix)"));
    img2DDepthName->setText(tr("ͼ�����(λ)"));
    img2DChannelsName->setText(tr("ͼ��ͨ����(��)"));

    QGridLayout* img2DPropertyLayout = new QGridLayout();
    img2DPropertyLayout->addWidget(img2DWidthName, 0, 0, 0);//���һ���������ö��뷽ʽ
    img2DPropertyLayout->addWidget(img2DWidthNum, 0, 1, 0); 
    img2DPropertyLayout->addWidget(img2DHeightName, 1, 0, 0); //�� x ��
    img2DPropertyLayout->addWidget(img2DHeightNum, 1, 1, 0); 
    img2DPropertyLayout->addWidget(img2DDepthName, 2, 0,  0); 
    img2DPropertyLayout->addWidget(img2DDepthNum, 2, 1,  0); 
    img2DPropertyLayout->addWidget(img2DChannelsName, 3, 0,  0); 
    img2DPropertyLayout->addWidget(img2DChannelsNum, 3, 1,  0); 

    img2DPropertywidget->setLayout(img2DPropertyLayout);
    img2DPropertywidget->setFixedSize(260, 110);//��С���ÿ��ƣ�ѡ��UI���
    img2DPropertywidget->setVisible(false);

    //��3Dͼ
    img3DPropertywidget = new QWidget();
    img3DLengthName = new QLabel();
    img3DLengthNum = new QLabel();
    img3DWidthName = new QLabel();
    img3DWidthNum = new QLabel();
    img3DHeightName = new QLabel();
    img3DHeightNum = new QLabel();
    img3DDepthName = new QLabel();
    img3DDepthNum = new QLabel();

    img3DLengthName->setText(tr("ͼ�񳤶�(pix)"));
    img3DWidthName->setText(tr("ͼ����(pix)"));
    img3DHeightName->setText(tr("ͼ��߶�(slice)"));
    img3DDepthName->setText(tr("ͼ�����(λ)"));

    QGridLayout* img3DPropertyLayout = new QGridLayout();
    img3DPropertyLayout->addWidget(img3DLengthName, 0, 0, 0);//��
    img3DPropertyLayout->addWidget(img3DLengthNum, 0, 1, 0); 
    img3DPropertyLayout->addWidget(img3DWidthName, 1, 0, 0); //��
    img3DPropertyLayout->addWidget(img3DWidthNum, 1, 1, 0); 
    img3DPropertyLayout->addWidget(img3DHeightName, 2, 0, 0); //��
    img3DPropertyLayout->addWidget(img3DHeightNum, 2, 1, 0); 
    img3DPropertyLayout->addWidget(img3DDepthName, 3, 0,  0); //��������
    img3DPropertyLayout->addWidget(img3DDepthNum, 3, 1,  0); 

    img3DPropertywidget->setLayout(img3DPropertyLayout);
    img3DPropertywidget->setFixedSize(260, 110);//��С���ÿ��ƣ�ѡ��UI���
    img3DPropertywidget->setVisible(false);
}

//���¼���ͼƬ����ִ����������ʱ�������һ�ν��ͼ
void mainwindowInterator::clearResultViewer()  
{
    if(ui->resultQVTKViewer->isEnabled())//�½�һ��vtkRender����һ����̨�������
    {
        ui->resultQVTKViewer->GetRenderWindow()->RemoveRenderer(resultRenderder);
        resultRenderder = vtkRenderer::New();
        ui->resultQVTKViewer->GetRenderWindow()->AddRenderer(resultRenderder);
        ui->resultQVTKViewer->GetRenderWindow()->Render();
    }
}

//���¼���ͼƬ�����ÿ������
void mainwindowInterator::resetControlPanel() //�������ui�������
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

void mainwindowInterator::clear()//ɾ���Ѿ�ʵ�����������
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
        pFilter = NULL;//��pFilter = NULL, �޷����ø����г�Ա����
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

//�ļ��˵���
void mainwindowInterator::on_action_Open_triggered()
{
    clearResultViewer(); //�����һ�β����Ľ��ͼ
    resetControlPanel(); //���ÿ������
    initVisualizationPipeline();//���ÿ��ӻ�����
    //����ͼ��
    QString filter;
    filter = tr("��άͼ�� (*.jpg *.JPG *.jpeg *.JPEG *.bmp *.BMP *.png *.PNG *.tiff *.TIFF)");
    qstrInputFileName = QFileDialog::getOpenFileName(this, tr("��ͼ��"), QDir::currentPath(), filter);//��ȡ��ǰ·��
    if (qstrInputFileName.isEmpty()) 
    {
        QMessageBox::warning(this, tr("��ʾ��Ϣ�Ի���"), tr("����ͼƬ·��Ϊ�գ�������ѡ��!"), QMessageBox::Ok);
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
            pixelType = originalReader->GetDataScalarType();//��������
            dimension = 2;//ά��
        }
        else
        {
            QMessageBox::warning(this, tr("��ʾ��Ϣ�Ի���"),
                tr("����ͼƬ��ʽ�ݲ�֧�֣�������ѡ��!Ŀǰ֧�ֵ�ͼƬ��ʽΪJPEG, JPG, PNG, BMP, TIFF"), 
                QMessageBox::Ok);
            return;
        }
        //���µ�ǰͼƬ��������
        img = cvLoadImage(inputFileName, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
        img2DWidthNum->setText(QString::number(img->width, 10));//int ->QString 
        img2DHeightNum->setText(QString::number(img->height, 10));//int ->QString
        img2DDepthNum->setText(QString::number(img->depth, 10));//int ->QString
        img2DChannelsNum->setText(QString::number(img->nChannels, 10));//OpenCV��ͨ������װ
        if(!img2DPropertywidget->isVisible())//����ؼ��Ѿ��ɼ�����ֱ�Ӹ���
        {
            ui->controlPanelLayout->addWidget(img2DPropertywidget);//��ʾͼƬ��������
            img2DPropertywidget->setVisible(true);
        }
        originalVtkViewer->UpdateDisplayExtent();//����
        originalVtkViewer->SetInputConnection(originalReader->GetOutputPort());
        originalVtkViewer->SetRenderWindow(ui->originalQVTKViewer->GetRenderWindow());  
        originalVtkViewer->SetRenderer(originalRenderder);  
        originalVtkViewer->SetupInteractor(ui->originalQVTKViewer->GetRenderWindow()->GetInteractor());  
        //��ʾ��ɫͼ���������ô�λ&����
        if(img->nChannels != 3)//�Ҷ�ͼ���ô�λ&����
        {
            originalVtkViewer->SetColorLevel(128);//��άͼ�Ҷȷ�Χ0~255
            originalVtkViewer->SetColorWindow(256);//��ʾ2D, 3Dͼ���л�ʱҪ�������ô���&��λ
        }
        originalVtkViewer->SetSliceOrientationToXY(); //Ĭ�Ͼ�����������  
        originalVtkViewer->GetImageActor()->InterpolateOff();  
        originalRenderder->ResetCamera();  
        originalRenderder->DrawOn(); 
        //originalVtkViewer->Render();//ͼ��Ϊ�Ŵ�״̬
        ui->originalQVTKViewer->GetRenderWindow()->Render();
    }
    //ɾ���Ѿ�ʵ�����������
    clear();//�����������ǰ�棬����ܳ��ֿ����������ʧ�ٳ��ֵ������Ӧ����
    //ɾ��logo
    if(ui->logoLable != NULL)
    {
        delete ui->logoLable;
        ui->logoLable = NULL;
    }
    //ɾ��3DͼƬ�������Կؼ�
    if(img3DPropertywidget->isVisible())
    {
        img3DPropertywidget->setVisible(false);
        ui->controlPanelLayout->removeWidget(img3DPropertywidget);//don't work
        //ui->controlPanelLayout->removeItem(ui->controlPanelLayout);//ɾ������
        ui->controlPanelLayout->update();
    }
}

void mainwindowInterator::on_action_Open_3DImage_triggered()
{
    clearResultViewer(); //�����һ�β����Ľ��ͼ
    resetControlPanel(); //���ÿ������
    initVisualizationPipeline();

    //����ͼ��
    QString filter;
    filter = tr("��άͼ�� (*.mhd *.raw *.vtk)");
    QString fileName = QFileDialog::getOpenFileName( this, QString(tr("��ͼ��")), QDir::currentPath() , filter );
    if ( fileName.isEmpty() == true ) return;
    // QString to char*
    ba = fileName.toLocal8Bit();
    inputFileName = ba.data();
    //�ж��ļ���׺
    string fileExtension;
    fileExtension = inputFileName;//char* to string
    fileExtension = fileExtension.substr(fileExtension.length() - 3, fileExtension.length());

    if(fileExtension == "mhd" || fileExtension == "MHD")
    {
        mhdReader = vtkMetaImageReader::New();
        mhdReader->SetFileName(inputFileName);
        mhdReader->Update();//cannot delete
        //ͳ�������С�Ҷ�ֵ �������������ͣ�������Ӧ���ô���&��λ
        //���Ҷ�ֵ���ڸ÷�Χ�����ֵʱ�����԰�Ӱ��ʾ�������ڸ÷�Χʱ����ʾΪ��ɫ
        pixelType =  mhdReader->GetDataScalarType();//�����������
        //�������ά��
        dimension = mhdReader->GetFileDimensionality();
        imgSize = mhdReader->GetOutput()->GetExtent();//��ʾͼ��ߴ�
        rotate = vtkImageFlip::New();//ͼ��ת
        rotate->SetInputConnection(mhdReader->GetOutputPort());
        rotate->SetFilteredAxis(1);//y�ᷴת
        originalVtkViewer->SetInputConnection(rotate->GetOutputPort());
    }
    else  if(fileExtension == "vtk" || fileExtension == "VTK")//�л���Ƭ�����г��ֶ�ջ���
    {
        vtkReader = vtkStructuredPointsReader::New();
        vtkReader->SetFileName(inputFileName);
        vtkReader->Update();//cannot delete
        //��ʾͼ��ߴ�, û�ɹ�
        int dim[6] = {0, 180, 0, 216, 0, 180};
        imgSize = dim;
        pixelType = VTK_UNSIGNED_CHAR;//��֪��vtk�������ͣ����Ҷ�ֵ��Χһ��Ϊ0~255
        dimension = 3;
        //����Ҫ��ת
        originalVtkViewer->SetInputConnection(vtkReader->GetOutputPort());
    }
    else  if(fileExtension == "raw" || fileExtension == "RAW")//raw���ݶ�������������ά�ȣ��ߴ������
    {
        rawReader = vtkImageReader::New();
        rawReader->SetFileName(inputFileName);
        rawReader->Update();//cannot delete
        pixelType =  rawReader->GetDataScalarType();//�����������
        rawReader->SetFileDimensionality(3); //������ʾͼ���ά��
        rawReader->SetDataScalarType(pixelType);
        rawReader->SetDataExtent(0, 511, 0, 479, 0, 63);//Ҫ�ֶ����óߴ磬��û���ձ�ʵ����
        rawReader->SetDataSpacing(1, 1, 1);
        imgSize = rawReader->GetDataExtent();
        dimension = rawReader->GetFileDimensionality();
        originalVtkViewer->SetInputConnection(rawReader->GetOutputPort());
    }
    else
    {
        QMessageBox::warning(this, tr("��ʾ��Ϣ�Ի���"),
            tr("����ͼƬ��ʽ�ݲ�֧�֣�������ѡ��!Ŀǰ֧�ֵ�ͼƬ��ʽΪMHD, RAW, VTK"), QMessageBox::Ok);
        return;
    }

    //����
    originalVtkViewer->UpdateDisplayExtent();//����, ûЧ��
    ui->originalQVTKViewer->GetRenderWindow()->AddRenderer(originalRenderder);
    originalVtkViewer->SetRenderer(originalRenderder);//����Ϊͬһ����Ⱦ����
    originalVtkViewer->SetRenderWindow(ui->originalQVTKViewer->GetRenderWindow());//����Ϊͬһ����Ⱦ����
    pCall = vtkNextSliceCallbk::New();
    style = vtkInteractorStyleImage::New();
    style->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall); //������Ԥ���彻����ʽ��ͻ
    style->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
    originalRenWinInteractor->SetInteractorStyle(style);
    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall);//ͨ����������ȥ����
    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
    pCall->SetInteractorStyle(style);
    pCall->SetViewer(originalVtkViewer, resultVtkViewer);
    pCall->SetInteractor(originalRenWinInteractor);
    ui->originalQVTKViewer->GetRenderWindow()->SetInteractor(originalRenWinInteractor);
    originalVtkViewer->SetupInteractor(ui->originalQVTKViewer->GetRenderWindow()->GetInteractor());//���ý�����ʽ
    originalVtkViewer->SetSliceOrientationToXY(); //������Ƭ����
    if(pixelType == VTK_UNSIGNED_SHORT || pixelType == VTK_SHORT)
    {
        imgDepth = 16;//ͼ�����
        originalVtkViewer->SetColorLevel(32768);//��λ 
        originalVtkViewer->SetColorWindow(65536);//���� 
    }
    else//VTK_UNSIGNED_CHAR�� ����vtk�������Ͳ�֪��, AtlasSegmentation.vtk�Ҷ�ֵΪ1�� 2�� 3
    {
        imgDepth = 8;//ͼ�����
        originalVtkViewer->SetColorLevel(128); 
        originalVtkViewer->SetColorWindow(256); 
    }
    //���µ�ǰͼƬ��������
    img3DLengthNum->setText(QString::number(imgSize[1] + 1, 10));//int ->QString 
    img3DWidthNum->setText(QString::number(imgSize[3] + 1, 10));//int ->QString
    img3DHeightNum->setText(QString::number(imgSize[5] + 1, 10));//int ->QString
    img3DDepthNum->setText(QString::number(imgDepth, 10));//int ->QString
    if(!img3DPropertywidget->isVisible())//����ؼ��Ѿ��ɼ�����ֱ�Ӹ���
    {
        ui->controlPanelLayout->addWidget(img3DPropertywidget);//��ʾͼƬ��������
        img3DPropertywidget->setVisible(true);
    }
    //ɾ���Ѿ�ʵ�����������
    clear();//�����������ǰ�棬����ܳ��ֿ����������ʧ�ٳ��ֵ������Ӧ����
    //ɾ��logo
    if(ui->logoLable != NULL)
    {
        delete ui->logoLable;
        ui->logoLable = NULL;
    }
    //ɾ��2DͼƬ�������Կؼ�
    if(img2DPropertywidget->isVisible())
    {
        img2DPropertywidget->setVisible(false);
        ui->controlPanelLayout->removeWidget(img2DPropertywidget);//ûЧ����Ҫ�޸�2015.5.15
        ui->controlPanelLayout->update();
    }
    //�Զ����Ƴ�ͼ��

    originalRenderder->ResetCamera();
    originalRenderder->Render();
    //originalRenderder->DrawOn();
    ui->originalQVTKViewer->GetRenderWindow()->Render();
}

void mainwindowInterator::on_action_OpenDICOM_triggered()
{
    //��ʱֻ֧�ֶ���dicom���ݲ������ݿ⽻����û�ж����ݴ���
    clearResultViewer(); //�����һ�β����Ľ��ͼ
    resetControlPanel(); //���ÿ������
    initVisualizationPipeline();
    //��������ͼ�񣬻�ȡ��ǰ�ļ���·��
    QString fileName = QFileDialog::getExistingDirectory( this, QString(tr("ѡ���ļ���")), QDir::currentPath());
    if ( fileName.isEmpty() == true ) return;
    // QString to char*
    ba = fileName.toLocal8Bit();
    inputFileName = ba.data();
    dicomReader = vtkDICOMImageReader::New();
    dicomReader->SetDirectoryName(inputFileName);//��������Ƭ
    dicomReader->Update();
    dimension = dicomReader->GetFileDimensionality();//dcmͼƬ2D
    pixelType = dicomReader->GetDataScalarType();
    originalVtkViewer->SetInputConnection(dicomReader->GetOutputPort());//���ӻ���������
    originalVtkViewer->UpdateDisplayExtent();//����

    //����
    originalVtkViewer->SetRenderer(originalRenderder);//����Ϊͬһ����Ⱦ����
    originalVtkViewer->SetRenderWindow(ui->originalQVTKViewer->GetRenderWindow());//����Ϊͬһ����Ⱦ����
    pCall = vtkNextSliceCallbk::New();
    style = vtkInteractorStyleImage::New();
    style->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall); //������Ԥ���彻����ʽ��ͻ
    style->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
    originalRenWinInteractor->SetInteractorStyle(style);
    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall);//ͨ����������ȥ����
    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
    pCall->SetInteractorStyle(style);
    pCall->SetViewer(originalVtkViewer, resultVtkViewer);
    pCall->SetInteractor(originalRenWinInteractor);
    ui->originalQVTKViewer->GetRenderWindow()->SetInteractor(originalRenWinInteractor);
    originalVtkViewer->SetupInteractor(ui->originalQVTKViewer->GetRenderWindow()->GetInteractor());//���ý�����ʽ
    originalVtkViewer->SetSliceOrientationToXY(); //������Ƭ����
    originalVtkViewer->SetColorLevel(128); 
    originalVtkViewer->SetColorWindow(256); 
    //ɾ��logo
    if(ui->logoLable != NULL)
    {
        delete ui->logoLable;
        ui->logoLable = NULL;
    }
    //ɾ��2D & 3DͼƬ�������Կؼ�
    if(img2DPropertywidget->isVisible())
    {
        img2DPropertywidget->setVisible(false);
        ui->controlPanelLayout->removeWidget(img2DPropertywidget);//ûЧ����Ҫ�޸�2015.5.15
        ui->controlPanelLayout->update();
    }
    if(img3DPropertywidget->isVisible())//����ؼ��Ѿ��ɼ��������߿ؼ�
    {
        img3DPropertywidget->setVisible(false);
        ui->controlPanelLayout->removeWidget(img3DPropertywidget);//ûЧ����Ҫ�޸�2015.5.15
        ui->controlPanelLayout->update();
    }
    clear();//ɾ���Ѿ�ʵ��������Ķ��󣬱����ظ�����
    pDicomPro = new dicomProcess(this);
    ui->controlPanelLayout->addWidget(pDicomPro);//���ÿ������
    pDicomPro->setFixedSize(330, 600);//�������ߴ�
    pDicomPro->inputFileName = this->inputFileName;//ITK��ȡ����
    pDicomPro->showImgProperty(dicomReader); //���µ�ǰͼƬ��������
    //�Զ����Ƴ�ͼ��
    originalVtkViewer->Render();
    //ui->originalQVTKViewer->GetRenderWindow()->Render();
}

//�ж϶���ͼƬ����(�ж��ļ���׺)�������Ӧ������֧�ָ�ͼƬ��ʽ���������ʾ
bool mainwindowInterator::isSurpported2DImgType()
{
    string fileExtension;
    fileExtension = inputFileName;//char* to string
    fileExtension = fileExtension.substr(fileExtension.length() - 3, fileExtension.length());
    if(fileExtension != "jpg" && fileExtension != "peg" && fileExtension != "bmp" 
        && fileExtension != "png" && fileExtension != "iff")//"peg"->"jpeg", "iff"->"tiff"
    {
        QMessageBox::warning(this, tr("��ʾ��Ϣ�Ի���"),
            tr("<p>�ò�����ʱ��֧�ָ�ͼƬ��ʽ��������ѡ���άͼƬ!</p>"
            " <p>Ŀǰ֧�ֶ�άͼƬ��ʽ�У�jpg, jpeg, bmp, tiff, png</p>"),  QMessageBox::Ok);
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
        QMessageBox::warning(this, tr("��ʾ��Ϣ�Ի���"),
            tr("<p>�ò�����ʱ��֧�ָ�ͼƬ��ʽ��������ѡ����άͼƬ!</p>"
            " <p>Ŀǰ֧����άͼƬ��ʽ�У�mhd, raw, dcm, vtk</p>"),  QMessageBox::Ok);
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
        QMessageBox::warning(this, tr("��ʾ��Ϣ�Ի���"),
            tr("<p>�ò�����ʱ��֧�ָ�ͼƬ��ʽ��������ѡ����άͼƬ!</p>"
            " <p>Ŀǰ֧��MHD��άͼƬ��ʽ</p>"),  QMessageBox::Ok);
        return false;
    }
    return true;
}

void mainwindowInterator::on_action_Save_triggered()
{
    //��������������Ķ�����ú����� Ҫ����
    //�����OpenCV�洢ͼƬ������Ҫָ������
    QString outputFileName = QFileDialog::getSaveFileName(this, tr("�����ļ�"), "F:", tr("ͼƬ�ļ�(*png *jpg *bmp *tiff )"));
    QPixmap* pix = new QPixmap();
    QString qstr= QString(QLatin1String(inputFileName));//��updateResultViewer()�л�ȡ
    pix->load(qstr);
    pix->save(outputFileName);
}

//������˵���
void mainwindowInterator::on_action_Color2Gray_triggered()
{
    //�����֧�ָ�ͼƬ���ͣ��򷵻�
    if(!isSurpported2DImgType())
    {
        return;
    }
    //ͨ�����ó�Ա�������ݲ��������ź�&������
    clear();//ɾ���Ѿ�ʵ��������Ķ��󣬱����ظ�����
    pPointOpe = new pointOperation();
    //��������Ҫ�ڷ����ź�֮ǰ
    connect(pPointOpe, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));//���������ڸ�����ͼ
    pPointOpe->color2Gray(inputFileName);
}

void mainwindowInterator::on_action_HistgramEqualization_triggered()
{
    //�����֧�ָ�ͼƬ���ͣ��򷵻�
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();//ɾ���Ѿ�ʵ��������Ķ��󣬱����ظ�����
    pPointOpe = new pointOperation();
    //��������Ҫ�ڷ����ź�֮ǰ
    connect(pPointOpe, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));//���������ڸ�����ͼ
    pPointOpe->histgramEqualization(inputFileName);
}

void mainwindowInterator::on_action_Interpolation_triggered()
{
    //�����֧�ָ�ͼƬ���ͣ��򷵻�
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();//ɾ���Ѿ�ʵ��������Ķ��󣬱����ظ�����
    pPointOpe = new pointOperation();
    //��������
    connect(pPointOpe, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));//���������ڸ�����ͼ
    pPointOpe->interpolation(inputFileName);
}

void mainwindowInterator::on_action_GrayHist_triggered()
{
    //�����֧�ָ�ͼƬ���ͣ��򷵻�
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();//ɾ���Ѿ�ʵ��������Ķ��󣬱����ظ�����
    pPointOpe = new pointOperation();
    //��������
    connect(pPointOpe, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));//���������ڸ�����ͼ
    pPointOpe->grayHistgram(inputFileName);
}

void mainwindowInterator::on_action_ColorHist_triggered()
{
    //�����֧�ָ�ͼƬ���ͣ��򷵻�
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();//ɾ���Ѿ�ʵ��������Ķ��󣬱����ظ�����
    pPointOpe = new pointOperation();
    //��������
    connect(pPointOpe, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));//���������ڸ�����ͼ
    pPointOpe->colorHistgram(inputFileName);
}

void mainwindowInterator::on_action_SingleChannel_triggered()
{
    //�����֧�ָ�ͼƬ���ͣ��򷵻�
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();//ɾ���Ѿ�ʵ��������Ķ��󣬱����ظ�����
    pPointOpe = new pointOperation();
    //��������
    connect(pPointOpe, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));//���������ڸ�����ͼ
    pPointOpe->getSingleChannelImg(inputFileName);
}

//���α仯�˵���
void mainwindowInterator::on_action_Geometry_triggered() 
{
    //�����֧�ָ�ͼƬ���ͣ��򷵻�
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();//ɾ���Ѿ�ʵ��������Ķ��󣬱����ظ�����
    pGeomtry = new geometryTransform(this);
    ui->controlPanelLayout->addWidget(pGeomtry);
    pGeomtry->setVisible(true);
    pGeomtry->inputFileName = &qstrInputFileName;//���ݵ�ǰ�򿪵�ͼƬ��geometryTransform����, QString

    //�ڿ�����������þ������
    connect(pGeomtry, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));//������ͼ
    connect(pGeomtry, SIGNAL(reset()), this, SLOT(clearResultViewer()));//���ã���ߴ������¼�����ͼ���ұߴ������
}

//�����仯�˵���
void mainwindowInterator::on_action_Fourier_triggered()//��ɢ����Ҷ�仯
{
    //�����֧�ָ�ͼƬ���ͣ��򷵻�
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();//ɾ���Ѿ�ʵ��������Ķ��󣬱����ظ�����
    pFourierTrans = new fourierTransform();
    //��������
    connect(pFourierTrans, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pFourierTrans->discreteFourierTrans(inputFileName);
}

//��̬ѧ�仯�˵���
//����˵�������������
void mainwindowInterator::on_action_Morphology_triggered()
{
    //�����֧�ָ�ͼƬ���ͣ��򷵻�
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();//ɾ���Ѿ�ʵ��������Ķ��󣬱����ظ�����
    pMorph = new morphology(this);
    ui->controlPanelLayout->addWidget(pMorph);
    pMorph->setVisible(true);
    pMorph->inputFileName = inputFileName;//��ָ�봫�ݵ�ǰ�򿪵�ͼƬ��morphology����

    //�ڿ�����������þ������
    connect(pMorph, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));//������ͼ
    connect(pMorph, SIGNAL(reset()), this, SLOT(clearResultViewer()));//���ã���ߴ������¼�����ͼ���ұߴ������
}

//ͼ����ǿ�˵���
//�˲���������DLLf��װ����ҪUI
//����˵���ִ���˲�����
void mainwindowInterator::on_action_MeanFilter_triggered()
{
    clear();//ɾ���Ѿ�ʵ��������Ķ��󣬱����ظ�����
    pFilter = new itkFilter();
    //��������
    connect(pFilter, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pFilter->meanFilter(inputFileName, dimension, pixelType);
}

void mainwindowInterator::on_action_GaussianFilter_triggered()
{
    clear();
    pFilter = new itkFilter();
    //��������
    connect(pFilter, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pFilter->gaussianFilter(inputFileName, dimension, pixelType);
}

void mainwindowInterator::on_action_BilateralFilter_triggered()
{
    clear();
    pFilter = new itkFilter();
    //��������
    connect(pFilter, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pFilter->bilateralFilter(inputFileName, dimension, pixelType);
}

void mainwindowInterator::on_action_LaplacianFilter_triggered()
{
    clear();
    pFilter = new itkFilter();
    //��������
    connect(pFilter, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pFilter->laplacianFilter(inputFileName, dimension, pixelType);
}

void mainwindowInterator::on_action_CurvatureAnisotropicDiffusionFilter_triggered()
{ 
    clear();
    pFilter = new itkFilter();
    //��������
    connect(pFilter, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pFilter->curvatureAnisotropicDiffusionFilter(inputFileName, dimension, pixelType);   
}

//��Ե�������˵���
void mainwindowInterator::on_action_Edge_triggered()
{
    //�����֧�ָ�ͼƬ���ͣ��򷵻�
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();//�����һ�ο������
    pEdgeContour = new edgeContour();
    //��������
    connect(pEdgeContour, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pEdgeContour->sobelOperator(inputFileName);
}

//ͼ��ָ�˵���
void mainwindowInterator::on_action_RegionGrowSeg_triggered()
{
    QMessageBox::warning(this, tr("��ʾ��Ϣ�Ի���"), tr("�ù�����δʵ��!"), QMessageBox::Ok);
    return;
}

void mainwindowInterator::on_action_WatershedSeg_triggered()
{
    QMessageBox::warning(this, tr("��ʾ��Ϣ�Ի���"), tr("�ù�����δʵ��!"), QMessageBox::Ok);
    return;
}

void mainwindowInterator::on_action_PEDSeg_triggered()
{
    clear();//ɾ���Ѿ�ʵ��������Ķ��󣬱����ظ�����
    pSegmentation = new segmentation();
    pSegmentation->setFixedSize(270, 300);//�ÿؼ�Ҫ��΢��Щ
    ui->controlPanelLayout->addWidget(pSegmentation);
    pSegmentation->inputFileName = inputFileName;//��ָ�봫�ݵ�ǰ�򿪵�ͼƬ��morphology����
    connect(pSegmentation, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));//���½����ͼ
    connect(pSegmentation, SIGNAL(returnInternalFileName(char*)), this, SLOT(updateOriginalViewer(char*)));//���³�ʼ��ͼ
    connect(pSegmentation, SIGNAL(executeRetinalLayerVisualization()), this, SLOT(retinalLayerVisualization()));//����Ĥ�ֲ�
    connect(pSegmentation, SIGNAL(executeAbnormalRegionVisualization()), this, SLOT(abnormalRegionVisualization()));//��������
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
    timer->start(100);//����һ��û���յ�Ķ�ʱ��

    //�½����ӻ�����
    resultRenderder = vtkRenderer::New();
    vtkRenderWindow* renwin = vtkRenderWindow::New();
    renwin->AddRenderer(resultRenderder);
    resultRenWinInteractor = vtkRenderWindowInteractor::New();
    resultRenWinInteractor->SetRenderWindow(renwin);
    ui->resultQVTKViewer->GetRenderWindow()->AddRenderer(resultRenderder);//������
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

                    //��ֵ�㷨����д
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
                        retinalSurfaceExtractor->SetValue(slice, 2);//������ֵ�����ڻ��ߵ���2Ϊ1 С��2Ϊ0 ��ֵͼ���ؽ�.
                        vtkDecimatePro *deci=vtkDecimatePro::New();//����mesh��������Ƭ. 
                        deci->SetInputConnection(retinalSurfaceExtractor->GetOutputPort());  
                        deci->SetTargetReduction(0.7);//��ԭ�ȵ�������Ƭ���ٵ�ԭ���İٷ�֮��ʮ.  
                        vtkSmoothPolyDataFilter *smooth=vtkSmoothPolyDataFilter::New();   
                        smooth->SetInputConnection(deci->GetOutputPort());  
                        smooth->SetNumberOfIterations(100) ;//����Laplaceƽ���ĵ������� 
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
                        //��ʱ�ͷ���Դ
                        retinalSurfaceExtractor->Delete();
                        mapper->Delete();
                        retinalSurfaceNormals->Delete();
                        retinalSurface->Delete();
    }

    vtkCamera *aCamera = vtkCamera::New();
    aCamera->SetViewUp(0,-5,0);//���ӽ�λ�� 
    aCamera->SetPosition(0,0,1);//��۲����λ
    aCamera->SetFocalPoint(0,0,0);
    aCamera->ComputeViewPlaneNormal();
    resultRenderder->SetActiveCamera(aCamera);
    resultRenderder->ResetCamera();//�ر��ʱ
    aCamera->Dolly(1.5);
    resultRenderder->SetBackground(0.0, 0.0, 0.0);//��ɫ����ɫ
    resultRenderder->ResetCameraClippingRange();

    //û��ʵ�ֽ���
    //resultRenWinInteractor->Initialize();
    //resultRenWinInteractor->Start();
    ui->resultQVTKViewer->GetRenderWindow()->Render();
    //timer->stop();
    delete progDlg;
}

void mainwindowInterator::abnormalRegionVisualization()
{
    //�½����ӻ�����
    resultRenderder = vtkRenderer::New();
    vtkRenderWindow* renwin = vtkRenderWindow::New();
    renwin->AddRenderer(resultRenderder);
    resultRenWinInteractor = vtkRenderWindowInteractor::New();
    resultRenWinInteractor->SetRenderWindow(renwin);
    ui->resultQVTKViewer->GetRenderWindow()->AddRenderer(resultRenderder);//������
    ui->resultQVTKViewer->GetRenderWindow()->SetInteractor(resultRenWinInteractor);

    mhdReader = vtkMetaImageReader::New();
    mhdReader->SetFileName("../data/ped/15715_3d_result.mhd");
    //������ɫ
    double R[3],G[3],B[3];
    R[0]=1;R[1]=0;R[2]=0; 
    G[0]=0;G[1]=1;G[2]=0; 
    B[0]=0;B[1]=0;B[2]=1; 
     
    //��ȡ��ֵ��
    for(int i = 1; i < 3; i++)//error
    {
        vtkMarchingCubes *retinalSurfaceExtractor = vtkMarchingCubes::New();
        retinalSurfaceExtractor->SetInputConnection(mhdReader->GetOutputPort());
        retinalSurfaceExtractor->SetValue(i, i + 1);//������ֵ�����ڻ��ߵ���2Ϊ1 С��2Ϊ0 ��ֵͼ���ؽ�.
        vtkDecimatePro *deci=vtkDecimatePro::New();//����mesh��������Ƭ. 
        deci->SetInputConnection(retinalSurfaceExtractor->GetOutputPort());  
        deci->SetTargetReduction(0.7);//��ԭ�ȵ�������Ƭ���ٵ�ԭ���İٷ�֮��ʮ.  
        vtkSmoothPolyDataFilter *smooth=vtkSmoothPolyDataFilter::New();   
        smooth->SetInputConnection(deci->GetOutputPort());  
        smooth->SetNumberOfIterations(100) ;//����Laplaceƽ���ĵ������� 
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
        //��ʱ�ͷ���Դ
        retinalSurfaceExtractor->Delete();
        mapper->Delete();
        retinalSurfaceNormals->Delete();
        retinalSurface->Delete();
    }

    //û��ʵ�ֽ��� 2015.4.23
    vtkCamera *aCamera = vtkCamera::New();
    aCamera->SetViewUp(0, -5, 0);//���ӽ�λ�� 
    aCamera->SetPosition(0, 0, 1);//��۲����λ
    aCamera->SetFocalPoint(0, 0, 0);
    aCamera->ComputeViewPlaneNormal();
    resultRenderder->SetActiveCamera(aCamera);
    //resultRenderder->ResetCamera(); 
    aCamera->Dolly(1.5);
    resultRenderder->SetBackground(0.0, 0.0, 0.0);//��ɫ����ɫ
    resultRenderder->ResetCameraClippingRange();
    ui->resultQVTKViewer->GetRenderWindow()->Render();
    //resultRenWinInteractor->Initialize();
    //resultRenWinInteractor->Start();
}

//ͼ����׼�˵���
void mainwindowInterator::on_action_Registration_2D_triggered()
{
    //�����֧�ָ�ͼƬ���ͣ��򷵻�
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();//�����һ�ο������
    QString filter;
    filter = tr("��άͼ�� (*.jpg *.JPG *.jpeg *.JPEG *.bmp *.BMP *.png *.PNG *.tiff *.TIFF)");
    QString qstrInputFileName2 = QFileDialog::getOpenFileName(this,
        tr("��ѡ��У׼��������ĵڶ���ͼ��"), QDir::currentPath(), filter);//��ȡ��ǰ·��
    if (qstrInputFileName2.isEmpty()) 
    {
        QMessageBox::warning(this, tr("��ʾ��Ϣ�Ի���"), tr("����ͼƬ·��Ϊ�գ�������ѡ��!"), QMessageBox::Ok);
        return;
    }
    char* movingImageName;
    QByteArray ba = qstrInputFileName2.toLatin1(); //QString to char*
    movingImageName = ba.data();
    pRegistration = new registration();
    //��������
    connect(pRegistration, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    //��һ��Ϊ�̶�ͼ�񣬵ڶ���Ϊ�ƶ�ͼ�񣬵ڶ��Ų��յ�һ�Ž�����׼
    pRegistration->itkDemonsRegistration(inputFileName, movingImageName);
}

//ͼ����ӻ��˵���
//����ƻ������ɫ��������������Ҫ��һ��ɫ��ʶĳ���������������ƣ�
void mainwindowInterator::on_action_VolumeRender_triggered() 
{
    isMHDImgType();//��֧��mhd
    //�½����ӻ�����
    resultRenderder = vtkRenderer::New();
    vtkRenderWindow* renwin = vtkRenderWindow::New();
    renwin->AddRenderer(resultRenderder);
    resultRenWinInteractor = vtkRenderWindowInteractor::New();
    resultRenWinInteractor->SetRenderWindow(renwin);
    ui->resultQVTKViewer->GetRenderWindow()->AddRenderer(resultRenderder);//������
    ui->resultQVTKViewer->GetRenderWindow()->SetInteractor(resultRenWinInteractor);

    mhdReader = vtkMetaImageReader::New();
    mhdReader->SetFileName(inputFileName);

    // ������������͸���ȵ�ת�ƺ�����
    vtkPiecewiseFunction *opacityTransferFunction=vtkPiecewiseFunction::New();
    opacityTransferFunction->AddPoint( 20, 0.0);
    opacityTransferFunction->AddPoint(255, 0.7);

    // ������������ɫ��ת�ƺ�����
    vtkColorTransferFunction *colorTransferFunction=vtkColorTransferFunction::New();
    colorTransferFunction->AddRGBPoint(  0.0, 0.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint( 64.0, 1.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(128.0, 0.0, 0.0, 1.0);
    colorTransferFunction->AddRGBPoint(192.0, 0.0, 1.0, 0.0);
    colorTransferFunction->AddRGBPoint(255.0, 0.0, 0.2, 0.0);

    // �������ԣ����а�������ӳ�亯����
    vtkVolumeProperty *volumeProperty=vtkVolumeProperty::New();
    volumeProperty->SetColor(colorTransferFunction);
    volumeProperty->SetScalarOpacity(opacityTransferFunction);
    volumeProperty->ShadeOn();
    volumeProperty->SetInterpolationTypeToLinear();

    // ����Ͷ��ӳ����֪�������Ⱦ���ݡ�
    vtkVolumeRayCastCompositeFunction  *compositeFunction=
        vtkVolumeRayCastCompositeFunction::New();
    vtkVolumeRayCastMapper *volumeMapper=vtkVolumeRayCastMapper::New();
    volumeMapper->SetVolumeRayCastFunction(compositeFunction);
    volumeMapper->SetInputConnection(mhdReader->GetOutputPort());//ֻ������unsigned char or unsigned short

    // vtkVolume�ƹ�ӳ���������Զ��󣬿ɿ��������ݵ�λ�úͳ���
    vtkVolume *volume=vtkVolume::New();
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);
    resultRenderder->AddVolume(volume);

    //û��ʵ�ֽ��� 2015.5.24
    //���������camera, ��ͼ������ʾ
    vtkCamera *aCamera = vtkCamera::New(); 
    aCamera->SetViewUp(0,-5,0);//���ӽ�λ�� 
    aCamera->SetPosition(0,0,1);//��۲����λ
    aCamera->SetFocalPoint(0,0,0);
    aCamera->ComputeViewPlaneNormal();
    resultRenderder->SetActiveCamera(aCamera);
    resultRenderder->ResetCamera();//�ر��ʱ
    aCamera->Dolly(1.5);
    resultRenderder->SetBackground(0.0, 0.0, 0.0);//��ɫ����ɫ
    resultRenderder->ResetCameraClippingRange();
    ui->resultQVTKViewer->GetRenderWindow()->Render();
}

void mainwindowInterator::on_action_SurfaceRender_triggered()//δ���
{
    progDlg = new QProgressDialog();
    progDlg->setWindowTitle("Please wait...");  
    progDlg->setRange(0, 100);
    progDlg->show();
    timer = new QTimer();
    currentValue = 0;
    progDlg->setValue(currentValue);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateProgressDialog()));
    timer->start(100);//����һ��û���յ�Ķ�ʱ��

    //��ʾlogo
    if(ui->logoLable == NULL)
    {
        QLabel* logo = new QLabel(this);
        logo->setPixmap(QPixmap("../data/logo/logo.jpg"));
        logo->setFixedSize(260, 300);
        ui->controlPanelLayout->addWidget(logo);
    }
    clear();//ɾ���Ѿ�ʵ��������Ķ���
    //�����ʱ�䳤����ʱ����ʾ��һ���������������
    clearResultViewer();
    //ɾ��2D & 3DͼƬ�������Կؼ�
    if(img2DPropertywidget->isVisible())
    {
        img2DPropertywidget->setVisible(false);
        ui->controlPanelLayout->removeWidget(img2DPropertywidget);//ûЧ����Ҫ�޸�2015.5.15
        ui->controlPanelLayout->update();
    }
    if(img3DPropertywidget->isVisible())//����ؼ��Ѿ��ɼ��������߿ؼ�
    {
        img3DPropertywidget->setVisible(false);
        ui->controlPanelLayout->removeWidget(img3DPropertywidget);//ûЧ����Ҫ�޸�2015.5.15
        ui->controlPanelLayout->update();
    }
    //���������ͼ��
    initVisualizationPipeline();
    vtkVolume16Reader *v16 = vtkVolume16Reader::New();
    v16->SetDataDimensions (64, 64);			//������Ƭ�ĳߴ�
    v16->SetImageRange (1, 93);				//���ô���ȡ���ļ���Χ
    v16->SetDataByteOrderToLittleEndian();	//����ΪС���ֽ���
    v16->SetFilePrefix ("../data/headsq/quarter");//�����ļ���ǰ׺
    v16->SetDataSpacing (3.2, 3.2, 1.5);		//�������ݼ��
    //��ʾԭʼ����
    originalVtkViewer->SetInputConnection(v16->GetOutputPort());//���ӻ���������
    originalVtkViewer->SetRenderer(originalRenderder);//����Ϊͬһ����Ⱦ����
    originalVtkViewer->SetRenderWindow(ui->originalQVTKViewer->GetRenderWindow());//����Ϊͬһ����Ⱦ����
    vtkNextSliceCallbk2* pCall = vtkNextSliceCallbk2::New();
    style = vtkInteractorStyleImage::New();
    style->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall); //������Ԥ���彻����ʽ��ͻ
    style->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
    originalRenWinInteractor->SetInteractorStyle(style);
    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall);//ͨ����������ȥ����
    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
    pCall->SetInteractorStyle(style);
    pCall->SetViewer(originalVtkViewer);
    pCall->SetInteractor(originalRenWinInteractor);
    ui->originalQVTKViewer->GetRenderWindow()->SetInteractor(originalRenWinInteractor);
    originalVtkViewer->SetupInteractor(ui->originalQVTKViewer->GetRenderWindow()->GetInteractor());//���ý�����ʽ
    originalVtkViewer->SetSliceOrientationToXY(); //������Ƭ����
    originalVtkViewer->SetColorLevel(128); 
    originalVtkViewer->SetColorWindow(256); 
    originalVtkViewer->Render();//���ݴ��ڷŴ�״̬
    ui->originalQVTKViewer->GetRenderWindow()->Render();

    //���������ݴ���
    vtkContourFilter *skinExtractor = vtkContourFilter::New();//��ȡ��ֵ��
    skinExtractor->SetInputConnection(v16->GetOutputPort());
    skinExtractor->SetValue(0, 500);
    vtkPolyDataNormals *skinNormals = vtkPolyDataNormals::New();//������������ʹ������ɫ���⻬
    skinNormals->SetInputConnection(skinExtractor->GetOutputPort());
    skinNormals->SetFeatureAngle(60.0);
    vtkStripper *stripper = vtkStripper::New(); //����������Ƭ��������Ⱦ���� 
    stripper->SetInput(skinNormals->GetOutput());  
    vtkPolyDataMapper *skinMapper = vtkPolyDataMapper::New();//���ӻ���������
    skinMapper->SetInputConnection(stripper->GetOutputPort());
    skinMapper->ScalarVisibilityOff();
    vtkActor *skin = vtkActor::New();
    skin->SetMapper(skinMapper);
    // ����һ��Χ�������ݵĿ�ܡ�
    vtkOutlineFilter *outlineData = vtkOutlineFilter::New();
    outlineData->SetInputConnection(v16->GetOutputPort());
    vtkPolyDataMapper *mapOutline = vtkPolyDataMapper::New();
    mapOutline->SetInputConnection(outlineData->GetOutputPort());
    vtkActor *outline = vtkActor::New();
    outline->SetMapper(mapOutline);
    outline->GetProperty()->SetColor(0,0,0);
    // ���������
    vtkCamera *aCamera = vtkCamera::New();
    aCamera->SetViewUp (0, 0, -1);
    aCamera->SetPosition (0, 1, 0);
    aCamera->SetFocalPoint (0, 0, 0);
    aCamera->ComputeViewPlaneNormal();
    resultRenderder->AddActor(skin);//�����Ա
    resultRenderder->AddActor(outline);//�����Ա
    resultRenderder->SetActiveCamera(aCamera);
    resultRenderder->ResetCamera ();
    aCamera->Dolly(1.5);// ������������㣬�ԷŴ�ͼ��
    // ����Dolly()�����ƶ������������Ҫ��������ļ��з�Χ��
    resultRenderder->ResetCameraClippingRange ();
    resultVtkViewer->GetRenderWindow()->SetInteractor(resultRenWinInteractor);
    resultVtkViewer->Render();
    //timer->stop();
    //delete progDlg;
}

//��������
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