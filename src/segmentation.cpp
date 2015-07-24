#include "segmentation.h"
#include "itkImage.h"//itk
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h" 
#include "itkBinaryThresholdImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkBinaryOpeningByReconstructionImageFilter.h"
#include "itkSimpleFilterWatcher.h"
#include <vtkImageData.h>//vtk
#include <vtkImageShiftScale.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkContourFilter.h>
#include <vtkDecimatePro.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkStripper.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkVolume.h>
#include <vtkProperty.h>
#define  PI 3.1415926//C++

segmentation::segmentation(QWidget *parent , Qt::WFlags flags)
    :QWidget(parent, flags),
    ui(new Ui::segmentation)
{
    ui->setupUi(this);
}

void segmentation::on_filterButton_clicked()
{
    typedef    float    InputPixelType;
    typedef    float    OutputPixelType;
    typedef itk::Image< InputPixelType,  3 >   InputImageType;
    typedef itk::Image< OutputPixelType, 3>   OutputImageType;
    typedef itk::ImageFileReader< InputImageType >  ReaderType;
    typedef itk::CurvatureAnisotropicDiffusionImageFilter<
        InputImageType, OutputImageType >  FilterType;
    FilterType::Pointer filter = FilterType::New();
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( inputFileName);
    filter->SetInput( reader->GetOutput() );
    const unsigned int numberOfIterations = 3;//5
    const double       timeStep =  0.0625; //timeStep=0.125(for 2D), 0.0625(for 3D)
    const double       conductance = 3;//3
    filter->SetNumberOfIterations( numberOfIterations );
    filter->SetTimeStep( timeStep );
    filter->SetConductanceParameter( conductance );
    filter->Update();//time-costing
    typedef unsigned short                        WritePixelType;//unsigned char
    typedef itk::Image< WritePixelType, 3 >        WriteImageType;
    typedef itk::RescaleIntensityImageFilter<
        OutputImageType, WriteImageType > RescaleFilterType;
    RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
    rescaler->SetOutputMinimum(   0 );
    rescaler->SetOutputMaximum( 65535 );//255 for CNV,  for PED
    typedef itk::ImageFileWriter< WriteImageType >  WriterType;
    WriterType::Pointer writer = WriterType::New();
    //文件前缀名
    filePrefix = inputFileName;//char* to string
    filePrefix = filePrefix.substr(0, filePrefix.length() - 4);
    filePrefix = filePrefix + "_filter.mhd";
    strcpy(outputFileName, filePrefix.c_str());//string to char*
    writer->SetFileName(outputFileName);
    rescaler->SetInput( filter->GetOutput() );
    writer->SetInput( rescaler->GetOutput() );
    writer->Update();

    //将结果图返回主窗口显示
    emit returnOutputFileName(outputFileName);//发出信号
}

void segmentation::on_retinalLayerSegButton_clicked()
{
    fstream fin1, fin7, fin11, fin12; 
    char* s1Txt = ("../data/ped/txt/15715_1.txt");//s1.txt
    char* s7Txt = ("../data/ped/txt/15715_7.txt"); 
    char* s11Txt = ("../data/ped/txt/15715_11.txt"); 
    char* s12Txt = ("../data/ped/txt/15715_12.txt");
    //这样开辟内存容易出现堆栈溢出问题, 选择new & delete
    //int s1[64][512], s7[64][512], s11[64][512], s12[64][512]; 
    int (*s1)[512] = new int [64][512];
    int (*s7)[512] = new int [64][512];
    int (*s11)[512] = new int [64][512];
    int (*s12)[512] = new int [64][512];
    fin1.open(s1Txt, ios::in);
    fin7.open(s7Txt, ios::in);
    fin11.open(s11Txt, ios::in);
    fin12.open(s12Txt, ios::in);
    for(int z=0; z<64; z++)
    {
        for(int x=0; x<512; x++)
        {
            fin1>>s1[z][x];
            fin7>>s7[z][x];
            fin11>>s11[z][x];
            fin12>>s12[z][x];
        }
    }
    fin1.close();
    fin7.close();
    fin11.close();
    fin12.close();

    typedef itk::Image<unsigned short,3>InputImageType; 
    typedef itk::Image<unsigned short,3>OutputImageType; 
    InputImageType::Pointer inputImage = InputImageType::New();
    InputImageType::Pointer outputImage = InputImageType::New();
    typedef itk::ImageFileReader<InputImageType>ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(inputFileName); 
    reader->Update();
    inputImage = reader->GetOutput();
    InputImageType::IndexType voxelIndex;
    InputImageType::SizeType imgSize = inputImage->GetLargestPossibleRegion().GetSize();
    OutputImageType::IndexType index;
    index[0]=0;
    index[1]=0;
    index[2]=0;
    OutputImageType::SizeType size;
    size[0]=imgSize[0];
    size[1]=imgSize[1];
    size[2]=imgSize[2];  
    //create a region for enhance result
    OutputImageType::RegionType region;
    region.SetIndex(index);
    region.SetSize(size);
    outputImage->SetRegions(region);
    outputImage->Allocate();
    //make four surfaces into a MHD file
    for(int z = 0; z < imgSize[2]; z++)
        for(int y = 0; y < imgSize[1]; y++)
            for(int x = 0; x < imgSize[0]; x++)
            {
                voxelIndex[0] = x;
                voxelIndex[1] = y;
                voxelIndex[2] = z;
                if(y == s1[z][x])
                {
                    inputImage->SetPixel(voxelIndex, 65535);
                }
                if(y == s7[z][x])
                {
                    inputImage->SetPixel(voxelIndex, 65535);
                }
                if(y == s11[z][x])
                {
                    inputImage->SetPixel(voxelIndex, 65535);
                }
                if(y == s12[z][x])
                {
                    inputImage->SetPixel(voxelIndex, 65535);
                }
            }
            delete []s1;//释放内存
            delete []s7;
            delete []s11;
            delete []s12;

            //文件前缀名
            filePrefix = inputFileName;//char* to string
            filePrefix = filePrefix.substr(0, filePrefix.length() - 4);
            filePrefix = filePrefix + "_layerSeg.mhd";
            strcpy(outputFileName, filePrefix.c_str());//string to char*
            typedef itk::ImageFileWriter<OutputImageType>WriterType;
            WriterType::Pointer writer = WriterType::New();
            writer->SetFileName(outputFileName);
            writer->SetInput(inputImage);
            writer->Update();
            emit returnOutputFileName(outputFileName);//发出信号
}

void segmentation::on_retinalLayerVisualizationButton_clicked()
{
   emit executeRetinalLayerVisualization();
}

void segmentation::on_abnormalRegionSegButton_clicked()
{
    fstream fin11, fin12; 
    char* s11Txt = ("../data/ped/txt/15715_11.txt");//s11.txt
    char* s12Txt = ("../data/ped/txt/15715_12.txt");
    int (*s11)[512] = new int[64][512];
    int (*s12)[512] = new int[64][512];
    int (*p)[512] = new int[64][512];
    //read TXT
    fin11.open(s11Txt, ios::in); 
    fin12.open(s12Txt, ios::in);
    //get the p matrix
    for(int z = 0; z < 64; z++)
    {
        for(int x = 0; x < 512; x++)
        {
            fin11>>s11[z][x];
            fin12>>s12[z][x];
            if(s12[z][x] - s11[z][x] > 5)  //5 threshold  
                p[z][x] = 1;
            else 
                p[z][x] = 0;
        }
    }
    fin11.close();
    fin12.close();
    //read filtered MHD data
    typedef itk::Image<unsigned short, 3>InputImageType; 
    typedef itk::Image<unsigned short, 3>OutputImageType;
    typedef itk::Image<unsigned char, 3>InitOutputImageType;
    InputImageType::Pointer inputImage = InputImageType::New();
    OutputImageType::Pointer outputImage = OutputImageType::New();
    InitOutputImageType::Pointer initOutputImage = InitOutputImageType::New();
    typedef itk::ImageFileReader<InputImageType>ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(inputFileName);//读入原图像
    reader->Update();
    inputImage = reader->GetOutput();
    InputImageType::IndexType voxelIndex;
    InitOutputImageType::IndexType initvoxelIndex;
    OutputImageType::IndexType newvoxelIndex;
    InputImageType::SizeType imgSize = inputImage->GetLargestPossibleRegion().GetSize();
    OutputImageType::IndexType index;
    index[0] = 0;
    index[1] = 0;
    index[2] = 0;
    OutputImageType::SizeType size;
    size[0] = imgSize[0];
    size[1] = imgSize[1];
    size[2] = imgSize[2];
    //create a region for initial result
    InitOutputImageType::RegionType initRegion;
    initRegion.SetIndex(index);
    initRegion.SetSize(size);
    initOutputImage->SetRegions( initRegion);
    initOutputImage->Allocate();
    //create a region for enhance result
    OutputImageType::RegionType region;
    region.SetIndex(index);
    region.SetSize(size);
    outputImage->SetRegions(region);
    outputImage->Allocate();
    //Initial result for PED segmentation  (a binary image)
    for(int z = 0; z < imgSize[2]; z++)
        for(int x = 0; x < imgSize[0]; x++)
        {
            initvoxelIndex[0] = x;
            initvoxelIndex[2] = z;
            for(int y = 0; y < imgSize[1]; y++)
            {
                //set all background a black region
                initvoxelIndex[1] = y;
                initOutputImage->SetPixel(initvoxelIndex, 0);
            }
            //set the same intensity for all PED region (empirical value)
            if(p[z][x] == 1)
            {
                for(int y = s11[z][x]; y <= s12[z][x]; y++) 
                {
                    initvoxelIndex[1] = y;
                    initOutputImage->SetPixel(initvoxelIndex, 255);//亮区域
                }
            }
        }
        //输出中间分割结果
        //文件前缀名
        filePrefix = inputFileName;//char* to string
        filePrefix = filePrefix.substr(0, filePrefix.length() - 4);
        string strInitFileName;
        strInitFileName = filePrefix + "_initBinaryImg.mhd";
        strcpy(initFileName, strInitFileName.c_str());//string to char*
        typedef itk::ImageFileWriter<InitOutputImageType>InitWriterType;
        InitWriterType::Pointer initWriter = InitWriterType::New();
        initWriter->SetFileName(initFileName);//生成二值图
        initWriter->SetInput(initOutputImage);
        initWriter->Update();
        //Enhance PED region and overlay it on the original image
        for(int z = 0; z < imgSize[2]; z++)
            for(int x = 0; x < imgSize[0]; x++)
            {
                voxelIndex[0] = x;
                voxelIndex[2] = z;
                newvoxelIndex[0] = x;
                newvoxelIndex[2] = z;
                for(int y = 0; y < imgSize[1]; y++)
                {
                    voxelIndex[1] = y;
                    newvoxelIndex[1] = y;
                    outputImage->SetPixel(newvoxelIndex, inputImage->GetPixel(voxelIndex));
                }
                //set the same intensity for all PED region (empirical value)
                if(p[z][x] == 1)
                {
                    for(int y = s11[z][x]; y <= s12[z][x]; y++) 
                    {
                        newvoxelIndex[1] = y;
                        outputImage->SetPixel(newvoxelIndex, 65535);//亮区域
                    }
                }
            }
            //释放内存不能缺省
            delete[]s11;
            delete[]s12;
            delete[]p;

            typedef itk::ImageFileWriter<OutputImageType>WriterType;
            WriterType::Pointer writer = WriterType::New();
            filePrefix = filePrefix + "_initResult.mhd";
            strcpy(outputFileName, filePrefix.c_str());//string to char*
            initResultFileName = outputFileName;//输出初始分割结果，二值图重叠在原图像上面
            writer->SetFileName(outputFileName);
            writer->SetInput(outputImage);
            writer->Update();

            emit returnOutputFileName(outputFileName);//发出信号
}

void segmentation::on_abnormalRegionVisualizationButton_clicked()
{
    emit executeAbnormalRegionVisualization();//发出信号
}

//重构操作
void segmentation::on_morphologyOperationButton_clicked()
{
    const int dimension = 3;
    typedef itk::Image< unsigned char, dimension > IType;
    typedef itk::ImageFileReader< IType > ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    IType::Pointer inputImage = IType::New();

    reader->SetFileName( initFileName );//读入二值图
    reader->Update();
    inputImage = reader->GetOutput();

    IType::SizeType imgSize;
    IType::IndexType voxelIndex;
    imgSize = inputImage->GetLargestPossibleRegion().GetSize();

    typedef itk::BinaryBallStructuringElement< bool, dimension> KernelType;
    KernelType ball;
    KernelType::SizeType ballSize;
    ofstream fout;
    long vol = 0;
    unsigned char temp;
    float r;

    for(int z = 0; z < imgSize[2]; z++)
        for(int y = 0; y < imgSize[1]; y++)
            for(int x = 0; x < imgSize[0]; x++)
            {
                voxelIndex[0] = x;
                voxelIndex[1] = y;
                voxelIndex[2] = z;
                temp = inputImage->GetPixel(voxelIndex);
                if(temp == 255)
                    vol += 1;
            }
            r = pow((3 * vol) / (4 * PI), (1.0 / 3)) ;
            r = r / 6;//experiment data
            ballSize.Fill( r );//radius 
            ball.SetRadius(ballSize);
            ball.CreateStructuringElement();

            typedef itk::BinaryOpeningByReconstructionImageFilter< IType, KernelType > I2LType;
            I2LType::Pointer reconstruction = I2LType::New();
            reconstruction->SetInput( reader->GetOutput() );
            reconstruction->SetKernel( ball );
            reconstruction->SetFullyConnected( 1 );
            reconstruction->SetForegroundValue( 255 );
            itk::SimpleFilterWatcher watcher(reconstruction, "filter");
            //文件前缀名
            filePrefix = inputFileName;//char* to string
            filePrefix = filePrefix.substr(0, filePrefix.length() - 4);
            string morphologyFileName;
            morphologyFileName = filePrefix + "_morphologyResult.mhd";
            strcpy(outputFileName, morphologyFileName.c_str());//string to char*
            typedef itk::ImageFileWriter< IType > WriterType;
            WriterType::Pointer morphologyWriter = WriterType::New();
            morphologyWriter->SetInput( reconstruction->GetOutput() );
            morphologyWriter->SetFileName( outputFileName );
            morphologyWriter->Update();

            //腐蚀结果叠加到原图
            typedef itk::Image< unsigned short,  dimension >   OriginalImageType;
            typedef itk::ImageFileReader<OriginalImageType>OriginalReaderType;
            OriginalReaderType::Pointer orignalImgreader = OriginalReaderType::New();
            OriginalImageType::Pointer originalImage = OriginalImageType::New();
            OriginalReaderType::IndexType originalImgVoxelIndex;
            reader->SetFileName(outputFileName);//读入重构操作结果图像
            reader->Update();
            inputImage = reader->GetOutput();
            orignalImgreader->SetFileName(inputFileName);//读入原图像
            orignalImgreader->Update();
            originalImage = orignalImgreader->GetOutput();
            for(int z = 0; z < imgSize[2]; z++)
                for(int y = 0; y < imgSize[1]; y++)
                    for(int x = 0; x < imgSize[0]; x++)
                    {
                        voxelIndex[0] = x;
                        voxelIndex[1] = y;
                        voxelIndex[2] = z;
                        originalImgVoxelIndex[0] = x;
                        originalImgVoxelIndex[1] = y;
                        originalImgVoxelIndex[2] = z;
                        temp = inputImage->GetPixel(voxelIndex);
                        if(temp == 255)
                            originalImage->SetPixel(originalImgVoxelIndex, 65535);
                    }
                    //输出结果
                    typedef itk::ImageFileWriter<OriginalImageType>NewWriterType;
                    NewWriterType::Pointer writer = NewWriterType::New();
                    //文件前缀名
                    filePrefix = inputFileName;//char* to string
                    filePrefix = filePrefix.substr(0, filePrefix.length() - 4);
                    filePrefix = filePrefix + "_refinedResult.mhd";
                    strcpy(outputFileName, filePrefix.c_str());//string to char*
                    writer->SetFileName(outputFileName);
                    writer->SetInput(originalImage);
                    writer->Update();

                    emit returnInternalFileName(initResultFileName);//更新原视图
                    emit returnOutputFileName(outputFileName);//显示结果图
}

//腐蚀操作
void segmentation::erosionOperation()//未用到
{
    const unsigned int Dimension = 3;
    typedef unsigned char   InputPixelType;
    typedef unsigned char   OutputPixelType;
    typedef itk::Image< InputPixelType,  Dimension >   InputImageType;
    typedef itk::Image< OutputPixelType, Dimension >   OutputImageType;
    typedef itk::ImageFileReader< InputImageType  >  ReaderType;
    typedef itk::ImageFileWriter< OutputImageType >  WriterType;
    typedef itk::BinaryThresholdImageFilter< InputImageType, InputImageType >  ThresholdFilterType;
    typedef itk::BinaryBallStructuringElement< 
        InputPixelType,
        Dimension  >             StructuringElementType;
    typedef itk::BinaryErodeImageFilter<
        InputImageType, 
        OutputImageType,
        StructuringElementType >  ErodeFilterType;
    ReaderType::Pointer reader = ReaderType::New();
    WriterType::Pointer writerErosion  = WriterType::New();
    ThresholdFilterType::Pointer thresholder = ThresholdFilterType::New();
    ErodeFilterType::Pointer  binaryErode  = ErodeFilterType::New();
    StructuringElementType  structuringElementErosion;
    reader->SetFileName( initFileName );//读入二值图
    reader->Update();
    InputImageType::Pointer inputImage = InputImageType::New();
    InputImageType::IndexType voxelIndex;
    inputImage = reader->GetOutput();
    InputImageType::SizeType imgSize = inputImage->GetLargestPossibleRegion().GetSize();
    long vol = 0;
    unsigned char temp;
    float r;
    for(int z = 0; z < imgSize[2]; z++)
        for(int y = 0; y < imgSize[1]; y++)
            for(int x = 0; x < imgSize[0]; x++)
            {
                voxelIndex[0] = x;
                voxelIndex[1] = y;
                voxelIndex[2] = z;
                temp = inputImage->GetPixel(voxelIndex);
                if(temp == 255)// 255 for PED
                    vol += 1;
            }
            r = pow((3 * vol) / (4 * PI), (1.0 / 3)) ;
            r = r / 20;//experiment data
            structuringElementErosion.SetRadius( r );  // 3x3 structuring element
            structuringElementErosion.CreateStructuringElement();
            binaryErode->SetKernel(  structuringElementErosion );
            //文件前缀名
            filePrefix = inputFileName;//char* to string
            filePrefix = filePrefix.substr(0, filePrefix.length() - 4);
            string erosionFileName;
            erosionFileName = filePrefix + "_erosionResult.mhd";
            strcpy(outputFileName, erosionFileName.c_str());//string to char*
            writerErosion->SetFileName(outputFileName);
            const InputPixelType lowerThreshold = 255;
            const InputPixelType upperThreshold = 255;
            thresholder->SetInput( reader->GetOutput() );
            InputPixelType background =   0;
            InputPixelType foreground = 255;
            thresholder->SetOutsideValue( background );
            thresholder->SetInsideValue(  foreground );
            thresholder->SetLowerThreshold( lowerThreshold );
            thresholder->SetUpperThreshold( upperThreshold );
            binaryErode->SetInput( thresholder->GetOutput() );
            binaryErode->SetErodeValue( foreground );
            writerErosion->SetInput( binaryErode->GetOutput() );
            writerErosion->Update();
            //binaryErode->GetOutput()->GetPixel(index);//获取像素值失败

            //腐蚀结果叠加到原图
            typedef itk::Image< unsigned short,  Dimension >   OriginalImageType;
            typedef itk::ImageFileReader<OriginalImageType>OriginalReaderType;
            OriginalReaderType::Pointer orignalImgreader = OriginalReaderType::New();
            OriginalImageType::Pointer originalImage = OriginalImageType::New();
            OriginalReaderType::IndexType originalImgVoxelIndex;
            reader->SetFileName(outputFileName);//读入腐蚀结果图像
            reader->Update();
            inputImage = reader->GetOutput();
            orignalImgreader->SetFileName(inputFileName);//读入原图像
            orignalImgreader->Update();
            originalImage = orignalImgreader->GetOutput();
            for(int z = 0; z < imgSize[2]; z++)
                for(int y = 0; y < imgSize[1]; y++)
                    for(int x = 0; x < imgSize[0]; x++)
                    {
                        voxelIndex[0] = x;
                        voxelIndex[1] = y;
                        voxelIndex[2] = z;
                        originalImgVoxelIndex[0] = x;
                        originalImgVoxelIndex[1] = y;
                        originalImgVoxelIndex[2] = z;
                        temp = inputImage->GetPixel(voxelIndex);
                        if(temp == 255)
                            originalImage->SetPixel(originalImgVoxelIndex, 65535);
                    }
                    //输出结果
                    typedef itk::ImageFileWriter<OriginalImageType>NewWriterType;
                    NewWriterType::Pointer writer = NewWriterType::New();
                    //文件前缀名
                    filePrefix = inputFileName;//char* to string
                    filePrefix = filePrefix.substr(0, filePrefix.length() - 4);
                    filePrefix = filePrefix + "_refinedResult.mhd";
                    strcpy(outputFileName, filePrefix.c_str());//string to char*
                    writer->SetFileName(outputFileName);
                    writer->SetInput(originalImage);
                    writer->Update();

                    emit returnInternalFileName(initResultFileName);//更新原视图
                    emit returnOutputFileName(outputFileName);//显示结果图
}

segmentation::~segmentation()
{

}