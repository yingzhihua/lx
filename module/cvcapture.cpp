#include "cvcapture.h"
#include <QDebug>
#include <QDateTime>
#include <QCoreApplication>

cvCapture::cvCapture(QObject *parent) : QThread(parent)
{
    cameraid = 0;
    imageCount = 1;
}

void cvCapture::run(){
    QByteArray res;

    qDebug()<<"start cvCapture:"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz")<<",cameraid="<<cameraid<<"isOpened:"<<cap.isOpened();
    if (!cap.isOpened())
        cap.open(cameraid);

    if (!cap.isOpened())
    {
        qDebug()<<"open cam fail";
    }

    cap.set(CAP_PROP_FRAME_WIDTH,1920);
    cap.set(CAP_PROP_FRAME_HEIGHT,1080);

    //cap.set(CAP_PROP_FRAME_WIDTH,2592);
    //cap.set(CAP_PROP_FRAME_HEIGHT,1944);
    //cap.set(CAP_PROP_FORMAT,CV_8UC1);
    qDebug()<<"width:"<<cap.get(CAP_PROP_FRAME_WIDTH)<<" height:"<<cap.get(CAP_PROP_FRAME_HEIGHT)<<" Frame:"<<cap.get(CAP_PROP_FPS)<<" auto_expoure:"<<cap.get(CAP_PROP_AUTO_EXPOSURE)<<" exp:"<<cap.get(CAP_PROP_EXPOSURE);

    for(int i = 1; i <= imageCount; i++){
        Mat Frame;
        qDebug()<<"read Frame "<<i<<",begin:"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        cap>>Frame;
        if (Frame.empty())
        {
            qDebug()<<"read Frame fail";
            break;
        }
        qDebug()<<"get frame time:"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz")<<",cols="<<Frame.cols<<",rows="<<Frame.rows<<"type:"<<Frame.type();
        QImage(Frame.data,Frame.cols,Frame.rows,QImage::Format_RGB888).rgbSwapped().save(QCoreApplication::applicationDirPath()+"/test"+QString::number(i)+".bmp");
    }
    //cap.release();
    qDebug()<<"end cvCapture:"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
}

bool cvCapture::Running(){
    int count = 0;
    while(this->isRunning() && count<30){
        qDebug()<<"Running:"<<count;
        count++;
        QThread::msleep(100);
    }
    if (count<100)
        return false;
    return true;
}
