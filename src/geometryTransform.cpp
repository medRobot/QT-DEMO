#include "geometryTransform.h"
#include <QTransform>

geometryTransform::geometryTransform(QWidget *parent , Qt::WFlags flags)
    :QWidget(parent, flags),
    ui(new Ui::geometryTransform)
{
    ui->setupUi(this);

    //初始化
    pix = new QPixmap();
    xFactor = 1;
    yFactor = 1;
    rotateFactor = 0;

    initUI();//初始化界面

    createConnect();//预定义好连接关系
}

void geometryTransform::initUI()
{
    //对控件设置，也可以在QT设计师中设置
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
    //控件间相互连动， 对于一对控件，不管先触发谁都能连动
    connect(ui->x_scale_current, SIGNAL(valueChanged(int)),  ui->Xslider_scale, SLOT(setValue(int)));//放缩
    connect(ui->Xslider_scale,SIGNAL(valueChanged(int)), ui->x_scale_current,   SLOT(setValue(int)));
    connect(ui->y_scale_current, SIGNAL(valueChanged(int)), ui->Yslider_scale, SLOT(setValue(int)));
    connect(ui->Yslider_scale, SIGNAL(valueChanged(int)), ui->y_scale_current, SLOT(setValue(int)));

    connect(ui->x_translate_current,  SIGNAL(valueChanged(int)), ui->Xslider_translate, SLOT(setValue(int)));//平移
    connect(ui->Xslider_translate, SIGNAL(valueChanged(int)), ui->x_translate_current,  SLOT(setValue(int)));
    connect(ui->y_translate_current, SIGNAL(valueChanged(int)),  ui->Yslider_translate, SLOT(setValue(int)));
    connect(ui->Yslider_translate, SIGNAL(valueChanged(int)), ui->y_translate_current,  SLOT(setValue(int)));

    connect(ui->dialNum, SIGNAL(valueChanged(int)), ui->dial_rotate, SLOT(setValue(int)));//旋转
    connect(ui->dial_rotate, SIGNAL(valueChanged(int)), ui->dialNum, SLOT(setValue(int)));
}

//对控件操作，自动连接传递参数
void geometryTransform::on_Xslider_scale_valueChanged(int value)
{
    //在QT设计师“编辑信号/槽”中建立信号与槽自动连接，则不再需要用代码显示表示
    //一个信号可以连接多个槽
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

//每次操作都应该新建一个QTransform变量，否则Transform会对已经执行过的操作再次重复
//几何操作，即可以对图像本身操作，也可以对QGraphicsView类的对象操作
void geometryTransform::on_translateButton_clicked()//平移
{
    pix->load(*inputFileName);
    QTransform transform;
    transform.translate(xFactor, yFactor);

    *pix = pix->transformed(transform);
    pix->save("../data/temp/imgTranslate.jpg");
    //delete pix;//释放资源，如果删除pix， 则不能连续操作
     outputFileName = ("../data/temp/imgTranslate.jpg");
    emit returnOutputFileName(outputFileName);//发送信号时不需要知道由谁来接收信号
}

void geometryTransform::on_scaleButton_clicked()//放缩
{
     pix->load(*inputFileName);
    //获取从滑条传递过来的数值
    QTransform transform;
    transform.scale(xFactor, yFactor);
    *pix = pix->transformed(transform);
    pix->save("../data/temp/imgScale.jpg");
    //delete pix;//释放资源
    outputFileName = ("../data/temp/imgScale.jpg");
    emit returnOutputFileName(outputFileName);//发送信号
}

void geometryTransform::on_rotateButton_clicked()//旋转
{
     pix->load(*inputFileName);
    //获取从转盘传递过来的数值
    QTransform transform;
    transform.rotate(rotateFactor);
    *pix = pix->transformed(transform);//尺度变化后结果要赋值给一个变量
    pix->save("../data/temp/imgRotate.jpg");
    //delete pix;//释放资源
    outputFileName = ("../data/temp/imgRotate.jpg");
    emit returnOutputFileName(outputFileName);//发送信号
}

void geometryTransform::resetControlPanel()
{
    //控件置初始位置
    ui->Xslider_scale->setValue(0);
    ui->Yslider_scale->setValue(0);
    ui->Xslider_translate->setValue(0);
    ui->Yslider_translate->setValue(0);
    ui->dial_rotate->setValue(0);

    //数值置0
    ui->x_scale_current->setValue(1);
    ui->y_scale_current->setValue(1);
    ui->x_translate_current->setValue(0);
    ui->y_translate_current->setValue(0);
    ui->dialNum->setValue(0);
    xFactor = 1;
    yFactor = 1;
    rotateFactor = 0;
}

void geometryTransform::on_resetButton_clicked()//重置
{
    resetControlPanel();//重新加载图像时，在mainwindowInterator.cpp中被调用
    emit reset();//在主窗口重置视图
}

geometryTransform::~geometryTransform()
{
    delete ui;
}