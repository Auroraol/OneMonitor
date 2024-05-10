#ifndef WEATHER_H
#define WEATHER_H

#include <QLabel>
#include <QMap>
#include <QNetworkAccessManager>
#include <QWidget>

namespace Ui
{
    class Weather;
}


class Today
{
public:
    Today()
    {
        date = "2023-3-4";
        city = "长春";
        ganmao = "感冒指数";
        wendu = "0";
        shidu = "09%";
        pm25 = 0;
        quality = "无数据";
        type = "多云";
        fl = "2级";
        fx = "南风";
        high = 30;
        low = 18;
    }

    QString date;   // 时间
    QString city;   // 城市

    QString ganmao; // 感冒指数

    QString wendu;      // 温度
    QString shidu;  // 湿度
    int pm25;       // pm2.5
    QString quality; // 质量

    QString type;    // 类型

    QString fx;      // 风向
    QString fl;      // 风能

    double high;       // 高
    double low;        // 低

};


class Weather : public QWidget
{
    Q_OBJECT

public:
    explicit Weather(QWidget* parent = nullptr);
    ~Weather();
    void getWeatherInfo(QString city); // 给OneMonitor类调用

private slots:
    void replyFinished(QNetworkReply* reply);
private:
    Ui::Weather* ui;
    Today m_Today;
    QMap <QString, QString> mapCode;   //  放城市的编码;
    QMap <QString, QString> mTypeMap;  // 放天气图片的路径
    void initCityCodeMap(QMap<QString, QString>& mapCode);


    // 解析
    QNetworkAccessManager* manager;
    QNetworkRequest* request;

    void parseJson(QByteArray& data);
    void initUIList();
    void updataUI();
};


#endif // WEATHER_H
