#include "picthread.h"
#include <QtGui/QPixmap>
#include <QApplication>
#include <QDesktopWidget>
#include <QBuffer>
#include <QDebug>

QMutex g_bufferLocker_;  // 互斥锁
QList<QByteArray> g_BufferList_;

PicThread::PicThread(QObject* parent)
    : QThread(parent)
{
    screen_ = QApplication::primaryScreen();
}

PicThread::~PicThread()
{
}

//void PicThread::run()
//{
//    while (true)
//    {
//        msleep(100);
//        QImage sendImage;
//        bool isok = getPic(sendImage);  // 获得图片
//        if (isok)
//        {
//            g_bufferLocker_.lock();  // 加锁
//            if (g_BufferList_.size() > 10)
//            {
//                qDebug() << "大小太长.. 删除一个";
//                g_BufferList_.clear();
//            }
//            g_BufferList_.append(sendImage);
//            g_bufferLocker_.unlock();
//        }
//        else
//        {
//            qDebug() << "获取图片错误";
//        }
//    }

//}

void PicThread::run()
{
    while (true)
    {
        msleep(100);
        QByteArray sendArray;
        bool isok = getPic(sendArray);  // 获得图片
        if (isok)
        {
            g_bufferLocker_.lock();  // 加锁
            if (g_BufferList_.size() > 10)
            {
                qDebug() << "大小太长.. 删除一个";
                g_BufferList_.clear();
            }
            g_BufferList_.append(sendArray);    //QList<QByteArray> g_BufferList_;
            g_bufferLocker_.unlock();
        }
        else
        {
            qDebug() << "获取图片错误";
        }
    }
}

// 获得图片
bool PicThread::getPic(QImage& image)
{
    QPixmap pic = screen_->grabWindow(0);
    image = pic.toImage();  // 返回QImag
    if(image.isNull())
    {
        return false;
    }
    return true;
}

// 获得图片
bool PicThread::getPic(QByteArray& result)
{
    QPixmap pic = screen_->grabWindow(0);
    // 写入图片到内存中
    QBuffer buffer(&result);
    buffer.open(QIODevice::WriteOnly);
    bool isok = pic.save(&buffer, "jpg");
    return isok;
}
