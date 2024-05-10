#ifndef ONEMONITOR_H
#define ONEMONITOR_H

#include <QPushButton>
#include <QTableWidgetItem>
#include <QTcpServer>
#include <QTcpSocket>
#include <QWidget>
#include <QMouseEvent>
#include <QSystemTrayIcon>
#include "agreement/method.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class OneMonitor;
}
QT_END_NAMESPACE

class OneMonitor : public QWidget
{
    Q_OBJECT

public:
    OneMonitor(QWidget* parent = nullptr);
    ~OneMonitor();

private slots:
    void SignerDeal();
//    void ValueAceeptDown();
    void StackWigetSwitch();
    void MeauFunction();
    void on_Btn_Meau_clicked();

    void on_pushButton_fwqc_clicked();
    void clinetConnected();

    void on_pushButton_fwqo_clicked();
    void on_tableWidget_customContextMenuRequested(const QPoint& pos);
    void actionTriggered(QAction* action);

    void on_tableWidget_itemDoubleClicked(QTableWidgetItem* item);

    void onReadyRead();
    void on_pushButton_3_clicked();

    void handerSockcteErrorr(QAbstractSocket::SocketError socketError);
    void handerStateChanged(QAbstractSocket::SocketState socketState);
    void on_Btn_Lim_2_clicked();

    //
    void OnSystemTrayClicked(QSystemTrayIcon::ActivationReason reason);
    void acTriggered(QAction* action);

    void on_Btn_exit_Big_W_clicked();

    void on_Btn_M_Exit_clicked();

    void on_Btn_M_Qu_clicked();

    void on_pushButton_4_clicked();

    void on_radioButton_clicked();

    void on_Btn_Sreah_clicked();



    void on_pushButton_clicked();

    void on_tabWidget_tabBarClicked(int index);

    void on_pushButton_2_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_7_clicked();

private:
    Ui::OneMonitor* ui;
    QVector<QPushButton*> button;  // ui按钮
    QTcpServer* m_server;                    // 服务器套接字
    QMap<QString, QTcpSocket*> m_clientMap;  // 保存通许套接字
    QString currentClient;
    bool flag;

    //
    bool isRecord;
    bool isConnected;
    QPixmap pixmap;  // 图片数据
    //
    QTcpSocket* m_connctionSocket = NULL;  // 通讯套接字
    Method method;

    void setqssstyle(const QString& qssFile, QPushButton* btn);

    void PageSwitch();
    void initButton();

    void initTableWidget();
    void showInfoTabl();

    void removeClient();

    QString formatIP(QString ip);
    void showPixmap(QPixmap* pixmap);

    //拖拽窗口
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    bool        m_bDrag;
    QPoint      mouseStartPoint;
    QPoint      windowTopLeftPoint;
    // 自启
    void setAutoStart();
    void stopAutoStart();
    // 系统托盘
    QSystemTrayIcon* m_tray;//任务栏托盘角标
    QMenu* m_trayMenu;
    void addSystemTray();

    QString cityName;
    void tbSearchByText(QString text); // 搜索

//signals:
//    void  switchCity(QString name);
};
#endif // ONEMONITOR_H
