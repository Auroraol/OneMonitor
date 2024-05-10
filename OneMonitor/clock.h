#ifndef CLOCK_H
#define CLOCK_H

#include <QLabel>
#include <QMouseEvent>

class Clock : public QLabel
{
    Q_OBJECT
public:
    explicit Clock(QWidget* parent = nullptr);
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
private:
    QPoint dragPosition;
    bool showColon;
    QString overTime;  //工作结束时间
    QString formatTime(int ms);
    void tiemDiff(QString& workTime);
signals:
public slots:
    void showTime();

};

#endif // CLOCK_H
