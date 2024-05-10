#include "clock.h"

#include <QTime>
#include <QTimer>
#include <QDebug>
#include <QMenu>
#include<QInputDialog>

Clock::Clock(QWidget* parent)
    : QLabel{parent}
    , overTime("23:00:00")
{
    QPalette p = palette();
    p.setColor(QPalette::Window, Qt::blue);
    setPalette(p);
//    setWindowFlags(Qt::FramelessWindowHint);
//    setWindowOpacity(0.8);
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start(1000);
    showTime();
    resize(230, 50);
    showColon = true;
}

void Clock::showTime()
{
    QTime time = QTime::currentTime();
    QString text = time.toString("hh:mm:ss");
    if(showColon)
    {
        text[2] = ':';
        text[5] = ':';
        showColon = false;
    }
    else
    {
        text[2] = ' ';
        text[5] = ' ';
        showColon = true;
    }
    // 显示时间信息
    QString workTime;
    tiemDiff(workTime);
    setText("当前时间:  " + text + "\n下班还有:\n\t           " + workTime);
}

void Clock::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
    if(event->button() == Qt::RightButton)
    {
        bool ok;
        // 输入文字对话框
        QString string  = QInputDialog::getText(this, "Working hours", "Please enter the work end time", QLineEdit::Normal, "23:00:00", &ok);
        if (ok)
        {
            overTime = string;
        }
    }
}

void Clock::mouseMoveEvent(QMouseEvent* event)
{
    if(event->buttons()&Qt::LeftButton)
    {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

// 实现工作结束时间计时
void Clock::tiemDiff(QString& workTime)
{
    // 时间差值 s
    QDateTime ti = QDateTime::currentDateTime();
    QString time01 = ti.toString("hh:mm:ss");
    QDateTime time1 = QDateTime::fromString(time01, "hh:mm:ss");
    QDateTime time2 = QDateTime::fromString(overTime, "hh:mm:ss");
    qint64 minutes = time1.secsTo(time2);
    //qDebug() << "time2 - time1=" << minutes ;
    workTime = formatTime(minutes * 1000); // 豪秒转时间
}

// 秒转时间
QString Clock::formatTime(int ms)
{
    int secInt = 1000;
    int minInt = secInt * 60;
    int houInt = minInt * 60;
    int dayInt = houInt * 24;
    long day = ms / dayInt;
    long hour = (ms - day * dayInt) / houInt;
    long minute = (ms - day * dayInt - hour * houInt) / minInt ;
    long second = (ms - day * dayInt - hour * houInt - minute * minInt ) / secInt;
    long milliSecond = ms - day * dayInt - hour * houInt - minute * minInt - second * secInt;
    QString dayQStr  = QString::number(day, 10);
    QString houQStr  = QString::number(hour, 10);
    QString minQStr  = QString::number(minute, 10);
    QString secQStr  = QString::number(second, 10);
    QString msecQStr = QString::number(milliSecond, 10);
    //return  dayQStr + "天 " + houQStr + ":" + minQStr + ":" + secQStr + " " + msecQStr;
    return houQStr + ":" + minQStr + ":" + secQStr;
}
