#include "geometryTransform.h"
#include <QTransform>

geometryTransform::geometryTransform(QWidget *parent , Qt::WFlags flags)
    :QWidget(parent, flags),
    ui(new Ui::geometryTransform)
{
    ui->setupUi(this);

    //��ʼ��
    pix = new QPixmap();
    xFactor = 1;
    yFactor = 1;
    rotateFactor = 0;

    initUI();//��ʼ������

    createConnect();//Ԥ��������ӹ�ϵ
}

void geometryTransform::initUI()
{
    //�Կؼ����ã�Ҳ������QT���ʦ������
    ui->x_translate_min->setText("0");
    ui->x_translate_max->setText("1000");
    ui->y_translate_min->setText("0");
    ui->y_translate_max->setText("1000");

    ui->x_scale_min->setText("1");
    ui->x_scale_max->setText("10");
    ui->y_scale_min->setText("1");
    ui->y_scale_max->setText("10");
}

void geometryTransform::createConnect()
{
    //�ؼ����໥������ ����һ�Կؼ��������ȴ���˭��������
    connect(ui->x_scale_current, SIGNAL(valueChanged(int)),  ui->Xslider_scale, SLOT(setValue(int)));//����
    connect(ui->Xslider_scale,SIGNAL(valueChanged(int)), ui->x_scale_current,   SLOT(setValue(int)));
    connect(ui->y_scale_current, SIGNAL(valueChanged(int)), ui->Yslider_scale, SLOT(setValue(int)));
    connect(ui->Yslider_scale, SIGNAL(valueChanged(int)), ui->y_scale_current, SLOT(setValue(int)));

    connect(ui->x_translate_current,  SIGNAL(valueChanged(int)), ui->Xslider_translate, SLOT(setValue(int)));//ƽ��
    connect(ui->Xslider_translate, SIGNAL(valueChanged(int)), ui->x_translate_current,  SLOT(setValue(int)));
    connect(ui->y_translate_current, SIGNAL(valueChanged(int)),  ui->Yslider_translate, SLOT(setValue(int)));
    connect(ui->Yslider_translate, SIGNAL(valueChanged(int)), ui->y_translate_current,  SLOT(setValue(int)));

    connect(ui->dialNum, SIGNAL(valueChanged(int)), ui->dial_rotate, SLOT(setValue(int)));//��ת
    connect(ui->dial_rotate, SIGNAL(valueChanged(int)), ui->dialNum, SLOT(setValue(int)));
}

//�Կؼ��������Զ����Ӵ��ݲ���
void geometryTransform::on_Xslider_scale_valueChanged(int value)
{
    //��QT���ʦ���༭�ź�/�ۡ��н����ź�����Զ����ӣ�������Ҫ�ô�����ʾ��ʾ
    //һ���źſ������Ӷ����
    ui->x_scale_current->setValue(value);
    xFactor = value;
}

void geometryTransform::on_Yslider_scale_valueChanged(int value)
{
    ui->y_scale_current->setValue(value);
    yFactor = value;
}

void geometryTransform::on_Xslider_translate_valueChanged(int value)
{
    ui->x_translate_current->setValue(value);
    xFactor = value;
}

void geometryTransform::on_Yslider_translate_valueChanged(int value)
{
    ui->y_translate_current->setValue(value);
    yFactor = value;
}

void geometryTransform::on_dial_rotate_valueChanged(int value)
{
    ui->dialNum->setValue(value);
    rotateFactor = value;
}

//ÿ�β�����Ӧ���½�һ��QTransform����������Transform����Ѿ�ִ�й��Ĳ����ٴ��ظ�
//���β����������Զ�ͼ���������Ҳ���Զ�QGraphicsView��Ķ������
void geometryTransform::on_translateButton_clicked()//ƽ��
{
    pix->load(*inputFileName);
    QTransform transform;
    transform.translate(xFactor, yFactor);

    *pix = pix->transformed(transform);
    pix->save("../data/temp/imgTranslate.jpg");
    //delete pix;//�ͷ���Դ�����ɾ��pix�� ������������
     outputFileName = ("../data/temp/imgTranslate.jpg");
    emit returnOutputFileName(outputFileName);//�����ź�ʱ����Ҫ֪����˭�������ź�
}

void geometryTransform::on_scaleButton_clicked()//����
{
     pix->load(*inputFileName);
    //��ȡ�ӻ������ݹ�������ֵ
    QTransform transform;
    transform.scale(xFactor, yFactor);
    *pix = pix->transformed(transform);
    pix->save("../data/temp/imgScale.jpg");
    //delete pix;//�ͷ���Դ
    outputFileName = ("../data/temp/imgScale.jpg");
    emit returnOutputFileName(outputFileName);//�����ź�
}

void geometryTransform::on_rotateButton_clicked()//��ת
{
     pix->load(*inputFileName);
    //��ȡ��ת�̴��ݹ�������ֵ
    QTransform transform;
    transform.rotate(rotateFactor);
    *pix = pix->transformed(transform);//�߶ȱ仯����Ҫ��ֵ��һ������
    pix->save("../data/temp/imgRotate.jpg");
    //delete pix;//�ͷ���Դ
    outputFileName = ("../data/temp/imgRotate.jpg");
    emit returnOutputFileName(outputFileName);//�����ź�
}

void geometryTransform::resetControlPanel()
{
    //�ؼ��ó�ʼλ��
    ui->Xslider_scale->setValue(0);
    ui->Yslider_scale->setValue(0);
    ui->Xslider_translate->setValue(0);
    ui->Yslider_translate->setValue(0);
    ui->dial_rotate->setValue(0);

    //��ֵ��0
    ui->x_scale_current->setValue(1);
    ui->y_scale_current->setValue(1);
    ui->x_translate_current->setValue(0);
    ui->y_translate_current->setValue(0);
    ui->dialNum->setValue(0);
    xFactor = 1;
    yFactor = 1;
    rotateFactor = 0;
}

void geometryTransform::on_resetButton_clicked()//����
{
    resetControlPanel();//���¼���ͼ��ʱ����mainwindowInterator.cpp�б�����
    emit reset();//��������������ͼ
}

geometryTransform::~geometryTransform()
{
    delete ui;
}