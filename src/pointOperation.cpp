#include "pointOperation.h"
#include <QMessageBox>

pointOperation::pointOperation(QWidget *parent, Qt::WFlags flags)
    :QWidget(parent, flags)
{
     inputImage = new IplImage();
     outputImage = new IplImage();
}

void pointOperation::color2Gray(char* fileName)
{
    //QPixmap to QImage, 访问像素点  //img = pix->QPixmap::toImage();
    //传递文件路径，直接加载QImage对象
    //QImage to CvMat, 调用OpenCV中函数
    //创建CvMat变量必须指定类型为 CV_32FC1
    //读入jpg, bmp 等一般为3通道图像

    inputImage = cvLoadImage(fileName);
    outputImage = cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_8U, 1);//1通道，1个字节
    //由cvCvtColor获得灰度图，结果图必须为单通道图像
    cvCvtColor(inputImage, outputImage, CV_RGB2GRAY);
    //cvConvertImage(inputImage, outputImage, 0);//第三个参数设置是否得到镜像图
    outputFileName = ("../data/temp/color2gray.jpg");
    cvSaveImage(outputFileName, outputImage);
    //释放资源，腾出内存
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);//发出信号
}

void pointOperation::histgramEqualization(char* fileName)
{
    //QString to char*
    //直方图均衡化要求原始图像，目标图像都为单通道，8位图
    inputImage = cvLoadImage(fileName, CV_LOAD_IMAGE_UNCHANGED);//读入8位灰度图或者彩色图
    outputImage = cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_8U, 1);
    if(inputImage->nChannels == 3)//输入彩色图，则强制转换
    {
        IplImage* temp = cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_8U, 1);
        cvConvertImage(inputImage, temp);
        cvEqualizeHist(temp, outputImage);
    }
    else
    {
         cvEqualizeHist(inputImage, outputImage);
    }
    outputFileName = ("../data/temp/histgramEqualization.jpg");
    cvSaveImage(outputFileName, outputImage);
    //释放资源，腾出内存
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);//发出信号
}

void pointOperation::interpolation(char* fileName)
{
    inputImage = cvLoadImage(fileName, 0);//读入任意类型图
    outputImage = cvCreateImage(cvSize(inputImage->width * 2, inputImage->height * 2), IPL_DEPTH_8U, 1);//8位，单通道
    cvResize(inputImage, outputImage, CV_INTER_CUBIC);//CV_INTER_CUBIC, CV_INTER_AREA, CV_INTER_LANCZOS4
    outputFileName = ("../data/temp/interpolation.jpg");
    cvSaveImage(outputFileName, outputImage);
    //释放资源，腾出内存
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

   emit returnOutputFileName(outputFileName);//发出信号
}

void pointOperation::grayHistgram(char* fileName)
{
    inputImage = cvLoadImage(fileName, CV_LOAD_IMAGE_UNCHANGED);//必须加载灰度图

    //如果加载图像类型错误，则给出提示
    if(inputImage->nChannels == 3)
    {
        QMessageBox::warning(this, tr("提示信息对话框"), tr("输入图片为非灰度图，请重新输入图片"), QMessageBox::Ok);
        return;//返回主窗口
    }

    int histSize = 256;//直方图宽度 
    int histHeight = 256;//直方图高度 
    float range[] = {0,255};  //灰度级的范围  
    float* ranges[] ={range};  

    //创建一维直方图，统计图像在[0 255]像素的均匀分布  
    CvHistogram* grayHist = cvCreateHist(1, &histSize, CV_HIST_ARRAY, ranges, 1);  

    //计算灰度图像的一维直方图  
    cvCalcHist(&inputImage, grayHist, 0, 0);  

    //归一化直方图  
    cvNormalizeHist(grayHist, 1.0);  

    //统计直方图中的最大直方块(做多的点数)  
    float maxValue = 0;  
    cvGetMinMaxHistValue(grayHist, 0, &maxValue, 0, 0);  

    //创建一张一维直方图的“图”，横坐标为灰度级，纵坐标为像素个数  
    IplImage* histImage = cvCreateImage(cvSize(histSize, histHeight), 8, 3);  
    cvZero(histImage); //全部初始化为0

    //分别将每个直方块的值绘制到图中 
    //图像原点为左上角
    // cvRectangle通过对角线上点绘制矩形
    for(int i = 0; i < histSize; i++)  
    {  
        float binNum = cvQueryHistValue_1D(grayHist, i); //像素i对应的点数 
        int binHeight = cvRound(binNum / maxValue * histHeight);  //要绘制的高度  
        cvRectangle(histImage,  
            cvPoint(i, histHeight - 1),  
            cvPoint(i + 1, histHeight - binHeight),  
            CV_RGB(255, 0, 0));    
    }  

    outputFileName = ("../data/temp/grayHistgram.jpg");
    cvSaveImage(outputFileName, histImage);
    //释放资源，腾出内存
    cvReleaseImage(&inputImage);
    cvReleaseImage(&histImage);

    emit returnOutputFileName(outputFileName);//发出信号
}

void pointOperation::colorHistgram(char* fileName)
{
    inputImage = cvLoadImage(fileName, 1);//加载彩色图

    IplImage* r=cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_8U, 1);   
    IplImage* g=cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_8U, 1);   
    IplImage* b=cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_8U, 1);   

    cvSplit(inputImage, b, g, r,NULL); //各个通道分离  

    int histWidth = 256;
    int histHeight = 256;
    float range[]={0,255};  
    float* ranges[]={range};  

    CvHistogram* rHist = cvCreateHist(1, &histWidth, CV_HIST_ARRAY, ranges, 1);  
    CvHistogram* gHist = cvCreateHist(1, &histWidth, CV_HIST_ARRAY, ranges, 1);  
    CvHistogram* bHist = cvCreateHist(1, &histWidth, CV_HIST_ARRAY, ranges, 1);  
    CvHistogram* hist   = cvCreateHist(1, &histWidth, CV_HIST_ARRAY, ranges, 1);  

    //R通道
    cvCalcHist(&r, rHist, 0, NULL);  
    float rMax=0;  
    cvGetMinMaxHistValue(rHist, NULL, &rMax, NULL, NULL);  
    IplImage* rHistImage = cvCreateImage(cvSize(histWidth, histHeight), 8, 3);  
    cvZero(rHistImage); //全部初始化为0
    for(int i = 0; i < histWidth; i++)  
    {  
        float binNum = cvQueryHistValue_1D(rHist, i); //像素i对应的点数 
        int binHeight = cvRound(binNum / rMax * histHeight);  //要绘制的高度  
        cvRectangle(rHistImage,  
            cvPoint(i, histHeight - 1),  
            cvPoint(i + 1, histHeight - binHeight),  
            CV_RGB(255, 0, 0));    
    }  

    //G通道
    cvCalcHist(&g, gHist, 0, NULL);  
    float gMax=0;  
    cvGetMinMaxHistValue(gHist, NULL, &gMax, NULL, NULL);  
    IplImage* gHistImage = cvCreateImage(cvSize(histWidth, histHeight), 8, 3);  
    cvZero(gHistImage); //全部初始化为0
    for(int i = 0; i < histWidth; i++)  
    {  
        float binNum = cvQueryHistValue_1D(gHist, i); //像素i对应的点数 
        int binHeight = cvRound(binNum / gMax * histHeight);  //要绘制的高度  
        cvRectangle(gHistImage,  
            cvPoint(i, histHeight - 1),  
            cvPoint(i + 1, histHeight - binHeight),  
            CV_RGB(0, 255, 0));    
    }  

    //B通道
    cvCalcHist(&b, bHist, 0, NULL);  
    float bMax=0;  
    cvGetMinMaxHistValue(bHist, NULL, &bMax, NULL, NULL);  
    IplImage* bHistImage = cvCreateImage(cvSize(histWidth, histHeight), 8, 3);  
    cvZero(bHistImage); //全部初始化为0
    for(int i = 0; i < histWidth; i++)  
    {  
        float binNum = cvQueryHistValue_1D(bHist, i); //像素i对应的点数 
        int binHeight = cvRound(binNum / bMax * histHeight);  //要绘制的高度  
        cvRectangle(bHistImage,  
            cvPoint(i, histHeight - 1),  
            cvPoint(i + 1, histHeight - binHeight),  
            CV_RGB(0, 0, 255));    
    }  

    //对3通道直方图整合到同一张图
    IplImage* dst=cvCreateImage(cvSize(histWidth * 3, histHeight), 8, 3);  
    cvSetZero(dst);  
    CvRect rect = cvRect(0, 0, histWidth, histWidth);   
    cvSetImageROI(dst, rect);   
    cvCopy(rHistImage, dst);   

    rect = cvRect(histWidth, 0, histWidth, histHeight);  
    cvSetImageROI(dst, rect);   
    cvCopy(gHistImage, dst);  

    rect = cvRect(histWidth * 2, 0, histWidth, histHeight);  
    cvSetImageROI(dst, rect);   
    cvCopy(bHistImage, dst);  
    cvResetImageROI(dst);  

    outputFileName = ("../data/temp/colorHistgram.jpg");
    cvSaveImage(outputFileName, dst);

    //释放资源，腾出内存
    cvReleaseImage(&inputImage);
    cvReleaseImage(&r);
    cvReleaseImage(&g);
    cvReleaseImage(&b);

    emit returnOutputFileName(outputFileName);//发出信号
}

void pointOperation::getSingleChannelImg(char* fileName)
{
    //QPixmap to QImage, 访问像素点  //img = pix->QPixmap::toImage();
    //传递文件路径，直接加载IplImage对象

    inputImage = cvLoadImage(fileName);
    outputImage = cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_8U, 1);//1通道，1个字节
    cvConvertImage(inputImage, outputImage, 0);//第三个参数设置是否得到镜像图
    outputFileName = ("../data/temp/singleChannel.jpg");//通过路径作为CvMat 与 QPixmap 间转化的桥梁
    cvSaveImage(outputFileName, outputImage);
    //释放资源，腾出内存
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);//发出信号
}

pointOperation::~pointOperation()
{

}