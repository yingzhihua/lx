#include "qrcoder.h"
#include "QZXing.h"

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
    TickMeter total;
    qDebug()<<"start time:"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    total.start();
    cap.open(1);
    if (!cap.isOpened())
    {
        qDebug()<<"open cam 1 fail";
        return;
    }
    cap.set(CAP_PROP_FRAME_WIDTH,1920);
    cap.set(CAP_PROP_FRAME_HEIGHT,1080);
    //cap.set(CAP_PROP_FRAME_WIDTH,2592);
    //cap.set(CAP_PROP_FRAME_HEIGHT,1944);
    cap.set(CAP_PROP_AUTO_EXPOSURE,1);
    cap.set(CAP_PROP_EXPOSURE,poxValue);
    qDebug()<<"width:"<<cap.get(CAP_PROP_FRAME_WIDTH)<<" height:"<<cap.get(CAP_PROP_FRAME_HEIGHT)<<" Frame:"<<cap.get(CAP_PROP_FPS)<<" auto_expoure:"<<cap.get(CAP_PROP_AUTO_EXPOSURE)<<" exp:"<<cap.get(CAP_PROP_EXPOSURE);

    QZXing decoder;
    decoder.setDecoder(QZXing::DecoderFormat_QR_CODE);

    for (;;) {
        Mat Frame;
        cap>>Frame;
        if (Frame.empty())
            break;
        qDebug()<<"read Frame "<<count<<",time:"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz")<<",cols="<<Frame.cols<<",rows="<<Frame.rows<<"scale:"<<scale;

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
        //GaussianBlur(subFrame2, subFrame2, Size(11,11),0);
        //normalize(subFrame2,subFrame2,0,255,NORM_MINMAX);
        threshold(subFrame2,subFrame2,binValue,255,THRESH_BINARY);
        //subFrame = subFrame*2;
        img = QImage((const uchar*)subFrame.data,subFrame.cols,subFrame.rows,QImage::Format_RGB888);
        img2 = QImage((const uchar*)subFrame2.data,subFrame2.cols,subFrame2.rows,QImage::Format_Grayscale8);
        if (handleimage)
            result = decoder.decodeImage(img2);
        else
            result = decoder.decodeImage(img);
        qDebug()<<"decode_info:"<<result;

        if (!result.isEmpty()||++count > 10)
        {
            img.save(QCoreApplication::applicationDirPath()+"/temp.bmp");
            break;
        }
    }
    cap.release();
    total.stop();
    qDebug()<<"QRDecode:time="<<total.getTimeMilli()<<",count="<<count;
    qDebug()<<"end time:"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");


    QByteArray res;
    res.resize(10);
    res[0] = '\xaa';
    res[1] = '\x04';
    res[7] = '\xB0';
    if (result.isEmpty())
        res.append("Cannot identify");
    else
        res.append(result);
    emit finishQRcode(res);
}
