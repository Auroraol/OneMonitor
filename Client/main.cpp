#include "client.h"
#include "picthread.h"

#include <QApplication>
void Delay_MSec(unsigned int msec);


int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    Client w;
    PicThread* picThread = new PicThread;
    w.show();
    // Delay_MSec(10);
    // w.hide();
    picThread->start();
    return a.exec();
}

void Delay_MSec(unsigned int msec)
{
    QEventLoop loop;//定义一个新的事件循环
    QTimer::singleShot(msec, &loop, SLOT(quit()));  //创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();  //事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
}
