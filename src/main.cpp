#include "mainwindow.h"
#include "mainwindowInterator.h"
#include <QtGui/QApplication>
#include <QTextCodec>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));//��ʶ������
    //mainWindow w;//�����ʵ����
    mainwindowInterator w;//ʵ�������������
    w.setWindowTitle(QObject::tr("����ͼ�������"));//����Ӧ�ó������
    //����Ӧ�ó���ͼ�꣬���·��
    w.setWindowIcon(QIcon("../data/logo/QT.ico"));//Сͼ��ı䣬exe�ļ�ͼ�겻��
    w.show();
    return a.exec();
}
