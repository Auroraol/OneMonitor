#include "weather.h"
#include "ui_weather.h"
#include "onemonitor.h"

#include <QFile>
#include <QDebug>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

extern QString cityName;

Weather::Weather(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::Weather)
{
    ui->setupUi(this);
    initUIList();
    initCityCodeMap(mapCode);
    //
    manager = new QNetworkAccessManager(this);
    request = new QNetworkRequest;
    connect(manager, &QNetworkAccessManager::finished, this, &Weather::replyFinished);
    getWeatherInfo("长春"); // 默认显示的城市
}

Weather::~Weather()
{
    delete ui;
}

// 初始化城市编码
void Weather::initCityCodeMap(QMap<QString, QString>& mapCode)
{
    //QFile file(QCoreApplication::applicationDirPath() + "/citycode.txt");
    QFile file("C:/Qt/code/Project7/OneMonitor/city_code/citycode.txt");
    bool a = file.open(QFile::ReadOnly);
    if (a == false)
    {
        qDebug() << "警告文件打开失败";
        return ;
    }
    QByteArray array ;
    while (!file.atEnd())             // atEnd读取到文件末尾
    {
        QString buf = file.readLine();
        if (buf.isEmpty() || buf == "\r\n")
        {
            continue;
        }
        //qDebug() << buf;
        QStringList tempStr = buf.split("=");
        QString str = tempStr[1].left(tempStr[1].size() - 2);
        if (str.isEmpty())
        {
            continue;
        }
        mapCode.insert(str, tempStr[0]);
    }
    mapCode.insert("新港", "101340904 ``  ``          KKK");
    //关闭文件
    file.close();
}

// 得到天气信息
void Weather::getWeatherInfo(QString city)
{
    QString cityStr = mapCode.value(city);
    //qDebug() << cityStr;
    QString url = "http://t.weather.itboy.net/api/weather/city/" + cityStr.toUtf8().toPercentEncoding();
    request->setUrl(QUrl(url));
    request->setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8"); //在请求中，有一种数据传输的Content-Type 是application/x-www-form-urlencoded。
    request->setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.198 Safari/537.36");
    manager->get(*request);
}

void Weather::replyFinished(QNetworkReply* reply)
{
    QByteArray data = reply->readAll();
    parseJson(data);
}

//解析数据
void Weather::parseJson(QByteArray& data)
{
    //解析
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(data, &error);
    //解析出错判断
    if (document.isNull() || error.error != QJsonParseError::NoError)
    {
        qDebug() << error.errorString();  //返回JSON解析错误时报告的错误信息
        return;
    }
    //
    QJsonObject rootObj = document.object(); // 转化为对象
    if (rootObj.value("status").toInt() != 200)
    {
        qDebug() << "(返回结果不为200：)" << rootObj.value("message").toString();
        return;
    }
    // 解析日期和城市
    m_Today.date = rootObj.value("date").toString();
    m_Today.city = rootObj.value("cityInfo").toObject().value("city").toString();
    // 预测的后五天天气信息
    QJsonArray forecast = rootObj.value("data").toObject().value("forecast").toArray();
    // 获取数组的第一个元素，类型是QJsonValue
    QJsonValue forecastValueChild = forecast.at(0);
    // 判断是不是对象类型
    if (forecastValueChild.type() == QJsonValue::Object)
    {
        // 转换成对象类型
        QJsonObject forecastObj = forecastValueChild.toObject();
        m_Today.type = forecastObj.value("type").toString();
        QString s;
        int len;
        s = forecastObj.value("high").toString().split(" ").at(1);
        len = s.size();
        QString high = s.left(len - 1);
        m_Today.high = high.toDouble();
        //qDebug() <<  m_Day[i + 1].high;
        s = forecastObj.value("low").toString().split(" ").at(1);
        len = s.size();
        QString low = s.left(len - 1);
        m_Today.low = low.toDouble();
        //风向风力
        m_Today.fx = forecastObj.value("fx").toString();
        m_Today.fl = forecastObj.value("fl").toString();
    }
// 解析今天的数据
    m_Today.ganmao = rootObj.value("data").toObject().value("ganmao").toString();
    m_Today.wendu = rootObj.value("data").toObject().value("wendu").toString();
    m_Today.shidu = rootObj.value("data").toObject().value("shidu").toString();
    m_Today.pm25 = rootObj.value("data").toObject().value("pm25").toInt();
    m_Today.quality = rootObj.value("data").toObject().value("quality").toString();
// 更新 UI数据
    updataUI();
}

void Weather::initUIList()
{
    //
    mTypeMap.insert("暴雪", ":/img_weather/type/BaoXue.png");
    mTypeMap.insert("暴雨", ":/img_weather/type/BaoYu.png");
    mTypeMap.insert("暴雨转大暴雨", ":/img_weather/type/BaoYuDaoDaBaoYu.png");
    mTypeMap.insert("大暴雨", ":/img_weather/type/DaBaoYu.png");
    mTypeMap.insert("大暴雨转特大暴雨", ":/img_weather/type/DaBaoYuDaoTeDaBaoYu.png");
    mTypeMap.insert("大到暴雪", ":/img_weather/type/DaDaoBaoXue.png");
    mTypeMap.insert("大雪", ":/img_weather/type/DaXue.png");
    mTypeMap.insert("大到暴雨", ":/img_weather/type/DaDaoBaoYu.png");
    mTypeMap.insert("大雨", ":/img_weather/type/DaYu.png");
    mTypeMap.insert("冻雨", ":/img_weather/type/DongYu.png");
    mTypeMap.insert("多云", ":/img_weather/type/DuoYun.png");
    mTypeMap.insert("浮尘", ":/img_weather/type/FuChen.png");
    mTypeMap.insert("雷阵雨", ":/img_weather/type/LeiZhenYu.png");
    mTypeMap.insert("雷阵雨伴有冰雹", ":/img_weather/type/LeiZhenYuBanYouBingBao.png");
    mTypeMap.insert("霾", ":/img_weather/type/Mai.png");
    mTypeMap.insert("强沙尘暴", ":/img_weather/type/QiangShaChenBao.png");
    mTypeMap.insert("晴", ":/img_weather/type/Qing.png");
    mTypeMap.insert("沙尘暴", ":/img_weather/type/ShaChenBao.png");
    mTypeMap.insert("特大暴雨", ":/img_weather/type/TeDaBaoYu.png");
    mTypeMap.insert("undefined", ":/img_weather/type/undefined.png");
    mTypeMap.insert("雾", ":/img_weather/type/Wu.png");
    mTypeMap.insert("小到中雪", ":/img_weather/type/XiaoDaoZhongXue.png");
    mTypeMap.insert("小到中雨", ":/img_weather/type/XiaoDaoZhongYu.png");
    mTypeMap.insert("小雨", ":/img_weather/type/XiaoXue.png");
    mTypeMap.insert("小雪", ":/img_weather/type/XiaoYu.png");
    mTypeMap.insert("雨", ":/img_weather/type/Xue.png");
    mTypeMap.insert("扬沙", ":/img_weather/type/YangSha.png");
    mTypeMap.insert("阴", ":/img_weather/type/Yin.png");
    mTypeMap.insert("雨", ":/img_weather/type/Yu.png");
    mTypeMap.insert("雨夹雪", ":/img_weather/type/YuJiaXue.png");
    mTypeMap.insert("阵雪", ":/img_weather/type/ZhenXue.png");
    mTypeMap.insert("阵雨", ":/img_weather/type/ZhenYu.png");
    mTypeMap.insert("中到大雪", ":/img_weather/type/ZhongDaoDaXue.png");
    mTypeMap.insert("中到大雨", ":/img_weather/type/ZhongDaoDaYu.png");
    mTypeMap.insert("中雪", ":/img_weather/type/ZhongXue.png");
    mTypeMap.insert("中雨", ":/img_weather/type/ZhongYu.png");
}

void Weather::updataUI()
{
    //qDebug() << m_Today.date;
    ui->lblCity_2->setText(m_Today.city);
    // 更新今天
    ui->lblTypeIcon->setPixmap(mTypeMap[m_Today.type]);
    ui->lblTemp_2->setText(m_Today.wendu + "℃");
    ui->lblType_2->setText(m_Today.type);
    ui->lblLowHigh_2->setText(QString::number(m_Today.low) + "℃～" + QString::number(m_Today.high) + "℃");
    ui->lblGanMao->setText("感冒指数：" + m_Today.ganmao);
    ui->lblWindFx ->setText(m_Today.fx);
    ui->lblWindFl->setText(m_Today.fl);
    ui->lblPM25->setText(QString::number(m_Today.pm25));
    ui->lblShiDu->setText(m_Today.shidu);
    ui->lblQuality->setText(m_Today.quality);
}


