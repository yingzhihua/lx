#include "qrcoder.h"
#include "opencv2/videoio.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <QDebug>
#include <QDateTime>
#include <QCoreApplication>

using namespace cv;
using namespace std;
QRcoder::QRcoder(QObject *parent) : QObject(parent)
{

}

QString QRcoder::QRDecode(){
    int count = 0;
    QString result;
#if 1
    VideoCapture cap;
    qDebug()<<"QRDecode,"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    cap.open(1);
    qDebug()<<"open endtime="<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    if (!cap.isOpened())
    {
        qDebug()<<"open cam 1 fail";
        return result;
        //cap.open(1);
    }

    QRCodeDetector qrcode;
    //TickMeter total;

    for (;;) {
        Mat Frame,src,straight_barcode;
        vector<Point> transform;
        string decode_info;
        cap>>Frame;
        if (Frame.empty())
            break;
        qDebug()<<"raad Frame "<<count<<",time:"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        cvtColor(Frame,src,COLOR_BGR2GRAY);
#if 0
        bool result_detection = qrcode.detect(src,transform);
        if (result_detection)
        {
            decode_info = qrcode.decode(src,transform,straight_barcode);
            if (!decode_info.empty())
                qDebug()<<"code:"<<QString::fromStdString(decode_info);
        }
#endif
#if 1
        if (count == 1)
        {
            char filename[50];
            sprintf(filename,"%s/temp.jpg",QCoreApplication::applicationDirPath().toLatin1().data());
            imwrite(filename,Frame);
        }
#endif
        if (++count > 10)
        break;
    }
    cap.release();
#endif
    return result;
}
