#include "client.h"
#include <Windows.h>

#include <QApplication>
#include <QBuffer>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QScreen>
#include <QSettings>
#include <QString>
#include <QString>
#include <QTextCodec>


#define SERVER_PORT 44444  //服务器端口号
extern QMutex g_bufferLocker_;
extern QList<QByteArray> g_BufferList_;//图片数据

Client::Client(QWidget* parent)
    : QWidget(parent)
    , isLockMouse_(false)
{
    clientSocket = new QTcpSocket;
    clientSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1); //尝试优化套接字以降低延迟
    loadSettings();
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Client::sendData);
    setAutoStart();
    qDebug() << "开启客户端";
}

Client::~Client()
{
}


// 设置配置文件
void Client::loadSettings()
{
    QSettings settings("config.ini", QSettings::IniFormat);
    settings.beginGroup("REMOTEDESKTOPSERVER"); // 开始群组
    QString ServerHost = settings.value("ServerHost").toString();
    if(ServerHost.isEmpty())
    {
        ServerHost = "127.0.0.1";
        settings.setValue("ServerHost", ServerHost);
    }
    int ServerPort = settings.value("ServerPort", 0).toInt();
    if(0 == ServerPort)
    {
        ServerPort = SERVER_PORT;
        settings.setValue("ServerPort", ServerPort);
    }
    settings.endGroup();  // 结束群组
    settings.sync();
    //链接(建立链接）   参数: 服务器ip 服务器端口
    clientSocket->connectToHost(ServerHost, ServerPort);
    startPassiveConnect();
    pHLayout = new QHBoxLayout();
    label_0 = new QLabel;
    label_0->setText(QString("开启客户端: %1").arg(ServerHost));
    pHLayout->addWidget(label_0);
    this->setLayout(pHLayout);
}

//被监控端启动
void Client::startPassiveConnect()
{
    // 通讯
    connect(clientSocket, &QTcpSocket::readyRead, this, [ = ]()
    {
        // 接收:  接收开始监控命令, 开始传输客户端桌面数据
        QString resStr;
        method.readMsg(clientSocket, &resStr);
        if (resStr.compare("CLIENT_START_MONITOR") == 0)
        {
            // 发送:  桌面数据, 每隔20ms
            timer->start(20);
        }
        if (resStr.compare("MOUSE_LOCKMOUSE_LOCKMOUSE_LOCKMOUSE_LOCKMOUSE_LOCKMOUSE_LOCKMOUSE_LOCK") == 0)
        {
            qDebug() << resStr;
            // 点击鼠标悬停
            isLockMouse_ = true;
        }
        if (resStr.compare("MOUSE_UNLOCKMOUSE_UNLOCKMOUSE_UNLOCKMOUSE_UNLOCKMOUSE_UNLOCKMOUSE_UNLOCK") == 0)
        {
            qDebug() << resStr;
            // 点击解锁
            isLockMouse_ = false;
        }
        if (resStr.compare("CLIENT_REBOOTCLIENT_REBOOTCLIENT_REBOOTCLIENT_REBOOT") == 0)
        {
            qDebug() << resStr;
            // 重启
            system("shutdown -r -t 10");
        }
        if (resStr.compare("CLIENT_SHUTDOWNCLIENT_SHUTDOWNCLIENT_SHUTDOWN") == 0)
        {
            qDebug() << resStr;
            // 关机
            system("shutdown -s -t 10");
        }
        if (resStr.compare("CLIENT_LOGOFFCLIENT_LOGOFFCLIENT_LOGOFFCLIENT_LOGOFFCLIENT_LOGOFF") == 0)
        {
            qDebug() << resStr;
            // 注销
            system("shutdown -l -t 10");
        }
        if (resStr.compare("CLIENT_AUTORUNCLIENT_AUTORUN") == 0)
        {
            qDebug() << resStr;
            // 开机自启
            setAutoStart();
        }
        if (resStr.compare("CLIENT_UNAUTORUN") == 0)
        {
            // 开机自启
            qDebug() << resStr;
            stopAutoStart();
        }
    });
}

QImage Client::CatchScreen()
{
    QScreen* screen = QGuiApplication::primaryScreen();
    return screen->grabWindow(0).toImage();  // 返回QImag
}

// 发送:  桌面数据, 每隔20ms
void Client::sendData()
{
    if (isLockMouse_)
    {
        lockTimes_++;
        if (lockTimes_ > 3000)
        {
            isLockMouse_ = false;
            lockTimes_ = 0; // 锁定鼠标
        }
        lockMouse();
    }
    else
    {
        lockTimes_ = 0;
    }
    //
    QByteArray data;
    g_bufferLocker_.lock();
    if (g_BufferList_.size() > 0)
    {
        data = g_BufferList_.takeFirst();  // list[0], 发的是图片
    }
    g_bufferLocker_.unlock();
    if (data.length() > 0)
    {
        method.sendImg(clientSocket, data, Qimages_send);
    }
}

// 开启自启
void Client::setAutoStart()
{
    QString sApp = QApplication::applicationFilePath();//我的程序名称
    sApp.replace("/", "\\");
    QSettings* setting = new QSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion", QSettings::NativeFormat);
    QTextCodec* codec = QTextCodec::codecForName("GBK");
    setting->setIniCodec(codec);
    //开机自动运行
    setting->beginGroup("Run");
    setting->setValue("Monitor.exe", QVariant(sApp));
    setting->endGroup();
    delete setting;
    setting = NULL;
}

//取消开机自启
void Client::stopAutoStart()
{
    QString sApp = QApplication::applicationFilePath();//程序名称
    sApp.replace("/", "\\");
    QSettings* setting = new QSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion", QSettings::NativeFormat);
    QTextCodec* codec = QTextCodec::codecForName("GBK");
    setting->setIniCodec(codec);
    //取消开机自动运行
    setting->beginGroup("Run");
    setting->remove("Monitor.exe");
    setting->endGroup();
    delete setting;
    setting = NULL;
}

// 锁定鼠标
void Client::lockMouse()
{
    //QPoint pos = QCursor::pos();
    //pos.setX(pos.x());
    //pos.setY(pos.y());
    QCursor::setPos(QPoint(0, 0));
}
