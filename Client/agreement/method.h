#ifndef METHOD_H
#define METHOD_H

#include <QString>
#include <QTcpSocket>
#include <QImage>
#include <QFile>
#include <QMutex>

const int MINSIZE = sizeof(qint64) * 2; // totalBytes+cmd

#define     QString_send          0x0F01   //发送字符串
#define     Qimage_send           0x0F02    //发送图片
#define     Qimages_send          0x0F03    //发送一组图片
#define     QFile_send            0x0F04   //发送文件
#define     Struct_send           0x0F05   //发送结构体
// 其他命令
enum COMMAND
{
    CLIENT_START_MONITOR = 0x1000,  //客户端开始监控
//    CLIENT_STOP_MONITOR,   //客户端停止监控
//    CLIENT_MOUSEMOVE,    //客户端鼠标移动
//    CLIENT_LBUTTONDOWN,   //客户端鼠标左键按下
//    CLIENT_LBUTTONUP,     //客户端鼠标左键抬起
//    CLIENT_LBUTTONDBCLICK,  //客户端鼠标左键双击
//    CLIENT_RBUTTONDOwN,    //客户端鼠标右键按下
//    CLIENT_RBUTTONUP,       //客户端鼠标右键抬起
//    CLIENT_RBUTTONDBCLICK,  //客户端鼠标右键双击
    CLIENT_LOGOFF = 0x1001,       //客户端注销
    CLIENT_REBOOT = 0x1002,       //客户端重启
    CLIENT_SHUTDOWN = 0x1003,    //客户端关机
//    CLIENT_EXIT,         //客户端退出
    CLIENT_AUTORUN = 0x1006,      //客户端开机自启动
    CLIENT_UNAUTORUN = 0x1007,      //客户端开机不自启动
//    CLIENTINAGE_DATA    //客户端图像数据
    MOUSE_LOCK = 0x1004,             // 鼠标锁定
    MOUSE_UNLOCK   = 0x1005         // 鼠标解锁
};

//手写结构体结构
struct stu_stateData
{

};

class Method
{

public:
    Method();
    // 写 参数: 通讯套接字 字符串 命令
    void sendStr(QTcpSocket* communicateSocket, QString msg, qint64 cmd);
    void sendImg(QTcpSocket* communicateSocket, QImage image, qint64 cmd);
    void sendImg(QTcpSocket* communicateSocket, QByteArray image, qint64 cmd);  //
    void sendQFile(QTcpSocket* communicateSocket, QString filepath, qint64 cmd);
    void sendStructData(QTcpSocket* communicateSocket, struct stu_stateData* stateData, qint64 cmd);
    // 读 参数: 通讯套接字 字符串 图片 文件路径 文件名 结构体
    void readMsg(QTcpSocket* communicateSocket, QString* resStr = NULL,
                 QPixmap* resImage = NULL, QString filePath = NULL,
                 QString filename = NULL, stu_stateData* resStructData = NULL);


private:
    QByteArray getImageData(const QImage& image);
    QImage getImage(QByteArray& data);
    QByteArray m_buffer;
    qint64 Cmd;
    qint64 totalBytes;      //一个数据包完整大小
    QFile* localFile = NULL;
    QMutex mutex;


};

#endif // METHOD_H
