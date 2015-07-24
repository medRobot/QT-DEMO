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
//彩色图滤波后变成了灰度图
void itkFilter::meanFilter(char* fileName, int dimension, int pixelType)
{
    // 函数用DLL封装好
    //ITK可以对bmp, png, jpg图片进行处理
    HINSTANCE hfilter = LoadLibraryA("filter_dll.dll");
    typedef void(*pFilter)(char* inputFilePath, int dimension, int pixelType); 
    pFilter filter = (pFilter)GetProcAddress(hfilter, "meanFilter_dll");//第二个参数为函数名
    filter(fileName, dimension, pixelType);
    FreeLibrary(hfilter);

    //加载滤波后结果，输出路径命名规则与DLL中一致
    string tempPath;
    tempPath = fileName;//char* to string
    tempPath = tempPath.substr(0, tempPath.length() - 4);
    if(dimension == 2)
        tempPath = tempPath + "_meanFilter.jpg";
    else
        tempPath = tempPath + "_meanFilter.mhd";
    strcpy(outputFileName, tempPath.c_str());//string to char*

    emit returnOutputFileName(outputFileName);//发出信号
}

//高斯滤波速度很慢，优化或者替换该函数
//考虑用多线程
void itkFilter::gaussianFilter(char* fileName, int dimension, int pixelType) 
{
    HINSTANCE hfilter = LoadLibraryA("filter_dll.dll");
    typedef void(*pFilter)(char* inputFilePath, int dimension, int pixelType); 
    pFilter filter = (pFilter)GetProcAddress(hfilter, "gaussianFilter_dll");//第二个参数为函数名
    filter(fileName, dimension, pixelType);
    FreeLibrary(hfilter);

    //加载滤波后结果，输出路径命名规则与DLL中一致
    string tempPath;
    tempPath = fileName;//char* to string
    tempPath = tempPath.substr(0, tempPath.length() - 4);
    if(dimension == 2)
        tempPath = tempPath + "_gaussianFilter.jpg";
    else
        tempPath = tempPath + "_gaussianFilter.mhd"; 
    strcpy(outputFileName, tempPath.c_str());//string to char*

    emit returnOutputFileName(outputFileName);//发出信号
}

//bilateralFilter暂时仅支持2d image
void itkFilter::bilateralFilter(char* fileName, int dimension, int pixelType) 
{
    HINSTANCE hfilter = LoadLibraryA("filter_dll.dll");
    typedef void(*pFilter)(char* inputFilePath, int dimension, int pixelType); 
    pFilter filter = (pFilter)GetProcAddress(hfilter, "bilateralFilter_dll");
    filter(fileName, dimension, pixelType);
    FreeLibrary(hfilter);

    //加载滤波后结果，输出路径命名规则与DLL中一致
    string tempPath;
    tempPath = fileName;//char* to string
    tempPath = tempPath.substr(0, tempPath.length() - 4);
    if(dimension == 2)
        tempPath = tempPath + "_bilateralFilter.jpg";
    else
    {
        QMessageBox::warning(this, tr("提示信息对话框"),
            tr("双边滤波暂时不支持三维图像"), QMessageBox::Ok);
        return;
    }
    strcpy(outputFileName, tempPath.c_str());//string to char*

    emit returnOutputFileName(outputFileName);//发出信号
}

void itkFilter::laplacianFilter(char* fileName, int dimension, int pixelType)
{
    HINSTANCE hfilter = LoadLibraryA("filter_dll.dll");
    typedef void(*pFilter)(char* inputFilePath, int dimension, int pixelType); 
    pFilter filter = (pFilter)GetProcAddress(hfilter, "laplacianFilter_dll");
    filter(fileName, dimension, pixelType);
    FreeLibrary(hfilter);

    //加载滤波后结果，输出路径命名规则与DLL中一致
    string tempPath;
    tempPath = fileName;//char* to string
    tempPath = tempPath.substr(0, tempPath.length() - 4);
    if(dimension == 2)
        tempPath = tempPath + "_laplacianFilter.jpg";
    else
        tempPath = tempPath + "_laplacianFilter.mhd";
    strcpy(outputFileName, tempPath.c_str());//string to char*

    emit returnOutputFileName(outputFileName);//发出信号
}

void itkFilter::curvatureAnisotropicDiffusionFilter(char* fileName, int dimension, int pixelType)
{
    HINSTANCE hfilter = LoadLibraryA("filter_dll.dll");
    typedef void(*pFilter)(char* inputFilePath, int dimension, int pixelType); 
    pFilter filter = (pFilter)GetProcAddress(hfilter, "curvatureAnisotropicDiffusionFilter_dll");
    filter(fileName, dimension, pixelType);
    FreeLibrary(hfilter);

    //加载滤波后结果，输出路径命名规则与DLL中一致
    string tempPath;
    tempPath = fileName;//char* to string
    tempPath = tempPath.substr(0, tempPath.length() - 4);
    if(dimension == 2)
        tempPath = tempPath + "_curvatureAnisotropicDiffusionFilter.jpg";
    else 
        tempPath = tempPath + "_curvatureAnisotropicDiffusionFilter.mhd";
    strcpy(outputFileName, tempPath.c_str());//string to char*

    emit returnOutputFileName(outputFileName);//发出信号
}

itkFilter::~itkFilter()
{

}

