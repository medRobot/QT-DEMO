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
    //QPixmap to QImage, �������ص�  //img = pix->QPixmap::toImage();
    //�����ļ�·����ֱ�Ӽ���QImage����
    //QImage to CvMat, ����OpenCV�к���
    //����CvMat��������ָ������Ϊ CV_32FC1
    //����jpg, bmp ��һ��Ϊ3ͨ��ͼ��

    inputImage = cvLoadImage(fileName);
    outputImage = cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_8U, 1);//1ͨ����1���ֽ�
    //��cvCvtColor��ûҶ�ͼ�����ͼ����Ϊ��ͨ��ͼ��
    cvCvtColor(inputImage, outputImage, CV_RGB2GRAY);
    //cvConvertImage(inputImage, outputImage, 0);//���������������Ƿ�õ�����ͼ
    outputFileName = ("../data/temp/color2gray.jpg");
    cvSaveImage(outputFileName, outputImage);
    //�ͷ���Դ���ڳ��ڴ�
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);//�����ź�
}

void pointOperation::histgramEqualization(char* fileName)
{
    //QString to char*
    //ֱ��ͼ���⻯Ҫ��ԭʼͼ��Ŀ��ͼ��Ϊ��ͨ����8λͼ
    inputImage = cvLoadImage(fileName, CV_LOAD_IMAGE_UNCHANGED);//����8λ�Ҷ�ͼ���߲�ɫͼ
    outputImage = cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_8U, 1);
    if(inputImage->nChannels == 3)//�����ɫͼ����ǿ��ת��
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
    //�ͷ���Դ���ڳ��ڴ�
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);//�����ź�
}

void pointOperation::interpolation(char* fileName)
{
    inputImage = cvLoadImage(fileName, 0);//������������ͼ
    outputImage = cvCreateImage(cvSize(inputImage->width * 2, inputImage->height * 2), IPL_DEPTH_8U, 1);//8λ����ͨ��
    cvResize(inputImage, outputImage, CV_INTER_CUBIC);//CV_INTER_CUBIC, CV_INTER_AREA, CV_INTER_LANCZOS4
    outputFileName = ("../data/temp/interpolation.jpg");
    cvSaveImage(outputFileName, outputImage);
    //�ͷ���Դ���ڳ��ڴ�
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

   emit returnOutputFileName(outputFileName);//�����ź�
}

void pointOperation::grayHistgram(char* fileName)
{
    inputImage = cvLoadImage(fileName, CV_LOAD_IMAGE_UNCHANGED);//������ػҶ�ͼ

    //�������ͼ�����ʹ����������ʾ
    if(inputImage->nChannels == 3)
    {
        QMessageBox::warning(this, tr("��ʾ��Ϣ�Ի���"), tr("����ͼƬΪ�ǻҶ�ͼ������������ͼƬ"), QMessageBox::Ok);
        return;//����������
    }

    int histSize = 256;//ֱ��ͼ��� 
    int histHeight = 256;//ֱ��ͼ�߶� 
    float range[] = {0,255};  //�Ҷȼ��ķ�Χ  
    float* ranges[] ={range};  

    //����һάֱ��ͼ��ͳ��ͼ����[0 255]���صľ��ȷֲ�  
    CvHistogram* grayHist = cvCreateHist(1, &histSize, CV_HIST_ARRAY, ranges, 1);  

    //����Ҷ�ͼ���һάֱ��ͼ  
    cvCalcHist(&inputImage, grayHist, 0, 0);  

    //��һ��ֱ��ͼ  
    cvNormalizeHist(grayHist, 1.0);  

    //ͳ��ֱ��ͼ�е����ֱ����(����ĵ���)  
    float maxValue = 0;  
    cvGetMinMaxHistValue(grayHist, 0, &maxValue, 0, 0);  

    //����һ��һάֱ��ͼ�ġ�ͼ����������Ϊ�Ҷȼ���������Ϊ���ظ���  
    IplImage* histImage = cvCreateImage(cvSize(histSize, histHeight), 8, 3);  
    cvZero(histImage); //ȫ����ʼ��Ϊ0

    //�ֱ�ÿ��ֱ�����ֵ���Ƶ�ͼ�� 
    //ͼ��ԭ��Ϊ���Ͻ�
    // cvRectangleͨ���Խ����ϵ���ƾ���
    for(int i = 0; i < histSize; i++)  
    {  
        float binNum = cvQueryHistValue_1D(grayHist, i); //����i��Ӧ�ĵ��� 
        int binHeight = cvRound(binNum / maxValue * histHeight);  //Ҫ���Ƶĸ߶�  
        cvRectangle(histImage,  
            cvPoint(i, histHeight - 1),  
            cvPoint(i + 1, histHeight - binHeight),  
            CV_RGB(255, 0, 0));    
    }  

    outputFileName = ("../data/temp/grayHistgram.jpg");
    cvSaveImage(outputFileName, histImage);
    //�ͷ���Դ���ڳ��ڴ�
    cvReleaseImage(&inputImage);
    cvReleaseImage(&histImage);

    emit returnOutputFileName(outputFileName);//�����ź�
}

void pointOperation::colorHistgram(char* fileName)
{
    inputImage = cvLoadImage(fileName, 1);//���ز�ɫͼ

    IplImage* r=cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_8U, 1);   
    IplImage* g=cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_8U, 1);   
    IplImage* b=cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_8U, 1);   

    cvSplit(inputImage, b, g, r,NULL); //����ͨ������  

    int histWidth = 256;
    int histHeight = 256;
    float range[]={0,255};  
    float* ranges[]={range};  

    CvHistogram* rHist = cvCreateHist(1, &histWidth, CV_HIST_ARRAY, ranges, 1);  
    CvHistogram* gHist = cvCreateHist(1, &histWidth, CV_HIST_ARRAY, ranges, 1);  
    CvHistogram* bHist = cvCreateHist(1, &histWidth, CV_HIST_ARRAY, ranges, 1);  
    CvHistogram* hist   = cvCreateHist(1, &histWidth, CV_HIST_ARRAY, ranges, 1);  

    //Rͨ��
    cvCalcHist(&r, rHist, 0, NULL);  
    float rMax=0;  
    cvGetMinMaxHistValue(rHist, NULL, &rMax, NULL, NULL);  
    IplImage* rHistImage = cvCreateImage(cvSize(histWidth, histHeight), 8, 3);  
    cvZero(rHistImage); //ȫ����ʼ��Ϊ0
    for(int i = 0; i < histWidth; i++)  
    {  
        float binNum = cvQueryHistValue_1D(rHist, i); //����i��Ӧ�ĵ��� 
        int binHeight = cvRound(binNum / rMax * histHeight);  //Ҫ���Ƶĸ߶�  
        cvRectangle(rHistImage,  
            cvPoint(i, histHeight - 1),  
            cvPoint(i + 1, histHeight - binHeight),  
            CV_RGB(255, 0, 0));    
    }  

    //Gͨ��
    cvCalcHist(&g, gHist, 0, NULL);  
    float gMax=0;  
    cvGetMinMaxHistValue(gHist, NULL, &gMax, NULL, NULL);  
    IplImage* gHistImage = cvCreateImage(cvSize(histWidth, histHeight), 8, 3);  
    cvZero(gHistImage); //ȫ����ʼ��Ϊ0
    for(int i = 0; i < histWidth; i++)  
    {  
        float binNum = cvQueryHistValue_1D(gHist, i); //����i��Ӧ�ĵ��� 
        int binHeight = cvRound(binNum / gMax * histHeight);  //Ҫ���Ƶĸ߶�  
        cvRectangle(gHistImage,  
            cvPoint(i, histHeight - 1),  
            cvPoint(i + 1, histHeight - binHeight),  
            CV_RGB(0, 255, 0));    
    }  

    //Bͨ��
    cvCalcHist(&b, bHist, 0, NULL);  
    float bMax=0;  
    cvGetMinMaxHistValue(bHist, NULL, &bMax, NULL, NULL);  
    IplImage* bHistImage = cvCreateImage(cvSize(histWidth, histHeight), 8, 3);  
    cvZero(bHistImage); //ȫ����ʼ��Ϊ0
    for(int i = 0; i < histWidth; i++)  
    {  
        float binNum = cvQueryHistValue_1D(bHist, i); //����i��Ӧ�ĵ��� 
        int binHeight = cvRound(binNum / bMax * histHeight);  //Ҫ���Ƶĸ߶�  
        cvRectangle(bHistImage,  
            cvPoint(i, histHeight - 1),  
            cvPoint(i + 1, histHeight - binHeight),  
            CV_RGB(0, 0, 255));    
    }  

    //��3ͨ��ֱ��ͼ���ϵ�ͬһ��ͼ
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

    //�ͷ���Դ���ڳ��ڴ�
    cvReleaseImage(&inputImage);
    cvReleaseImage(&r);
    cvReleaseImage(&g);
    cvReleaseImage(&b);

    emit returnOutputFileName(outputFileName);//�����ź�
}

void pointOperation::getSingleChannelImg(char* fileName)
{
    //QPixmap to QImage, �������ص�  //img = pix->QPixmap::toImage();
    //�����ļ�·����ֱ�Ӽ���IplImage����

    inputImage = cvLoadImage(fileName);
    outputImage = cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_8U, 1);//1ͨ����1���ֽ�
    cvConvertImage(inputImage, outputImage, 0);//���������������Ƿ�õ�����ͼ
    outputFileName = ("../data/temp/singleChannel.jpg");//ͨ��·����ΪCvMat �� QPixmap ��ת��������
    cvSaveImage(outputFileName, outputImage);
    //�ͷ���Դ���ڳ��ڴ�
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);//�����ź�
}

pointOperation::~pointOperation()
{

}