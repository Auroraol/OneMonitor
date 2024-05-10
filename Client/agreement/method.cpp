#include "method.h"

#include <QDataStream>
#include <QDebug>
#include <QBuffer>
#include <QDir>
#include <QPixmap>


Method::Method()
{
}

void Method::sendStr(QTcpSocket* communicateSocket, QString msg, qint64 cmd)
{
    if(!communicateSocket->isValid()) //确保连接仍然有效
    {
        qDebug() << "losing connect.......";
        return;
    }
    /********************构造数据包************************/
    qint64 totalBytes = 0;
    QByteArray block; //用于暂存我们要发送的数据
    QDataStream output(&block, QIODevice::WriteOnly); //使用数据流写入数据
    output.setVersion(QDataStream::Qt_5_2);
    totalBytes = msg.toUtf8().size(); // 数据大小
    //  totalBytes + cmd  构成了包头, 长度为2*qint64 也就是头文件中定义的MINSIZE
    output << qint64(totalBytes) << qint64(cmd);
    //
    totalBytes += block.size();//加上上一行内容的长度
    output.device()->seek(0);//回到数据流的开始位置, 将设备的指针位置置为0，方便后面重新给值
    output << totalBytes; //得到 数据包长度
    communicateSocket->write(block);
    block.resize(0);//清空
    for(int i = 0; i < 10000; i++); //延时
    block = msg.toUtf8();
    communicateSocket->write(block);//发送命令内容  // 发送数据
    qDebug() << "字符串发送完成!";
    block.resize(0);
}

void Method::sendImg(QTcpSocket* communicateSocket, QImage image, qint64 cmd)
{
    if(!communicateSocket->isValid()) //确保连接仍然有效
    {
        qDebug() << "losing connect.......";
        return;
    }
    QString imageData = getImageData(image); //png 编码为[base64] QByteArray数据
    /********************构造数据包************************/
    qint64 totalBytes = 0;
    QByteArray block; //用于暂存我们要发送的数据
    QDataStream output(&block, QIODevice::WriteOnly); //使用数据流写入数据
    output.setVersion(QDataStream::Qt_5_2);
    totalBytes = imageData.toUtf8().size();
    // totalBytes+cmd  构成了包头 长度为2*qint64 也就是头文件中定义的MINSIZE
    output << qint64(totalBytes)  << qint64(cmd);
    totalBytes += block.size(); //得到数据包总大小
    output.device()->seek(0);  //回到数据流的开始位置
    output << totalBytes;
    communicateSocket->write(block);
    block.resize(0);//清空
    for(int i = 0; i < 10000; i++); //延时
    block = imageData.toUtf8();
    communicateSocket->write(block);
    qDebug() << "图片发送完成!";
    block.resize(0);//清空
}

void Method::sendImg(QTcpSocket* communicateSocket, QByteArray image, qint64 cmd)
{
    if(!communicateSocket->isValid()) //确保连接仍然有效
    {
        qDebug() << "losing connect.......";
        return;
    }
    /********************构造数据包************************/
    qint64 totalBytes = 0;
    QByteArray block; //用于暂存我们要发送的数据
    QDataStream output(&block, QIODevice::WriteOnly); //使用数据流写入数据
    output.setVersion(QDataStream::Qt_5_2);
    totalBytes = image.size();
    // totalBytes+cmd  构成了包头 长度为2*qint64 也就是头文件中定义的MINSIZE
    output << qint64(totalBytes)  << qint64(cmd);
    totalBytes += block.size(); //得到数据包总大小
    output.device()->seek(0);  //回到数据流的开始位置
    output << totalBytes;
    communicateSocket->write(block);
    block.resize(0);//清空
    for(int i = 0; i < 10000; i++); //延时
    block = image;
    communicateSocket->write(block);
    //qDebug() << "图片发送完成!";
    block.resize(0);//清空
}

QByteArray Method::getImageData(const QImage& image)
{
    QByteArray imageData;
    QBuffer buffer(&imageData);
    image.save(&buffer, "png");
    imageData = imageData.toBase64();
    return imageData;
}
QImage Method::getImage(QByteArray& data)
{
    QImage image;
    image.loadFromData(data);
    return image;
}

void Method::sendQFile(QTcpSocket* communicateSocket, QString filepath, qint64 cmd)
{
    QFile localFile(filepath);
    if(!localFile.open(QFile::ReadOnly))
    {
        return;
    }
    qDebug() << "open data file success";
    qint64 totalBytes = 0;
    QByteArray outBlock;
    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_5_2);
    totalBytes = localFile.size();  // 文件大小
    sendOut << qint64(totalBytes) << qint64(QFile_send);
    totalBytes += outBlock.size();
    sendOut.device()->seek(0);
    sendOut << totalBytes;
    communicateSocket->write(outBlock);
    outBlock.resize(0);
    outBlock = localFile.readAll();
    communicateSocket->write(outBlock);
    outBlock.resize(0);
    localFile.close();
}

void Method::sendStructData(QTcpSocket* communicateSocket, struct stu_stateData* stateData, qint64 cmd)
{
    if(!communicateSocket->isValid())
    {
        return;
    }
    qint64 totalBytes = 2 * sizeof(qint64) + sizeof(struct stu_stateData); //整个数据
    QByteArray outBlock;
    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_5_2);
    outBlock.resize(totalBytes);
    //向缓冲区写入文件头
    sendOut << totalBytes << qint64(cmd); // Struct_send是命令
    //向缓冲区写入文件数据
    mutex.lock();
    // 指针偏移2*sizeof(qint64),再拷贝
    memcpy(outBlock.data() + 2 * sizeof(qint64), &stateData, sizeof(struct stu_stateData));
    mutex.unlock();
    communicateSocket->write(outBlock);
    outBlock.resize(0);
}


void Method::readMsg(QTcpSocket* communicateSocket, QString* resStr, QPixmap* resImage,
                     QString filePath, QString filename, struct stu_stateData* resStructData)
{
    //如果不存在数据，就直接结束
    if(communicateSocket->bytesAvailable() <= 0)
    {
        qDebug() << "不存在数据";
        return;
    }
    //从缓存区中去除数据，但是不确定取出来的字节数
    QByteArray  buffer;
    buffer = communicateSocket->readAll();
    m_buffer.append(buffer);
    unsigned int totalLen = m_buffer.size();  // 缓存中的内容长度
    //这边确实需要利用长度做while循环，因为有可能一下子读取到两条以上的完整记录，就需要进行循环处理了；
    //超过一条完整小于第二条完整记录时，如果已经达到包头长度就先把包头保存下来，整个过程循环往复
    while(totalLen)
    {
        QDataStream packet(m_buffer);    //与QDataStream绑定，方便操作
        packet.setVersion(QDataStream::Qt_5_2);
        //不够包头长度的不处理，结束while循环
        if(totalLen < MINSIZE)  // MINSIZE = [数据包长度, 命令的类型]大小
        {
            break;
        }
        //读出 数据包长度, 命令的类型
        packet >> totalBytes >> Cmd;
        qDebug() << "数据包长度:" << totalBytes << " 命令的类型:" << Cmd;
        //缓存中的内容长度没有达到命令的长度，先结束，等足够了再来解析
        if(totalLen < totalBytes)
        {
            break;
        }
        //足够长了就开始解析
        switch(Cmd)
        {
            case QString_send :    //接收QString
                {
                    qDebug() << "开始接收字符串...";
                    QByteArray datas = m_buffer.mid(MINSIZE, totalBytes - MINSIZE); //读出的数据
                    resStr->append(datas);
                    break;
                }
            case Qimage_send:
                {
                    qDebug() << "开始接收图片...";
                    QByteArray datas = m_buffer.mid(MINSIZE, totalBytes - MINSIZE); //读出的数据
                    QImage imageData = getImage(datas);
                    *resImage = QPixmap::fromImage(imageData);
                    break;
                }
            case Qimages_send:
                {
                    qDebug() << "开始接收一组图片...";
                    QByteArray datas = m_buffer.mid(MINSIZE, totalBytes - MINSIZE); //读出的数据
                    resImage->loadFromData(datas);
                    break;
                }
            case QFile_send :    //接收文件
                {
                    QDir dir(filePath);      //系统文件目录
                    if(!dir.exists())
                    {
                        dir.mkdir(filePath);
                    }
                    qDebug() << "收到文件";
                    qDebug() << "文件大小为：" << totalBytes;
                    QString tmpfileName = QString("%1/%2").arg(filePath).arg(filename);
                    localFile = new QFile(tmpfileName);//真正的文件路径
                    if(!localFile->open(QIODevice::WriteOnly)) //写的方式打开该文件
                    {
                        qDebug() << QString("无法打开文件%1:\n%2.").arg(filename).arg(localFile->errorString());
                        return;
                    }
                    localFile->resize(0);//清空文件
                    QByteArray datas = m_buffer.mid(MINSIZE, totalBytes - MINSIZE); //读出的数据
                    localFile->write(datas);
                    localFile->close();
                    break;
                }
            case Struct_send: //接收结构体
                {
                    //motionData是一个stu_stateData结构体变量
                    QByteArray realStateData = m_buffer.mid(MINSIZE, totalBytes - MINSIZE);
                    if (!realStateData.isEmpty())
                    {
                        realStateData.resize(sizeof(struct stu_stateData));
                        char* buf = realStateData.data();
                        //motionMutex.lock();
                        memcpy(resStructData, buf, sizeof(struct stu_stateData));
                        //motionMutex.unlock();
                        //之后操作headMsg就行
                    }
                    break;
                }
            // 按要求追加命令
            case CLIENT_START_MONITOR:
                {
                    qDebug() << "开启监控...";
                    QByteArray datas = m_buffer.mid(MINSIZE, totalBytes - MINSIZE); //读出的数据
                    resStr->append(datas);
                    break;
                }
            case MOUSE_LOCK:
                {
                    // 鼠标悬停
                    //qDebug() << "鼠标悬停";
                    QByteArray datas = m_buffer.mid(MINSIZE, totalBytes - MINSIZE); //读出的数据
                    resStr->append(datas);
                }
            case MOUSE_UNLOCK:
                {
                    // 鼠标解锁
                    QByteArray datas = m_buffer.mid(MINSIZE, totalBytes - MINSIZE); //读出的数据
                    resStr->append(datas);
                }
            case CLIENT_LOGOFF:
                {
                    // 鼠标解锁
                    QByteArray datas = m_buffer.mid(MINSIZE, totalBytes - MINSIZE); //读出的数据
                    resStr->append(datas);
                }
            case CLIENT_REBOOT:
                {
                    // 鼠标解锁
                    QByteArray datas = m_buffer.mid(MINSIZE, totalBytes - MINSIZE); //读出的数据
                    resStr->append(datas);
                }
            case CLIENT_SHUTDOWN:
                {
                    // 鼠标解锁
                    QByteArray datas = m_buffer.mid(MINSIZE, totalBytes - MINSIZE); //读出的数据
                    resStr->append(datas);
                }
            case CLIENT_AUTORUN:
                {
                    // 开启自启
                    QByteArray datas = m_buffer.mid(MINSIZE, totalBytes - MINSIZE); //读出的数据
                    resStr->append(datas);
                }
            case CLIENT_UNAUTORUN:
                {
                    // 取消开启自启
                    QByteArray datas = m_buffer.mid(MINSIZE, totalBytes - MINSIZE); //读出的数据
                    resStr->append(datas);
                }
        }
        //缓存多余的数据
        buffer = m_buffer.right(totalLen - totalBytes); //截取下一个数据包的数据，留作下次读取
        totalLen = buffer.size();
        //更新多余的数据
        m_buffer = buffer;
    }
}


