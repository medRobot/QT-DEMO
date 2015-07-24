#include "mainwindow.h"
#include "mainwindowInterator.h"
#include <QtGui/QApplication>
#include <QTextCodec>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));//能识别中文
    //mainWindow w;//主框架实例化
    mainwindowInterator w;//实例化主框架子类
    w.setWindowTitle(QObject::tr("数字图像处理软件"));//设置应用程序标题
    //设置应用程序图标，相对路径
    w.setWindowIcon(QIcon("../data/logo/QT.ico"));//小图标改变，exe文件图标不变
    w.show();
    return a.exec();
}
