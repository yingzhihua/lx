#include "serialmgr.h"
#include <QDebug>

static QSerialPort serialTemp;
static QSerialPort serialCtrl;

static QMutex mutex;
static QByteArrayList sendList;
static qint64 bytesWritten = 0;

static bool sending = false;
static int sendCount = 0;

static QString portTemp = "ttyUSB0";
static QString portCtrl = "ttyS4";
static QByteArray byteReadData;
static QByteArray byteCtrlReadData;
static QTimer writeTimer;
static int serialInitCount = 0;

SerialMgr::SerialMgr(QObject *parent) : QObject(parent)
{
    connect(&serialTemp,&QSerialPort::readyRead,this,&SerialMgr::handleReadTemp);
    connect(&serialCtrl,&QSerialPort::readyRead,this,&SerialMgr::handleReadCtrl);
    connect(&serialTemp,&QSerialPort::errorOccurred,this,&SerialMgr::handleErrorTemp);
    connect(&serialCtrl,&QSerialPort::errorOccurred,this,&SerialMgr::handleErrorCtrl);
    connect(&serialTemp,&QSerialPort::bytesWritten,this,&SerialMgr::handleBytesWritten);
    connect(&serialCtrl,&QSerialPort::bytesWritten,this,&SerialMgr::handleBytesWritten);
    connect(&writeTimer,&QTimer::timeout,this,&SerialMgr::handleWriteTimeout);    
    serialInitCount++;
    qDebug()<<"SerialMgr Init,count="<<serialInitCount;
}

int SerialMgr::SerialInit()
{
    serialTemp.setPortName(portTemp);
    serialTemp.setBaudRate(QSerialPort::Baud115200);
    serialTemp.setDataBits(QSerialPort::Data8);
    serialTemp.setParity(QSerialPort::NoParity);
    serialTemp.setStopBits(QSerialPort::OneStop);
    serialTemp.setFlowControl(QSerialPort::NoFlowControl);

    serialCtrl.setPortName(portCtrl);
    serialCtrl.setBaudRate(QSerialPort::Baud115200);
    serialCtrl.setDataBits(QSerialPort::Data8);
    serialCtrl.setParity(QSerialPort::NoParity);
    serialCtrl.setStopBits(QSerialPort::OneStop);
    serialCtrl.setFlowControl(QSerialPort::NoFlowControl);

    writeTimer.setSingleShot(true);

    if (!serialTemp.open(QIODevice::ReadWrite))
    {
        qDebug()<<"serialTemp open failed";
        return -1;
    }
    else {
        qDebug()<<"serialTemp open success";
    }

    if (!serialCtrl.open(QIODevice::ReadWrite))
    {
        qDebug()<<"serialCtrl open failed";
        return -2;
    }
    else {
        qDebug()<<"serialCtrl open success";
    }
    return 0;
}

void SerialMgr::handleReadTemp()
{
    byteReadData.append(serialTemp.readAll());
    if (byteReadData.length() == 0)
        return;
    //qDebug()<<"Temp Read length:"<<byteReadData.length()<<","<<byteReadData.toHex(' ');
    if ((byteReadData[0]!='\xaa'&&byteReadData[0]!='\x99')||byteReadData.length()>1000)
    {
        byteReadData.clear();
        return;
    }

    if (byteReadData[byteReadData.length()-1]=='\x55')
    {
        int pos = 0;
        bool sendNext = false;
        while(pos+11<byteReadData.length()){
            if (byteReadData[pos] == '\xaa'){
                qDebug()<<"Temp Read:"<<byteReadData.toHex(' ');
                Log::LogWithTime("Temp Read: "+byteReadData.toHex(' '));
                clearTime(byteReadData.mid(pos,byteReadData[pos+2]));
                if (byteReadData[pos+10] == '\x33')
                {
                    ackReceive(byteReadData.mid(pos,byteReadData[pos+2]));
                    sendNext = true;
                }
                //*
                else if(byteReadData[pos+10] == '\x55') {
                    errReceive(byteReadData.mid(pos,byteReadData[pos+2]));
                    byteReadData.clear();
                    return;
                }
                //*/

                if (byteReadData[pos+2] == '\x0')
                    break;
                pos += byteReadData[pos+2];
            }
            else if(byteReadData[pos] == '\x99'){
                int len = byteReadData[pos+1];
                len += byteReadData[pos+2]<<8;
                if (len == 0)
                    break;
                Log::LogTemp(byteReadData.mid(pos,len));
                pos += len;
            }
            else
                break;
        }

        byteReadData.clear();
        if (sendNext)
            serialSend(true);
    }
}

void SerialMgr::handleReadCtrl()
{
    byteCtrlReadData.append(serialCtrl.readAll());
    if (byteCtrlReadData.length() == 0)
        return;
    if (byteCtrlReadData[0]!='\xaa'||byteCtrlReadData.length()>1000)
    {
        byteCtrlReadData.clear();
        return;
    }
    qDebug()<<"Ctrl Read:"<<byteCtrlReadData.toHex(' ');
    Log::LogWithTime("Ctrl Read: "+byteCtrlReadData.toHex(' '));
    if (byteCtrlReadData[byteCtrlReadData.length()-1]=='\x55')
    {
        int pos = 0;
        bool sendNext = false;
        while(pos+11<byteCtrlReadData.length()){
            clearTime(byteCtrlReadData.mid(pos,byteCtrlReadData[pos+2]));
            if (byteCtrlReadData[pos+10] == '\x33'){
                ackReceive(byteCtrlReadData.mid(pos,byteCtrlReadData[pos+2]));
                sendNext = true;
            }
            else if(byteCtrlReadData[pos+10] == '\x55') {
                errReceive(byteCtrlReadData.mid(pos,byteCtrlReadData[pos+2]));
                byteCtrlReadData.clear();
                return;
            }
            pos += byteCtrlReadData[pos+2];
        }
        byteCtrlReadData.clear();
        if (sendNext)
            serialSend(true);
    }
}

void SerialMgr::serialSend(bool clearCount)
{
    qDebug()<<"serialSend,sending"<<sending<<",ListCount:"<<sendList.count();
    if (sending == true)
        return;

    if (sendList.count() > 0)
    {
        QByteArray data = sendList.first();
        if (data[1] == '\1')
        {
            qDebug()<<"Ctrl Write:"<<data.toHex(' ');
            Log::LogWithTime("Ctrl Write: "+data.toHex(' '));
            serialCtrl.write(data);
        }
        else
        {
            qDebug()<<"Temp Write:"<<data.toHex(' ');
            Log::LogWithTime("Temp Write: "+data.toHex(' '));
            serialTemp.write(data);
        }
        bytesWritten = 0;

        writeTimer.start(2000);
        sending = true;
        if (clearCount)
            sendCount = 0;
    }
}

void SerialMgr::clearTime(QByteArray wData)
{
    if (sending && sendList.count()>0)
    {
        QByteArray data = sendList.first();
        if (wData[1] == data[1] && wData[7] == data[7])
            writeTimer.start(120000);
    }
}

bool SerialMgr::ackReceive(QByteArray wData)
{
    qDebug()<<"ackReceive start";
    mutex.lock();
    if (sending && sendList.count()>0)
    {
        QByteArray data = sendList.first();
        if (wData[1] == data[1] && wData[7] == data[7])
        {
            sendList.removeFirst();
            writeTimer.stop();
            sending = false;

            if (data[7] == '\x24'){
                QByteArray send;
                send.resize(11);
                send[0] = 0xAA;
                send[1] = 0x02;
                send[2] = 0x0B;
                send[7] = 0x76;
                send[8] = 0;
                send[9] = 0;
                send[10] = 0x55;
                serialTemp.write(send);
            }
        }
    }
    mutex.unlock();
    emit finishAction(wData);
    qDebug()<<"ackReceive end";
}

void SerialMgr::errReceive(QByteArray wData)
{
    qDebug()<<"errReceive start";
    mutex.lock();
    if (sending && sendList.count()>0)
    {
        QByteArray data = sendList.first();
        if (wData[1] == data[1] && wData[7] == data[7])
        {
            sendList.removeFirst();
            writeTimer.stop();
            sending = false;
        }
    }
    mutex.unlock();
    emit errAction(wData);
    qDebug()<<"errReceive end";
}

void SerialMgr::handleErrorTemp(QSerialPort::SerialPortError error)
{
    qDebug()<<serialTemp.portName()<<",error:"<<serialTemp.errorString()<<",errorcode:"<<error;
}
void SerialMgr::handleErrorCtrl(QSerialPort::SerialPortError error)
{
    qDebug()<<serialCtrl.portName()<<",error:"<<serialCtrl.errorString()<<",errorcode:"<<error;
}

void SerialMgr::handleWriteTimeout(){
    sendCount++;
    qDebug()<<"handleWriteTimeout:"<<sendCount;
    if (sending && sendCount < 3)
    {
        mutex.lock();
        sending = false;
        serialSend(false);
        mutex.unlock();
    }
    else
    {
        if (sendList.count() > 0)
        {
            QByteArray data = sendList.first();
            char devicetype = data[1];
            sendList.clear();
            if (devicetype == '\1')
                emit errOccur(ERROR_CODE_CTRL_CONNECT);
            else
                emit errOccur(ERROR_CODE_TEMP_CONNECT);
        }
    }
}

void SerialMgr::handleBytesWritten(qint64 bytes){
    bytesWritten += bytes;
    qDebug()<<"handleBytesWritten:"<<bytesWritten;
}

bool SerialMgr::serialWrite(QByteArray wData)
{
    qDebug()<<"serialWrite start";
    mutex.lock();
    sendList.append(wData);
    serialSend(true);
    mutex.unlock();
    qDebug()<<"serialWrite end";
    return true;
}

void SerialMgr::CtrlDirectWrite(QByteArray wData){
    serialCtrl.write(wData);
}
