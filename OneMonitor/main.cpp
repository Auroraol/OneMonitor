#include "onemonitor.h"

#include <QApplication>
#include <QTextCodec>

int main(int argc, char* argv[])
{
    QTextCodec* codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
    QApplication a(argc, argv);
    OneMonitor w;
    w.show();
    return a.exec();
}
