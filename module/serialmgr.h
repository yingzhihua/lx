#ifndef SERIALMGR_H
#define SERIALMGR_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>
#include <QMutex>

class SerialMgr : public QObject
{
    Q_OBJECT

public:
    explicit SerialMgr(QObject *parent = nullptr);
    static int SerialInit();
    static bool serialWrite(QByteArray wData);

signals:
    void finishAction(QByteArray result);
    void errOccur(int errCode);

private slots:
    void handleReadTemp();
    void handleReadCtrl();
    void handleBytesWritten(qint64 bytes);
    void handleErrorTemp(QSerialPort::SerialPortError error);
    void handleErrorCtrl(QSerialPort::SerialPortError error);
    void handleWriteTimeout();

private:    
    void ackReceive(QByteArray wData);
    void clearTime(QByteArray wData);
    static void serialSend(bool clearCount);
};

#endif // SERIALMGR_H
