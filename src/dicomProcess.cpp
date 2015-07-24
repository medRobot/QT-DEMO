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
    //�������ݿ�

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
        QMessageBox::warning(this, tr("��ʾ��Ϣ�Ի���"), tr("�������ݿ�ʧ�ܣ�����������!"), QMessageBox::Ok);
        return;
    }
}

void dicomProcess::showImgProperty(vtkDICOMImageReader* reader)
{
    int* imgSize;
    double* origin;
    double* spacing;
    //��ȡͼ��ߴ�
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
    //��ȡͼ�����
    if(reader->GetDataScalarType() == VTK_UNSIGNED_CHAR)
    {
        ui->imgDepthLabel->setText(QString::number(8, 10));
    }
    else if(reader->GetDataScalarType() == VTK_UNSIGNED_SHORT || reader->GetDataScalarType() == VTK_SHORT)
    {
        ui->imgDepthLabel->setText(QString::number(16, 10));
    }

    //��ITK��signed short����DIOCM, ����������Ӧѡ���������� 2015.5.17
    typedef unsigned short    PixelType;//signed
    const unsigned int      Dimension = 3;
    typedef itk::Image< PixelType, Dimension >         ImageType;
    typedef itk::ImageSeriesReader< ImageType >        ReaderType;//������ͼƬ
    ReaderType::Pointer itkReader = ReaderType::New();
    typedef itk::GDCMImageIO       ImageIOType;//��DICOMͼƬ
    ImageIOType::Pointer dicomIO = ImageIOType::New();
    itkReader->SetImageIO( dicomIO );//���ݶ����ڴ�
    typedef itk::GDCMSeriesFileNames NamesGeneratorType;
    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
    nameGenerator->SetUseSeriesDetails( true );
    nameGenerator->AddSeriesRestriction("0008|0021" );
    nameGenerator->SetDirectory(inputFileName);//�����ļ�Ŀ¼

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
    //���ٿռ��ȡDICOMͷ�ļ�����Ϣ
    nameInfo =   new char[50];//new char(15);//Բ���ű�ʾָ����ֵ
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
    ImageIOType::ByteOrder byteOrder;//����ת��Ϊ�ַ������
    byteOrder = dicomIO->GetByteOrder();
    ImageIOType::TCompressionType compressType;
    compressType = dicomIO->GetCompressionType();
    //���ϴ����޴�
    //��char* name����ȡ�������Ա�����
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
    //DICOM����Ϣ��ʾ���������
    ui->manufactureLabel->setText(QString(QLatin1String(manufacture)));
    ui->hospitalLabel->setText(QString(QLatin1String(hospital)));
    //��������
    if(pixelType == 1)
        ui->pixelTypeLabel->setText("SCALAR");//IOPixelType
    else if(pixelType == 2)
        ui->pixelTypeLabel->setText("RGB");
    else if(pixelType == 3)
        ui->pixelTypeLabel->setText("RGBA");
    //�ļ�����
    if(fileType == 0)
        ui->fileTypeLabel->setText("ASCII");//��itkimageiobase.h��ȡ
    else if(fileType == 1) 
        ui->fileTypeLabel->setText("Binary");
    //�ֽ����˳��
    if(byteOrder == ImageIOType::ByteOrder::LittleEndian)
        ui->byteOrderLabel->setText("LittleEndian");
    else  if(byteOrder == ImageIOType::ByteOrder::BigEndian)
        ui->byteOrderLabel->setText("BigEndian");
    //�ļ�ѹ������
    if(compressType == ImageIOType::TCompressionType::JPEG2000)
        ui->compressTypeLabel->setText("JPEG2000");
    else if(compressType == ImageIOType::TCompressionType::JPEG)
        ui->compressTypeLabel->setText("JPEG");
    else if(compressType == ImageIOType::TCompressionType::JPEG) 
        ui->compressTypeLabel->setText("JPEGLS");
    //���������ݿ�����Ϣ
    ui->nameLineEdit->setText(patientName); 
    ui->sexLineEdit->setText(patientSex);
    ui->ageLineEdit->setText(patientAge);
    ui->IDLineEdit->setText(QString(QLatin1String(ID)));//char* to QString
    ui->modalityLineEdit->setText(QString(QLatin1String(modility)));
    ui->timeLineEdit->setText(QString(QLatin1String(time)));
    //new & delete �ɶԳ���
    //�ͷ��ѿ��ٿռ䣬new�洢�ռ䲻�ܱ�ʵ����Ҫ�ռ�С������delete���������
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
        QMessageBox::warning(this, tr("��ʾ��Ϣ�Ի���"), tr("�������ݿ�ʧ�ܣ�����������!"), QMessageBox::Ok);
        return;
    }
    QSqlQuery query;
    query.exec("select patientID from patient");
    int numRows = query.size();//���ص�ǰ���м�¼����
    //���ݲ����ظ�����
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
        QMessageBox::warning(this, tr("��ʾ��Ϣ�Ի���"), 
            tr("���ݿ������иü�¼�������ظ�����!"), QMessageBox::Ok);
        return;
    }
    //idΪint���ͣ�����Ϊvarchar����
    query.prepare("insert into patient (id, patientID, name, sex, age, modality, time)"
        "values (:id, :patientID, :name, :sex, :age, :modality, :time)");
    //ʹ�����ư󶨣��������SQL�����ʹ�ñ���
    query.bindValue(":id", numRows + 1);//1, int
    query.bindValue(":patientID", ui->IDLineEdit->text());//2, QString
    query.bindValue(":name", ui->nameLineEdit->text());//3, QString
    query.bindValue(":sex", ui->sexLineEdit->text());//4, QString
    query.bindValue(":age", ui->ageLineEdit->text());//5, QString
    query.bindValue(":modality", ui->modalityLineEdit->text());//6, QString
    query.bindValue(":time", ui->timeLineEdit->text());//7, QString
    query.exec();//ִ��
    QMessageBox::information(this, tr("��ʾ��Ϣ�Ի���"), 
        tr("�����ݿ��гɹ����Ӹü�¼!"), QMessageBox::Ok);
}

void dicomProcess::on_deleteButtonSQL_clicked()
{
    if(!isConnected)
    {
        QMessageBox::warning(this, tr("��ʾ��Ϣ�Ի���"), tr("�������ݿ�ʧ�ܣ�����������!"), QMessageBox::Ok);
        return;
    }
    QSqlQuery query;
    query.exec("select patientID from patient");
    int numRows = query.size();
    //��Ϊ�ղ���ɾ��
    if(numRows == 0)
    {
        QMessageBox::warning(this, tr("��ʾ��Ϣ�Ի���"), tr("���ݿ��м�¼Ϊ�գ�����ִ��ɾ��!"), QMessageBox::Ok);
        return;
    }
    //��¼�����ڣ�����ɾ��
    bool isExist = false;
    while(query.next())
    {
        if(query.value(0).toString() == ui->IDLineEdit->text())
        {
            isExist = true;//��¼�������˳�
            break;
        }
    }
    if(!isExist)
    {
        QMessageBox::warning(this, tr("��ʾ��Ϣ�Ի���"), tr("���ݿ���û�иü�¼������ִ��ɾ��!"), QMessageBox::Ok);
        return;
    }
    query.prepare("delete from patient where patientID = :patientID");//patientIDΨһ
    //ʹ�����ư󶨣��������SQL�����ʹ�ñ���
    query.bindValue(":patientID", ui->IDLineEdit->text());//QString
    query.exec();//ִ��
    QMessageBox::information(this, tr("��ʾ��Ϣ�Ի���"), 
        tr("�����ݿ��гɹ�ɾ���ü�¼!"), QMessageBox::Ok);
}

void dicomProcess::on_modifyButtonSQL_clicked()
{
    if(!isConnected)
    {
        QMessageBox::warning(this, tr("��ʾ��Ϣ�Ի���"), tr("�������ݿ�ʧ�ܣ�����������!"), QMessageBox::Ok);
        return;
    }
    QSqlQuery query;
    query.exec("select patientID from patient");
    int numRows = query.size();
    //��Ϊ�ղ����޸�
    if(numRows == 0)
    {
        QMessageBox::warning(this, tr("��ʾ��Ϣ�Ի���"),
            tr("���ݿ��м�¼Ϊ�գ�����ִ���޸�!"), QMessageBox::Ok);
        return;
    }
    //��¼�����ڣ������޸�
    bool isExist = false;
    while(query.next())
    {
        if(query.value(0).toString() == ui->IDLineEdit->text())
        {
            isExist = true;//��¼�������˳�
            break;
        }
    }
    if(!isExist)
    {
        QMessageBox::warning(this, tr("��ʾ��Ϣ�Ի���"), tr("���ݿ���û�иü�¼������ִ���޸�!"), QMessageBox::Ok);
        return;
    }
    //������¼ͬʱ����
    query.prepare("update patient set name = :name, sex = :sex, "
        "age = :age, modality = :modality, time = :time where patientID = :patientID");
    //ʹ�����ư󶨣��������SQL�����ʹ�ñ���
    query.bindValue(":patientID", ui->IDLineEdit->text());//1, QString
    query.bindValue(":name", ui->nameLineEdit->text());//2, QString
    query.bindValue(":sex", ui->sexLineEdit->text());//3, QString
    query.bindValue(":age", ui->ageLineEdit->text());//4, QString
    query.bindValue(":modality", ui->modalityLineEdit->text());//5, QString
    query.bindValue(":time", ui->timeLineEdit->text());//6, QString
    query.exec();//ִ��
    QMessageBox::information(this, tr("��ʾ��Ϣ�Ի���"), 
        tr("�����ݿ��гɹ��޸ĸü�¼!"), QMessageBox::Ok);
}

void dicomProcess::on_queryButtonSQL_clicked()//��ѯ��ǰ��¼
{
    if(!isConnected)
    {
        QMessageBox::warning(this, tr("��ʾ��Ϣ�Ի���"), tr("�������ݿ�ʧ�ܣ�����������!"), QMessageBox::Ok);
        return;
    }
    QSqlQuery query;
    query.exec("select patientID from patient");
    bool isExist = false;
    while(query.next())
    {
        if(query.value(0).toString() == ui->IDLineEdit->text())
        {
            QMessageBox::information(this, tr("��ѯ����Ի���"), tr("���ݿ��д��ڸü�¼!"), QMessageBox::Ok);
            return;
        }
    }
    if(!isExist)
    {
        QMessageBox::information(this, tr("��ѯ����Ի���"), tr("���ݿ��в����ڸü�¼!"), QMessageBox::Ok);
        return;
    }
}

void dicomProcess::on_clearButtonSQL_clicked()//���6��lineEdits
{
    ui->IDLineEdit->setText("");
    ui->nameLineEdit->setText("");
    ui->sexLineEdit->setText("");
    ui->ageLineEdit->setText("");
    ui->modalityLineEdit->setText("");
    ui->timeLineEdit->setText("");
}

void dicomProcess::on_showAllTableButtonSQL_clicked()//�ԶԻ�����ʾ���ű�
{
    if(!isConnected)
    {
        QMessageBox::warning(this, tr("��ʾ��Ϣ�Ի���"), tr("�������ݿ�ʧ�ܣ�����������!"), QMessageBox::Ok);
        return;
    }
    //�����Ի���
    QDialog* dialog = new QDialog(this);
    dialog->setFixedSize(750, 300);
    QSqlTableModel* model = new QSqlTableModel(this);
    model->setTable("patient");
    model->select();
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //ָ��������Ϊdialog, ��tableView�����ڶԻ�������ʾ
    QTableView* tableView = new QTableView(dialog);//����
    tableView->setFixedSize(735, 300);
    tableView->setModel(model);
    dialog->show();
}

dicomProcess::~dicomProcess()
{
    //�ر����ݿ⣬�����ظ�����
    //dbSQL.close();
}