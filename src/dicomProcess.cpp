#include "dicomProcess.h"
#include "itkImageFileReader.h"//ITK
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include <QSqlQuery>//QT
#include <QString>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QTableView>
#include <QDialog>
#include <QTextCodec>
#include <string>
#include <string.h>//strlen
#include <iostream>
using namespace std;

dicomProcess::dicomProcess(QWidget *parent , Qt::WFlags flags)
    :QWidget(parent, flags),
    ui(new Ui::dicomProcess)
{
    ui->setupUi(this);
    //连接数据库

    dbSQL= QSqlDatabase::addDatabase("QMYSQL");
    //dbSQL.setHostName("localhost");
    dbSQL.setHostName("192.168.1.112");
    dbSQL.setPort(3306);
    dbSQL.setDatabaseName("imageprocess");
    dbSQL.setUserName("root");
    dbSQL.setPassword("mipav");
    isConnected = dbSQL.open();
    if(!isConnected)
    {
        QMessageBox::warning(this, tr("提示信息对话框"), tr("连接数据库失败，请重新连接!"), QMessageBox::Ok);
        return;
    }
}

void dicomProcess::showImgProperty(vtkDICOMImageReader* reader)
{
    int* imgSize;
    double* origin;
    double* spacing;
    //获取图像尺寸
    imgSize = reader->GetDataExtent();
    origin = reader->GetDataOrigin();
    spacing = reader->GetDataSpacing();
    ui->imgLengthLabel->setText(QString::number(imgSize[1] + 1, 10));//int ->QString 
    ui->imgWidthLabel->setText(QString::number(imgSize[3] + 1, 10)); 
    ui->imgSliceLabel->setText(QString::number(imgSize[5] + 1, 10)); 
    ui->pixelSpacingLabel->setText(QString::number(spacing[0]) + (" ") 
        + QString::number(spacing[1]) + (" ") + QString::number(spacing[2]));
    ui->imgOriginLabel->setText(QString::number(origin[0]) + (" ") 
        + QString::number(origin[1]) + (" ") + QString::number(origin[2]));
    //获取图像深度
    if(reader->GetDataScalarType() == VTK_UNSIGNED_CHAR)
    {
        ui->imgDepthLabel->setText(QString::number(8, 10));
    }
    else if(reader->GetDataScalarType() == VTK_UNSIGNED_SHORT || reader->GetDataScalarType() == VTK_SHORT)
    {
        ui->imgDepthLabel->setText(QString::number(16, 10));
    }

    //由ITK读signed short类型DIOCM, 还不能自适应选择数据类型 2015.5.17
    typedef unsigned short    PixelType;//signed
    const unsigned int      Dimension = 3;
    typedef itk::Image< PixelType, Dimension >         ImageType;
    typedef itk::ImageSeriesReader< ImageType >        ReaderType;//读序列图片
    ReaderType::Pointer itkReader = ReaderType::New();
    typedef itk::GDCMImageIO       ImageIOType;//读DICOM图片
    ImageIOType::Pointer dicomIO = ImageIOType::New();
    itkReader->SetImageIO( dicomIO );//数据读入内存
    typedef itk::GDCMSeriesFileNames NamesGeneratorType;
    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
    nameGenerator->SetUseSeriesDetails( true );
    nameGenerator->AddSeriesRestriction("0008|0021" );
    nameGenerator->SetDirectory(inputFileName);//设置文件目录

    typedef std::vector< std::string >    SeriesIdContainer;
    const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();
    SeriesIdContainer::const_iterator seriesItr = seriesUID.begin();
    SeriesIdContainer::const_iterator seriesEnd = seriesUID.end();
    std::string seriesIdentifier;
    seriesIdentifier = seriesUID.begin()->c_str();
    typedef std::vector< std::string >   FileNamesContainer;
    FileNamesContainer fileNames;
    fileNames = nameGenerator->GetFileNames( seriesIdentifier );
    itkReader->SetFileNames( fileNames );
    itkReader->Update();
    dicomIO->GetMetaDataDictionary();
    //开辟空间获取DICOM头文件中信息
    nameInfo =   new char[50];//new char(15);//圆括号表示指定初值
    ID = new char[50];
    time = new char[50];
    manufacture = new char[50];
    //char* windowCenter = new char(15);//No API
    //char* windowWidth = new char(15);
    modility = new char[15];
    hospital = new char[50];
    int pixelType =  dicomIO->GetPixelType();
    int componetType = dicomIO->GetComponentType();//SHORT, UNSIGNED SHORT
    int fileType = dicomIO->GetFileType();
    dicomIO->GetPatientName(nameInfo);
    dicomIO->GetModality(modility);
    dicomIO->GetPatientID(ID);
    dicomIO->GetManufacturer(manufacture);
    dicomIO->GetStudyDate(time);
    dicomIO->GetInstitution(hospital);
    ImageIOType::ByteOrder byteOrder;//不能转换为字符串输出
    byteOrder = dicomIO->GetByteOrder();
    ImageIOType::TCompressionType compressType;
    compressType = dicomIO->GetCompressionType();
    //以上代码无错
    //从char* name中提取姓名，性别，年龄
    QString patientName, patientSex, patientAge;
    for(size_t i = 0; i < strlen(nameInfo); i++)
    {
        if(nameInfo[i] == 'M') 
        {
            patientSex = ("M");
            std::string patientInfo = nameInfo;
            patientInfo = patientInfo.substr(0, i - 1);
            char temp[15];
            strcpy(temp, patientInfo.c_str());
            patientName = QString(QLatin1String(temp));
            //patientName = QString::fromStdString(patientInfo);//
        } 
        if(nameInfo[i] == 'F') 
        {
            patientSex = ("F");
            string patientInfo = nameInfo;
            patientInfo = patientInfo.substr(0, i - 1);
            char temp[15];
            strcpy(temp, patientInfo.c_str());//string to char*
            patientName = QString(QLatin1String(temp));//char* to QString
        } 
        if(nameInfo[i] == 'Y') patientAge = QString(nameInfo[i - 2]) + QString(nameInfo[i - 1]);//char to QString
    }
    //DICOM中信息显示到控制面板
    ui->manufactureLabel->setText(QString(QLatin1String(manufacture)));
    ui->hospitalLabel->setText(QString(QLatin1String(hospital)));
    //像素类型
    if(pixelType == 1)
        ui->pixelTypeLabel->setText("SCALAR");//IOPixelType
    else if(pixelType == 2)
        ui->pixelTypeLabel->setText("RGB");
    else if(pixelType == 3)
        ui->pixelTypeLabel->setText("RGBA");
    //文件类型
    if(fileType == 0)
        ui->fileTypeLabel->setText("ASCII");//从itkimageiobase.h获取
    else if(fileType == 1) 
        ui->fileTypeLabel->setText("Binary");
    //字节输出顺序
    if(byteOrder == ImageIOType::ByteOrder::LittleEndian)
        ui->byteOrderLabel->setText("LittleEndian");
    else  if(byteOrder == ImageIOType::ByteOrder::BigEndian)
        ui->byteOrderLabel->setText("BigEndian");
    //文件压缩类型
    if(compressType == ImageIOType::TCompressionType::JPEG2000)
        ui->compressTypeLabel->setText("JPEG2000");
    else if(compressType == ImageIOType::TCompressionType::JPEG)
        ui->compressTypeLabel->setText("JPEG");
    else if(compressType == ImageIOType::TCompressionType::JPEG) 
        ui->compressTypeLabel->setText("JPEGLS");
    //保存在数据库中信息
    ui->nameLineEdit->setText(patientName); 
    ui->sexLineEdit->setText(patientSex);
    ui->ageLineEdit->setText(patientAge);
    ui->IDLineEdit->setText(QString(QLatin1String(ID)));//char* to QString
    ui->modalityLineEdit->setText(QString(QLatin1String(modility)));
    ui->timeLineEdit->setText(QString(QLatin1String(time)));
    //new & delete 成对出现
    //释放已开辟空间，new存储空间不能比实际需要空间小，否则delete操作会出错
    delete[] nameInfo;
    delete[] ID;
    delete[] time;
    delete[] manufacture;
    delete[] modility;
    delete[] hospital;
}

void dicomProcess::on_addButtonSQL_clicked()
{
    if(!isConnected)
    {
        QMessageBox::warning(this, tr("提示信息对话框"), tr("连接数据库失败，请重新连接!"), QMessageBox::Ok);
        return;
    }
    QSqlQuery query;
    query.exec("select patientID from patient");
    int numRows = query.size();//返回当前表中记录个数
    //数据不能重复加载
    bool isloaded = false;
    while(query.next())
    {
        if(query.value(0).toString() == ui->IDLineEdit->text())
        {
            isloaded = true;
            break;
        }
    }
    if(isloaded)
    {
        QMessageBox::warning(this, tr("提示信息对话框"), 
            tr("数据库中已有该记录，不能重复加载!"), QMessageBox::Ok);
        return;
    }
    //id为int类型，其他为varchar类型
    query.prepare("insert into patient (id, patientID, name, sex, age, modality, time)"
        "values (:id, :patientID, :name, :sex, :age, :modality, :time)");
    //使用名称绑定，则可以在SQL语句中使用变量
    query.bindValue(":id", numRows + 1);//1, int
    query.bindValue(":patientID", ui->IDLineEdit->text());//2, QString
    query.bindValue(":name", ui->nameLineEdit->text());//3, QString
    query.bindValue(":sex", ui->sexLineEdit->text());//4, QString
    query.bindValue(":age", ui->ageLineEdit->text());//5, QString
    query.bindValue(":modality", ui->modalityLineEdit->text());//6, QString
    query.bindValue(":time", ui->timeLineEdit->text());//7, QString
    query.exec();//执行
    QMessageBox::information(this, tr("提示信息对话框"), 
        tr("向数据库中成功增加该记录!"), QMessageBox::Ok);
}

void dicomProcess::on_deleteButtonSQL_clicked()
{
    if(!isConnected)
    {
        QMessageBox::warning(this, tr("提示信息对话框"), tr("连接数据库失败，请重新连接!"), QMessageBox::Ok);
        return;
    }
    QSqlQuery query;
    query.exec("select patientID from patient");
    int numRows = query.size();
    //表为空不能删除
    if(numRows == 0)
    {
        QMessageBox::warning(this, tr("提示信息对话框"), tr("数据库中记录为空，不能执行删除!"), QMessageBox::Ok);
        return;
    }
    //记录不存在，不能删除
    bool isExist = false;
    while(query.next())
    {
        if(query.value(0).toString() == ui->IDLineEdit->text())
        {
            isExist = true;//记录存在则退出
            break;
        }
    }
    if(!isExist)
    {
        QMessageBox::warning(this, tr("提示信息对话框"), tr("数据库中没有该记录，不能执行删除!"), QMessageBox::Ok);
        return;
    }
    query.prepare("delete from patient where patientID = :patientID");//patientID唯一
    //使用名称绑定，则可以在SQL语句中使用变量
    query.bindValue(":patientID", ui->IDLineEdit->text());//QString
    query.exec();//执行
    QMessageBox::information(this, tr("提示信息对话框"), 
        tr("在数据库中成功删除该记录!"), QMessageBox::Ok);
}

void dicomProcess::on_modifyButtonSQL_clicked()
{
    if(!isConnected)
    {
        QMessageBox::warning(this, tr("提示信息对话框"), tr("连接数据库失败，请重新连接!"), QMessageBox::Ok);
        return;
    }
    QSqlQuery query;
    query.exec("select patientID from patient");
    int numRows = query.size();
    //表为空不能修改
    if(numRows == 0)
    {
        QMessageBox::warning(this, tr("提示信息对话框"),
            tr("数据库中记录为空，不能执行修改!"), QMessageBox::Ok);
        return;
    }
    //记录不存在，不能修改
    bool isExist = false;
    while(query.next())
    {
        if(query.value(0).toString() == ui->IDLineEdit->text())
        {
            isExist = true;//记录存在则退出
            break;
        }
    }
    if(!isExist)
    {
        QMessageBox::warning(this, tr("提示信息对话框"), tr("数据库中没有该记录，不能执行修改!"), QMessageBox::Ok);
        return;
    }
    //整条记录同时更新
    query.prepare("update patient set name = :name, sex = :sex, "
        "age = :age, modality = :modality, time = :time where patientID = :patientID");
    //使用名称绑定，则可以在SQL语句中使用变量
    query.bindValue(":patientID", ui->IDLineEdit->text());//1, QString
    query.bindValue(":name", ui->nameLineEdit->text());//2, QString
    query.bindValue(":sex", ui->sexLineEdit->text());//3, QString
    query.bindValue(":age", ui->ageLineEdit->text());//4, QString
    query.bindValue(":modality", ui->modalityLineEdit->text());//5, QString
    query.bindValue(":time", ui->timeLineEdit->text());//6, QString
    query.exec();//执行
    QMessageBox::information(this, tr("提示信息对话框"), 
        tr("在数据库中成功修改该记录!"), QMessageBox::Ok);
}

void dicomProcess::on_queryButtonSQL_clicked()//查询当前记录
{
    if(!isConnected)
    {
        QMessageBox::warning(this, tr("提示信息对话框"), tr("连接数据库失败，请重新连接!"), QMessageBox::Ok);
        return;
    }
    QSqlQuery query;
    query.exec("select patientID from patient");
    bool isExist = false;
    while(query.next())
    {
        if(query.value(0).toString() == ui->IDLineEdit->text())
        {
            QMessageBox::information(this, tr("查询结果对话框"), tr("数据库中存在该记录!"), QMessageBox::Ok);
            return;
        }
    }
    if(!isExist)
    {
        QMessageBox::information(this, tr("查询结果对话框"), tr("数据库中不存在该记录!"), QMessageBox::Ok);
        return;
    }
}

void dicomProcess::on_clearButtonSQL_clicked()//清空6个lineEdits
{
    ui->IDLineEdit->setText("");
    ui->nameLineEdit->setText("");
    ui->sexLineEdit->setText("");
    ui->ageLineEdit->setText("");
    ui->modalityLineEdit->setText("");
    ui->timeLineEdit->setText("");
}

void dicomProcess::on_showAllTableButtonSQL_clicked()//以对话框显示整张表
{
    if(!isConnected)
    {
        QMessageBox::warning(this, tr("提示信息对话框"), tr("连接数据库失败，请重新连接!"), QMessageBox::Ok);
        return;
    }
    //弹出对话框
    QDialog* dialog = new QDialog(this);
    dialog->setFixedSize(750, 300);
    QSqlTableModel* model = new QSqlTableModel(this);
    model->setTable("patient");
    model->select();
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //指定父窗口为dialog, 则tableView可以在对话框中显示
    QTableView* tableView = new QTableView(dialog);//居中
    tableView->setFixedSize(735, 300);
    tableView->setModel(model);
    dialog->show();
}

dicomProcess::~dicomProcess()
{
    //关闭数据库，避免重复连接
    //dbSQL.close();
}