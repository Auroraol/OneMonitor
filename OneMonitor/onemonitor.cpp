#include "onemonitor.h"
#include "ui_onemonitor.h"
#include <QDebug>
#include <QVector>

#include <QGraphicsOpacityEffect>
#include <QMenu>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QVariant>
#include <QMessagebox>
#include <QDir>
#include <QDateTime>
#include <QCommonStyle>
#include <QSettings>
#include <QTextCodec>

#define SERVER_PORT 44444  //服务器端口号
static QString NormalCss = "QPushButton{ \
        color: rgb(0, 255, 255);  \
        background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(0, 0, 0, 0), stop:1 rgba(255, 255, 255, 0)); \
        border:none;               \
        font: italic 15pt \"Californian FB\";  \
        } \
QPushButton:hover{ \
background-color: qlineargradient(spread:pad, x1:0.134, y1:0.493955, x2:1, y2:0.562, stop:0 rgba(35, 130, 175, 223), stop:1 rgba(255, 255, 255, 0));\
        }";

static QString ChangeCss = "QPushButton{ \
    color: rgb(0, 255, 255); \
    background-color: qlineargradient(spread:pad, x1:0.134, y1:0.493955, x2:1, y2:0.562, stop:0 rgba(35, 130, 175, 223), stop:1 rgba(255, 255, 255, 0)); \
    border:none; \
    font: italic 15pt \"Californian FB\"; \
}";

OneMonitor::OneMonitor(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::OneMonitor)
    , flag(false)
    , m_server(new QTcpServer())
    , isConnected(false)
    , isRecord(false)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/img/ioc.ico"));
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    this->resize(1148, 736);
    // this->setMinimumSize(940, 560);
    initTableWidget();
    initButton();
    PageSwitch();
    MeauFunction();  //侧边
    addSystemTray(); // 系统托盘
}

OneMonitor::~OneMonitor()
{
    delete ui;
}

// 初始化表格
void OneMonitor::initTableWidget()
{
    ui->tableWidget->setColumnCount(1);
    QStringList list;
    list << "监控列表";
    ui->tableWidget->setHorizontalHeaderLabels(list);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
}

// 显示表格
void OneMonitor::showInfoTabl()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    // 依次追加, 显示客户端信息[ip, port]
    QMap<QString, QTcpSocket*>::iterator it = m_clientMap.begin();
    while(it != m_clientMap.end())
    {
        int n = ui->tableWidget->rowCount();
        //qDebug() << n;
        ui->tableWidget->insertRow(n);
        ui->tableWidget->setItem(n, 0, new QTableWidgetItem(it.key()));
        it++;
    }
}

// 双击显示监控窗口
void OneMonitor::on_tableWidget_itemDoubleClicked(QTableWidgetItem* item)
{
    if (m_clientMap.contains(item->text()))
    {
        // 样式改变
        ui->stackedWidget->setCurrentWidget(ui->page3);
        ui->Btn_M_Page3->setStyleSheet(ChangeCss);
        ui->Btn_M_Home->setStyleSheet(NormalCss);
        // 客户端的套接字
        m_connctionSocket = m_clientMap[item->text()];
        m_connctionSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1); //尝试优化套接字以降低延迟
        // 发起开始监控命令
        method.sendStr(m_connctionSocket, "CLIENT_START_MONITOR", CLIENT_START_MONITOR);
        // 通讯
        connect(m_connctionSocket, &QTcpSocket::readyRead, this, &OneMonitor::onReadyRead);
        connect(m_connctionSocket, &QTcpSocket::stateChanged, this, &OneMonitor::handerStateChanged);  // 状态改变
        connect(m_connctionSocket, &QAbstractSocket::errorOccurred, this, &OneMonitor::handerSockcteErrorr);  // 错误处理
    }
}

// 服务器接收数据
void OneMonitor::onReadyRead()
{
    //接收:  接收桌面数据,显示在弹出中
    method.readMsg(m_connctionSocket, NULL, &pixmap);
    if (!pixmap.isNull())
    {
        showPixmap(&pixmap);
        QString str = m_connctionSocket->peerAddress().toString();
        ui->label_4->setText(formatIP(str));
    }
}

void OneMonitor::showPixmap(QPixmap* pixmap)
{
    pixmap->scaled(ui->label_tx_ms_7->size(), Qt::IgnoreAspectRatio);//重新调整图像大小以适应窗口
    pixmap->scaled(ui->label_tx_ms_7->size(), Qt::KeepAspectRatio);//设置pixmap缩放的尺寸
    ui->label_tx_ms_7->setScaledContents(true);                    //设置label的属性,能够缩放pixmap充满整个可用的空间。
    ui->label_tx_ms_7->setPixmap(*pixmap);
}

// ip地址转成10进制形式
QString OneMonitor::formatIP(QString ip)
{
    QString str;
    if (ip.contains(":"))
    {
        QStringList tmpList = ip.split(":");
        foreach(QString tmp, tmpList)
        {
            if (tmp.contains("."))
            {
                str = tmp;
                break;
            }
        }
    }
    else
    {
        str = ip;
    }
    return str;
}

void OneMonitor::initButton()
{
    button.push_back(ui->Btn_M_Home);
    button.push_back(ui->Btn_M_Page3);
    button.push_back(ui->Btn_M_Setting);
    // 点击后切换样式表
    for (int i = 0; i < button.size(); i++)
    {
        connect(button[i], &QPushButton::clicked, this, OneMonitor::SignerDeal);
        if (i == button.size() - 1)
        {
            continue;
        }
        connect(button[i], &QPushButton::clicked, this, OneMonitor::StackWigetSwitch);
    }
}

// 页面切换
void OneMonitor::PageSwitch()
{
    connect(button.at(0), &QPushButton::clicked, this, [ = ]()
    {
        ui->stackedWidget->setCurrentWidget(ui->pagehome);
    });
    connect(button.at(1), &QPushButton::clicked, this, [ = ]()
    {
        ui->stackedWidget->setCurrentWidget(ui->page3);
    });
    connect(button.at(2), &QPushButton::clicked, this, [ = ]()
    {
        ui->stackedWidget->setCurrentWidget(ui->pagesetting);
    });
}

//处理后切换样式表
void OneMonitor::SignerDeal()
{
    ui->Btn_M_Home->setStyleSheet(NormalCss);
    ui->Btn_M_Page3->setStyleSheet(NormalCss);
    ui->Btn_M_Setting->setStyleSheet(NormalCss);
    QPushButton* btn = (QPushButton*)sender();
    btn->setStyleSheet(ChangeCss);
}

// 页面渐变入场
void OneMonitor::StackWigetSwitch()
{
    QGraphicsOpacityEffect* opacity = new QGraphicsOpacityEffect(this);
    ui->stackedWidget->setGraphicsEffect(opacity); //应用到需要透明变化的控件；
    //使用属性动画类让控件在透明度范围内变化
    QPropertyAnimation* opacityAnimation = new QPropertyAnimation(opacity, "opacity");
    opacityAnimation->setStartValue(0.0);
    opacityAnimation->setEndValue(1.0);
    opacityAnimation->setDuration(1500); //动效时长1s
    //
    QPropertyAnimation* animation = new QPropertyAnimation;
    animation->setTargetObject(ui->stackedWidget);
    animation->setPropertyName("geometry");
    animation->setStartValue(QRect(190, 70, 0, 0));
    animation->setEndValue(QRect(190, 70, 900, 580));
    animation->setDuration(500);
    //
    QSequentialAnimationGroup* group = new QSequentialAnimationGroup(this);
    group->addAnimation(animation);
    group->addAnimation(opacityAnimation);
    group->start();
}

// 菜单栏
void OneMonitor::MeauFunction()
{
    static int flag = 0;
    QPropertyAnimation* animation = new QPropertyAnimation(ui->Btn_Widget);
    animation->setTargetObject(ui->Btn_Widget);
    if (flag == 0)
    {
        flag = 1;
        // 设置阴影边框;
        QGraphicsDropShadowEffect* d =  new QGraphicsDropShadowEffect(this);
        d->setOffset(-4, -4);
        d->setColor(QColor(73, 229, 237));   // 阴影颜色
        d->setBlurRadius(40);                // 阴影半径;
        ui->Btn_Widget->setGraphicsEffect( d);  // 窗口上的所有控件使用当前的阴影效果;
        // 闪烁动画
        animation->setStartValue(QRect(ui->Btn_Widget->geometry().x(), ui->Btn_Widget->geometry().y(),
                                       ui->Btn_Widget->geometry().width(),
                                       ui->Btn_Widget->geometry().height()));
        animation->setEndValue((QRect(ui->Btn_Widget->geometry().x(), ui->Btn_Widget->geometry().y(),
                                      171, 591)));
    }
    else
    {
        flag = 0;
        ui->Btn_Widget->setGraphicsEffect(NULL);
        animation->setStartValue(QRect(ui->Btn_Widget->geometry().x(), ui->Btn_Widget->geometry().y(),
                                       ui->Btn_Widget->geometry().width(),
                                       ui->Btn_Widget->geometry().height()));
        animation->setEndValue((QRect(ui->Btn_Widget->geometry().x(), ui->Btn_Widget->geometry().y(),
                                      45, 591)));
        animation->setDuration(500);
        animation->setEasingCurve(QEasingCurve::OutCirc);
        animation->start();
    }
}

void OneMonitor::on_Btn_Meau_clicked()
{
    flag = !flag;
    if (flag)
    {
        ui->Btn_Widget->hide();
    }
    else
    {
        ui->Btn_Widget->show();
    }
}

// 开启服务器按钮
void OneMonitor::on_pushButton_fwqc_clicked()
{
    if (m_server->listen(QHostAddress::Any, SERVER_PORT)) // 监听
    {
        qDebug() << QString::fromLocal8Bit("服务器开启") ;
        ui->label_fwq->setText("服务器已开启");
        connect(m_server, &QTcpServer::newConnection, this, &OneMonitor::clinetConnected);
    }
    else
    {
        qDebug() << QString::fromLocal8Bit("Server startup failure") << m_server->errorString();
    }
}

//新增客户端
void OneMonitor::clinetConnected()
{
    QTcpSocket* camera = m_server->nextPendingConnection(); // 获取通讯套接字
    QString ip = camera->peerAddress().toString();
    QString port = QString().setNum(camera->peerPort());
    QString clientName = ip + ":" + port;
    // 使用hash表保存通讯套接字
    m_clientMap[clientName] = camera;  //QMap<QString, QTcpSocket*> clientMap_;
    qDebug() << "新增客户端: " << clientName << "当前客户端个数:" << m_clientMap.size();
    connect(camera, &QTcpSocket::disconnected, this, &OneMonitor::removeClient);  // 通讯套接字没有连接服务器就断开
    //
    showInfoTabl();
    // 通讯
}

//移除客户端
void OneMonitor::removeClient()
{
    QTcpSocket* client = (QTcpSocket*)sender();
    QString ip = client->peerAddress().toString();
    QString port = QString().setNum(client->peerPort());
    QString cameraName = ip + ":" + port;
    if (m_clientMap.contains(cameraName))
    {
        m_clientMap.remove(cameraName);
    }
    qDebug() << "移除客户端: " << cameraName << "当前客户端个数:" << m_clientMap.size();
    //
    showInfoTabl();
}

// 处理连接错误
void OneMonitor::handerSockcteErrorr(QAbstractSocket::SocketError socketError)
{
    switch (socketError)
    {
        case QAbstractSocket::RemoteHostClosedError:
            break;
        case QAbstractSocket::HostNotFoundError:
            QMessageBox::information(this, tr("Fortune Client"),
                                     tr("没有找到服务器。请查看主机名和端口设置"));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            QMessageBox::information(this, tr("Fortune Client"),
                                     tr("The connection was refused by the peer. "
                                        "Make sure the fortune server is running, "
                                        "and check that the host name and port "
                                        "settings are correct."));
            break;
        default:
            QMessageBox::information(this, tr("Fortune Client"),
                                     tr("The following error occurred: %1.")
                                     .arg(m_connctionSocket->errorString()));
    }
}

// 处理连接状态
void OneMonitor::handerStateChanged(QAbstractSocket::SocketState socketState)
{
    switch (socketState)
    {
        case QAbstractSocket::HostLookupState:
        case QAbstractSocket::ConnectingState:
            break;
        case QAbstractSocket::ConnectedState:
            isConnected = true;
            isRecord = true;
            break;
        case QAbstractSocket::BoundState:
        case QAbstractSocket::ListeningState:
            break;
        case QAbstractSocket::ClosingState:
        case QAbstractSocket::UnconnectedState:
            {
                isConnected = false;
                isRecord = false;
                ui->pushButton_3->setText(QString::fromLocal8Bit("Screenshot"));
                break;
            }
        default:
            Q_ASSERT_X(0, "stateChanged", "Unknown socket state!");
    }
}

// 关闭服务器按钮
void OneMonitor::on_pushButton_fwqo_clicked()
{
    ui->label_fwq->setText("服务器已关闭");
    m_server->close();
    QMap<QString, QTcpSocket*>::iterator it = m_clientMap.begin();
    while(it != m_clientMap.end())
    {
        //主动和客户端断开连接
        it.value()->disconnectFromHost();  //断开连接
        it.value()->close();
        it++;
    }
}

// tableWidget右键菜单
void OneMonitor::on_tableWidget_customContextMenuRequested(const QPoint& pos)
{
    QMenu menu;
    menu.setStyleSheet("background-color: #5066c6;");
    QAction* logout = menu.addAction(tr("注销"));
    menu.addSeparator();  //增加分割行
    QAction* reboot = menu.addAction(tr("重启"));
    menu.addSeparator();
    QAction* shutdown = menu.addAction(tr("关机"));
    menu.addSeparator();
    QAction* PowerBoot = menu.addAction(tr("开机自启"));
    menu.addSeparator();
    QAction* UNPowerBoot = menu.addAction(tr("取消开机自启"));
    menu.addSeparator();
    connect(&menu, SIGNAL(triggered(QAction*)), this, SLOT(actionTriggered(QAction*)));
    menu.exec(QCursor::pos());
}

void OneMonitor::actionTriggered(QAction* action)
{
    if (m_connctionSocket->isValid())
    {
        if(action->text() == "注销")
        {
            method.sendStr(m_connctionSocket, "CLIENT_LOGOFF", CLIENT_LOGOFF);
        }
        else if(action->text() == "重启")
        {
            method.sendStr(m_connctionSocket, "CLIENT_REBOOT", CLIENT_REBOOT);
        }
        else if (action->text() == "关机")
        {
            method.sendStr(m_connctionSocket, "CLIENT_SHUTDOWN", CLIENT_SHUTDOWN);
        }
        else if (action->text() == "开机自启")
        {
            method.sendStr(m_connctionSocket, "CLIENT_AUTORUN", CLIENT_AUTORUN);
        }
        else if (action->text() == "取消开机自启" )
        {
            method.sendStr(m_connctionSocket, "CLIENT_UNAUTORUN", CLIENT_UNAUTORUN);
        }
    }
}

// 截图
void OneMonitor::on_pushButton_3_clicked()
{
    if (isConnected)
    {
        return;
    }
    QString path = QApplication::applicationDirPath();
    path += tr("/pic/");
    QDir dir(path);
    if (!dir.exists())
    {
        if (!(dir.mkdir(path)))
        {
            path == QString("C:\\");
        }
    }
    QString ip = ui->label_4->text();
    QString fileName = path + ip + " " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss") + ".bmp";
    bool isok = pixmap.save(fileName);
    QMessageBox box;
    if (isok)
    {
        box.setStyleSheet("QMessageBox{background-color: #536ace;\
                 border:1px solid #CCFFF6;\
                border-radius:3px;\
                font: italic 15pt \"Californian FB\";  \
                }");
        box.setWindowTitle("Screenshot");
        box.setText(QString::fromLocal8Bit("Successful preservation: \n%1").arg(fileName));
        box.exec();
    }
}

// 窗口拖拽
void OneMonitor::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bDrag = true;
        //获得鼠标的初始位置
        mouseStartPoint = event->globalPos();
        //mouseStartPoint = event->pos();
        //获得窗口的初始位置
        windowTopLeftPoint = this->frameGeometry().topLeft();
    }
}

void OneMonitor::mouseMoveEvent(QMouseEvent* event)
{
    if(m_bDrag)
    {
        //获得鼠标移动的距离
        QPoint distance = event->globalPos() - mouseStartPoint;
        //QPoint distance = event->pos() - mouseStartPoint;
        //改变窗口的位置
        this->move(windowTopLeftPoint + distance);
    }
}

void OneMonitor::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bDrag = false;
    }
}

// 系统托盘
void OneMonitor::addSystemTray()
{
    QCommonStyle style;
    m_trayMenu = new QMenu();
    m_tray = new QSystemTrayIcon;
    m_trayMenu->addAction(QIcon(style.standardPixmap(QStyle::SP_ComputerIcon)), "打开主界面");
    m_trayMenu->addAction(QIcon(style.standardPixmap(QStyle::SP_MessageBoxInformation)), "设置");
    m_trayMenu->addAction(QIcon(style.standardPixmap(QStyle::SP_DialogCancelButton)), "退出");
    //托盘加入菜单
    m_tray->setToolTip("OneMonitor");
    m_tray->setIcon(QIcon(":/img/ioc.ico"));  // 必须加
    m_tray->setContextMenu(m_trayMenu);
    //绑定托盘小图标点击事件
    connect(m_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(OnSystemTrayClicked(QSystemTrayIcon::ActivationReason)));
    //菜单子项触发
    connect(m_trayMenu, SIGNAL(triggered(QAction*)), this, SLOT(acTriggered(QAction*)));
    m_tray->show(); //显示小图标
}

//点击了任务栏托盘小图标
void OneMonitor::OnSystemTrayClicked(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
            this->showNormal();
            break;
        default:
            break;
    }
}

void OneMonitor::acTriggered(QAction* action)
{
    if(action->text() == "打开主界面")
    {
        this->showNormal();
    }
    else if(action->text() == "退出")
    {
        this->hide();
        qApp->exit(0);
    }
    else if (action->text() == "设置")
    {
        this->showNormal();
        ui->stackedWidget->setCurrentWidget(ui->pagesetting);
        ui->Btn_M_Setting->setStyleSheet(ChangeCss);
        ui->Btn_M_Home->setStyleSheet(NormalCss);
        ui->Btn_M_Page3->setStyleSheet(NormalCss);
    }
}

// 最小化
void OneMonitor::on_Btn_Lim_2_clicked()
{
    this->showMinimized();
}

// 关闭
void OneMonitor::on_Btn_exit_Big_W_clicked()
{
    this->close();
}

void OneMonitor::on_Btn_M_Exit_clicked()
{
    this->close();
}

void OneMonitor::on_Btn_M_Qu_clicked()
{
    QMessageBox box;
    box.setStyleSheet("QMessageBox{background-color: #536ace;\
             border:1px solid #CCFFF6;\
            border-radius:3px;\
            font: italic 15pt \"Californian FB\";  \
            }");
    box.setWindowTitle(QString::fromLocal8Bit("About OneMonitor"));
    box.setText(QString::fromLocal8Bit("OneMonitor V1.0 <br>Create by LFJ,  2023/3/14"));
    box.exec();
}

// 开启自启
void OneMonitor::setAutoStart()
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
void OneMonitor::stopAutoStart()
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

// 设置程序是否开机自启
void OneMonitor::on_radioButton_clicked()
{
    if (ui->radioButton->isChecked())
    {
        setAutoStart();
    }
    else
    {
        stopAutoStart();
    }
}

//搜索
void OneMonitor::on_Btn_Sreah_clicked()
{
    QString str =  ui->lineEdit->text();
    tbSearchByText(str);
}

void OneMonitor::tbSearchByText(QString text)
{
    QList<QTableWidgetItem*> findItems = ui->tableWidget->findItems(text, Qt::MatchContains); // 模糊匹配
    int nRow = ui->tableWidget->rowCount();
    bool bRowHidden = true;
    for(int i = 0; i < nRow; ++i)
    {
        bRowHidden = true;
        foreach(QTableWidgetItem* item, findItems)
        {
            if(NULL == item)
            {
                continue;
            }
            if(ui->tableWidget->row(item) == i)
            {
                bRowHidden = false;
                break;
            }
        }
        ui->tableWidget->setRowHidden(i, bRowHidden);
    }
}

// 鼠标  音频   关机 重启  // 资源进度图
//

// 城市更换切换天气
void OneMonitor::on_pushButton_clicked()
{
    // 发出消息
    if (ui->lineEdit_cityName->text().isEmpty())
    {
        return;
    }
    cityName = ui->lineEdit_cityName->text();   // 向Weather类发出信号
    if (cityName.isEmpty())
    {
        cityName = "长春";
    }
    if (cityName == cityName.left(cityName.size() - 1) + "市")
    {
        ui->widget_2->getWeatherInfo(cityName.left(cityName.size() - 1));  //ui->widget_2继承Weather类
    }
    else
    {
        ui->widget_2->getWeatherInfo(cityName);
    }
}

// 进入天气页面就更新当前天气
void OneMonitor::on_tabWidget_tabBarClicked(int index)
{
    if (index == 1)
    {
        ui->widget_2->getWeatherInfo(cityName);
    }
}

// 点击鼠标悬停
void OneMonitor::on_pushButton_2_clicked()
{
    if (m_connctionSocket != NULL)
    {
        method.sendStr(m_connctionSocket, "MOUSE_LOCK", MOUSE_LOCK);
    }
}
// 点击解锁
void OneMonitor::on_pushButton_6_clicked()
{
    if (m_connctionSocket != NULL)
    {
        method.sendStr(m_connctionSocket, "MOUSE_UNLOCK", MOUSE_UNLOCK);
    }
}
// 关机
void OneMonitor::on_pushButton_5_clicked()
{
    if (m_connctionSocket != NULL)
    {
        method.sendStr(m_connctionSocket, "CLIENT_SHUTDOWN", CLIENT_SHUTDOWN);
    }
}
// 重启
void OneMonitor::on_pushButton_4_clicked()
{
    if (m_connctionSocket != NULL)
    {
        method.sendStr(m_connctionSocket, "CLIENT_REBOOT", CLIENT_REBOOT);
    }
}
// 注销
void OneMonitor::on_pushButton_7_clicked()
{
    if (m_connctionSocket != NULL)
    {
        method.sendStr(m_connctionSocket, "CLIENT_LOGOFF", CLIENT_LOGOFF);
    }
}
