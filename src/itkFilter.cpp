#include "itkFilter.h"
#include <QProgressBar>
#include <QMessageBox>
#include <QProgressDialog>
#include <Windows.h>
#include <string>
using namespace std;

itkFilter::itkFilter(QWidget *parent, Qt::WFlags flags)
    : QWidget(parent, flags)
{

}
//��ɫͼ�˲������˻Ҷ�ͼ
void itkFilter::meanFilter(char* fileName, int dimension, int pixelType)
{
    // ������DLL��װ��
    //ITK���Զ�bmp, png, jpgͼƬ���д���
    HINSTANCE hfilter = LoadLibraryA("filter_dll.dll");
    typedef void(*pFilter)(char* inputFilePath, int dimension, int pixelType); 
    pFilter filter = (pFilter)GetProcAddress(hfilter, "meanFilter_dll");//�ڶ�������Ϊ������
    filter(fileName, dimension, pixelType);
    FreeLibrary(hfilter);

    //�����˲����������·������������DLL��һ��
    string tempPath;
    tempPath = fileName;//char* to string
    tempPath = tempPath.substr(0, tempPath.length() - 4);
    if(dimension == 2)
        tempPath = tempPath + "_meanFilter.jpg";
    else
        tempPath = tempPath + "_meanFilter.mhd";
    strcpy(outputFileName, tempPath.c_str());//string to char*

    emit returnOutputFileName(outputFileName);//�����ź�
}

//��˹�˲��ٶȺ������Ż������滻�ú���
//�����ö��߳�
void itkFilter::gaussianFilter(char* fileName, int dimension, int pixelType) 
{
    HINSTANCE hfilter = LoadLibraryA("filter_dll.dll");
    typedef void(*pFilter)(char* inputFilePath, int dimension, int pixelType); 
    pFilter filter = (pFilter)GetProcAddress(hfilter, "gaussianFilter_dll");//�ڶ�������Ϊ������
    filter(fileName, dimension, pixelType);
    FreeLibrary(hfilter);

    //�����˲����������·������������DLL��һ��
    string tempPath;
    tempPath = fileName;//char* to string
    tempPath = tempPath.substr(0, tempPath.length() - 4);
    if(dimension == 2)
        tempPath = tempPath + "_gaussianFilter.jpg";
    else
        tempPath = tempPath + "_gaussianFilter.mhd"; 
    strcpy(outputFileName, tempPath.c_str());//string to char*

    emit returnOutputFileName(outputFileName);//�����ź�
}

//bilateralFilter��ʱ��֧��2d image
void itkFilter::bilateralFilter(char* fileName, int dimension, int pixelType) 
{
    HINSTANCE hfilter = LoadLibraryA("filter_dll.dll");
    typedef void(*pFilter)(char* inputFilePath, int dimension, int pixelType); 
    pFilter filter = (pFilter)GetProcAddress(hfilter, "bilateralFilter_dll");
    filter(fileName, dimension, pixelType);
    FreeLibrary(hfilter);

    //�����˲����������·������������DLL��һ��
    string tempPath;
    tempPath = fileName;//char* to string
    tempPath = tempPath.substr(0, tempPath.length() - 4);
    if(dimension == 2)
        tempPath = tempPath + "_bilateralFilter.jpg";
    else
    {
        QMessageBox::warning(this, tr("��ʾ��Ϣ�Ի���"),
            tr("˫���˲���ʱ��֧����άͼ��"), QMessageBox::Ok);
        return;
    }
    strcpy(outputFileName, tempPath.c_str());//string to char*

    emit returnOutputFileName(outputFileName);//�����ź�
}

void itkFilter::laplacianFilter(char* fileName, int dimension, int pixelType)
{
    HINSTANCE hfilter = LoadLibraryA("filter_dll.dll");
    typedef void(*pFilter)(char* inputFilePath, int dimension, int pixelType); 
    pFilter filter = (pFilter)GetProcAddress(hfilter, "laplacianFilter_dll");
    filter(fileName, dimension, pixelType);
    FreeLibrary(hfilter);

    //�����˲����������·������������DLL��һ��
    string tempPath;
    tempPath = fileName;//char* to string
    tempPath = tempPath.substr(0, tempPath.length() - 4);
    if(dimension == 2)
        tempPath = tempPath + "_laplacianFilter.jpg";
    else
        tempPath = tempPath + "_laplacianFilter.mhd";
    strcpy(outputFileName, tempPath.c_str());//string to char*

    emit returnOutputFileName(outputFileName);//�����ź�
}

void itkFilter::curvatureAnisotropicDiffusionFilter(char* fileName, int dimension, int pixelType)
{
    HINSTANCE hfilter = LoadLibraryA("filter_dll.dll");
    typedef void(*pFilter)(char* inputFilePath, int dimension, int pixelType); 
    pFilter filter = (pFilter)GetProcAddress(hfilter, "curvatureAnisotropicDiffusionFilter_dll");
    filter(fileName, dimension, pixelType);
    FreeLibrary(hfilter);

    //�����˲����������·������������DLL��һ��
    string tempPath;
    tempPath = fileName;//char* to string
    tempPath = tempPath.substr(0, tempPath.length() - 4);
    if(dimension == 2)
        tempPath = tempPath + "_curvatureAnisotropicDiffusionFilter.jpg";
    else 
        tempPath = tempPath + "_curvatureAnisotropicDiffusionFilter.mhd";
    strcpy(outputFileName, tempPath.c_str());//string to char*

    emit returnOutputFileName(outputFileName);//�����ź�
}

itkFilter::~itkFilter()
{

}

