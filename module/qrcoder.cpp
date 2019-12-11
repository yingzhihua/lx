#include "qrcoder.h"
#include "QZXing.h"
#include "log.h"

#include <iostream>
#include <QDebug>
#include <QDateTime>
#include <QCoreApplication>

QImage QRcoder::img;
QImage QRcoder::img2;
QRcoder::QRcoder(QObject *parent) : QThread(parent)
{
    handleimage = true;
    scale = true;
}

void QRcoder::run(){
    int count = 0;
    QString result;
    VideoCapture cap;

    qDebug()<<"start time:"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    Log::LogCam("QRcoder start:"+QDateTime::currentDateTime().toString("hh:mm:ss.zzz"));

    cap.open(1);
    if (!cap.isOpened())
    {
        qDebug()<<"open cam 1 fail";
        Log::LogCam("open cam 1 fail");
        QByteArray res;
        res.resize(10);
        res[0] = '\xaa';
        res[1] = '\x04';
        res[2] = '\x00';
        res[7] = '\xB0';
        res.append("Cannot identify");
        emit finishQRcode(res);
        return;
    }
    cap.set(CAP_PROP_FRAME_WIDTH,1920);
    cap.set(CAP_PROP_FRAME_HEIGHT,1080);
    //cap.set(CAP_PROP_FRAME_WIDTH,2592);
    //cap.set(CAP_PROP_FRAME_HEIGHT,1944);
    cap.set(CAP_PROP_AUTO_EXPOSURE,1);
    cap.set(CAP_PROP_EXPOSURE,poxValue);
    //qDebug()<<"width:"<<cap.get(CAP_PROP_FRAME_WIDTH)<<" height:"<<cap.get(CAP_PROP_FRAME_HEIGHT)<<" Frame:"<<cap.get(CAP_PROP_FPS)<<" auto_expoure:"<<cap.get(CAP_PROP_AUTO_EXPOSURE)<<" exp:"<<cap.get(CAP_PROP_EXPOSURE);

    QZXing decoder;
    decoder.setDecoder(QZXing::DecoderFormat_QR_CODE);

    for (;;) {
        Mat Frame;
        cap>>Frame;
        if (Frame.empty())
            break;
        qDebug()<<"read Frame "<<count<<",time:"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz")<<",cols="<<Frame.cols<<",rows="<<Frame.rows<<"scale:"<<scale;
        Log::LogCam(QString("read Frame %1,time:%2,cols=%3,rows=%4").arg(count).arg(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")).arg(Frame.cols).arg(Frame.rows));

        Mat subFrame,subFrame2;
        if (scale){
        subFrame = Mat(440,440,Frame.type());
        subFrame2 = Mat(440,440,Frame.type());
        Frame(Rect(520,180,440,440)).copyTo(subFrame);
        Frame(Rect(520,180,440,440)).copyTo(subFrame2);
        qDebug()<<"scale scale";
        }else{
            Frame.copyTo(subFrame);
            Frame.copyTo(subFrame2);
            qDebug()<<"no scale";
        }

        cvtColor(subFrame, subFrame, COLOR_BGR2RGB);
        cvtColor(subFrame2, subFrame2, COLOR_BGR2GRAY);

        threshold(subFrame2,subFrame2,binValue,255,THRESH_BINARY);

        img = QImage((const uchar*)subFrame.data,subFrame.cols,subFrame.rows,QImage::Format_RGB888);
        img2 = QImage((const uchar*)subFrame2.data,subFrame2.cols,subFrame2.rows,QImage::Format_Grayscale8);
#if 0
        if (handleimage)
            result = decoder.decodeImage(img2);
        else
            result = decoder.decodeImage(img);
#endif
        qDebug()<<"decode_info:"<<result;
        Log::LogCam("decode_info:"+result);
        if (!result.isEmpty()||++count > 10)
        {
            //img.save(QCoreApplication::applicationDirPath()+"/temp.bmp");
            break;
        }
    }
    cap.release();

    qDebug()<<"end time:"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    Log::LogCam("QRcoder end:"+QDateTime::currentDateTime().toString("hh:mm:ss.zzz"));
    QByteArray res;
    res.resize(10);
    res[0] = '\xaa';
    res[1] = '\x04';
    res[2] = '\x01';
    res[7] = '\xB0';
    if (result.isEmpty())
        res.append("Cannot identify");
    else
        res.append(result);
    emit finishQRcode(res);
}
