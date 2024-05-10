#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QWidget>
#include <QTcpSocket>
#include <QTimer>
#include <QLabel>
#include <QHBoxLayout>
#include "agreement/method.h"
#include "picthread.h"

class Client : public QWidget
{
    Q_OBJECT

public:
    Client(QWidget* parent = nullptr);
    ~Client();
private:
    QLabel* label_0 ;
    QHBoxLayout* pHLayout;
    QTcpSocket* clientSocket;    // 客户端的套接字
    void loadSettings();
    void startPassiveConnect();
    Method method;
    QTimer* timer;
    void setAutoStart();
    void stopAutoStart();
    void startTransfer();
    QImage CatchScreen();
    QByteArray getImageData(const QImage& image);

    bool isLockMouse_;
    int lockTimes_;
    void lockMouse();
private slots:
    void sendData();
};
#endif // CLIENT_H
