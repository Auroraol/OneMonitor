#ifndef PICTHREAD_H
#define PICTHREAD_H

#include <QThread>
#include <QtGui/QScreen>
#include <QMutex>

class PicThread : public QThread
{
    Q_OBJECT

public:
    PicThread(QObject* parent = NULL);
    ~PicThread();

    void run();

private:
    bool getPic(QByteArray& result);

private:
    QScreen* screen_;


    bool getPic(QImage &image);
};

#endif // PICTHREAD_H
