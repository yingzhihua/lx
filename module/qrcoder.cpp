#include "qrcoder.h"
#include "QZXing.h"
#include "log.h"
#include "exglobal.h"

#include "zbar.h"

#include <iostream>
#include <QDebug>
#include <QDateTime>
#include <QCoreApplication>

QImage QRcoder::img;
QImage QRcoder::img2;

const int scaleX = 520;
const int scaleY = 180;
const int scalewide = 500;

//#define USE_QZXING
#define USE_ZBAR

QRcoder::QRcoder(QObject *parent) : QThread(parent)
{
    handleimage = true;
    scale = true;
}

void QRcoder::run(){
    int count = 0;
    QString result;
    VideoCapture cap;

    QByteArray res;
    res.resize(10);

    res[0] = '\xaa';
    res[1] = '\x04';
    res[2] = '\x00';
    if (mode == DECODE_MODE_PIERCE)
        res[7] = '\xB1';
    else
        res[7] = '\xB0';

    qDebug()<<"start time:"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    Log::LogCam("QRcoder start:"+QDateTime::currentDateTime().toString("hh:mm:ss.zzz"));

    cap.open(1);
    if (!cap.isOpened())
    {
        qDebug()<<"open cam 1 fail";
        Log::LogCam("open cam 1 fail");
        res.append("Cannot identify");
        emit finishQRcode(res);
        return;
    }
    cap.set(CAP_PROP_FRAME_WIDTH,1920);
    cap.set(CAP_PROP_FRAME_HEIGHT,1080);
    cap.set(CAP_PROP_AUTO_EXPOSURE,1);
    cap.set(CAP_PROP_EXPOSURE,poxValue);

    qDebug()<<"width:"<<cap.get(CAP_PROP_FRAME_WIDTH)<<" height:"<<cap.get(CAP_PROP_FRAME_HEIGHT)<<" Frame:"<<cap.get(CAP_PROP_FPS)<<" auto_expoure:"<<cap.get(CAP_PROP_AUTO_EXPOSURE)<<" exp:"<<cap.get(CAP_PROP_EXPOSURE);


    for (;;) {
        Mat Frame;QString result;
        cap>>Frame;
        if (Frame.empty())
            break;

        qDebug()<<"read Frame "<<count<<",time:"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz")<<",cols="<<Frame.cols<<",rows="<<Frame.rows<<"scale:"<<scale;
        Log::LogCam(QString("read Frame %1,time:%2,cols=%3,rows=%4,type=%5").arg(count).arg(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")).arg(Frame.cols).arg(Frame.rows).arg(Frame.type()));

        Mat sourceFrame = Mat(Frame.rows, Frame.cols, Frame.type(), ExGlobal::bufrgb);
        cvtColor(Frame, sourceFrame, COLOR_BGR2RGB);
        img = QImage((const uchar*)sourceFrame.data,sourceFrame.cols,sourceFrame.rows,QImage::Format_RGB888);

        Mat handleFrame = Mat(Frame.rows,Frame.cols,Frame.type(),ExGlobal::hbufrgb);
        cvtColor(sourceFrame, handleFrame, COLOR_RGB2GRAY);

        if (mode == DECODE_MODE_PIERCE)
            result = pierce(handleFrame);
        else if (mode == DECODE_MODE_QR)
            result = QrDecode(handleFrame);

        if (!result.isEmpty()||++count > 10)
        {            
            break;
        }
    }
    cap.release();

    qDebug()<<"end time:"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    Log::LogCam("QRcoder end:"+QDateTime::currentDateTime().toString("hh:mm:ss.zzz"));

    if (result.isEmpty())
    {
        res.append("Cannot identify");
    }
    else
    {
        res[2] = '\x01';
        res.append(result);
    }
    emit finishQRcode(res);
}

void QRcoder::handleImage(Mat &image){
    #if 0
    Mat srcGray;
    GaussianBlur(image,srcGray,Size(3,3),0,0,BORDER_DEFAULT);
    int edgeThresh = 80;
    Canny(srcGray,image,edgeThresh,edgeThresh*2);
    #endif
    //threshold(image,image,70,255,THRESH_BINARY);
    vector<Vec3f> circles;
    //HoughCircles(image,circles,HOUGH_GRADIENT,1.3,500,230,40,50,100);
    HoughCircles(image,circles,HOUGH_GRADIENT,1.3,500,230,100);
    qDebug()<<"circles num = "<<circles.size();
    for (size_t i = 0; i < circles.size(); i++){
        Point center(cvRound(circles[i][0]),cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        circle(image, center,radius-10,Scalar(0),3);
        qDebug()<<"x="<<center.x<<",y="<<center.y<<",r="<<radius;
    }
}

QString QRcoder::QrDecode(Mat &image){
    QString result;

    QRCodeDetector qrcode;
    vector<Point> transform;
#if defined(USE_QZXING)
    QZXing decoder;
    decoder.setDecoder(QZXing::DecoderFormat_QR_CODE);
#elif defined(USE_ZBAR)
    zbar::ImageScanner scanner;
    scanner.set_config(zbar::ZBAR_NONE,zbar::ZBAR_CFG_ENABLE,1);
#else
    //QRCodeDetector qrcode;
#endif
    Mat sourceFrame = Mat(image.rows, image.cols, image.type(), ExGlobal::bufrgb);
    image.copyTo(sourceFrame);
    Mat handleFrame;
    vector<Point2f> spos;
    spos.push_back(Point2f(ExGlobal::QrX1,ExGlobal::QrY1));
    spos.push_back(Point2f(ExGlobal::QrX2,ExGlobal::QrY2));
    spos.push_back(Point2f(ExGlobal::QrX3,ExGlobal::QrY3));
    spos.push_back(Point2f(ExGlobal::QrX4,ExGlobal::QrY4));

    if (scale){
        handleFrame = Mat(scalewide,scalewide,image.type(),ExGlobal::hbufrgb);
        vector<Point2f> obj;
        obj.push_back(Point2f(0,0));
        obj.push_back(Point2f(scalewide,0));
        obj.push_back(Point2f(scalewide,scalewide));
        obj.push_back(Point2f(0,scalewide));
        Mat H = findHomography(spos,obj,RANSAC);
        warpPerspective(sourceFrame,handleFrame,H,handleFrame.size());
    }else{
        handleFrame = Mat(image.rows,image.cols,image.type(),ExGlobal::hbufrgb);
        image.copyTo(handleFrame);
    }

    cvtColor(sourceFrame, sourceFrame, COLOR_BGR2RGB);
    cvtColor(handleFrame, handleFrame, COLOR_BGR2GRAY);

    bool result_detection = qrcode.detect(handleFrame,transform);
    qDebug()<<"handleFrame.type="<<handleFrame.type()<<",Frame.type="<<image.type()<<",result_detection="<<result_detection;
    //threshold(subFrame2,subFrame2,binValue,255,THRESH_BINARY);

#if defined(USE_QZXING)
        if (handleimage)
            result = decoder.decodeImage(img2);
        else
            result = decoder.decodeImage(img);
#elif defined(USE_ZBAR)
        zbar::Image imageZbar(handleFrame.cols,handleFrame.rows,"Y800",handleFrame.data,handleFrame.cols*handleFrame.rows);
        scanner.scan(imageZbar);
        zbar::Image::SymbolIterator symbol = imageZbar.symbol_begin();
        if (imageZbar.symbol_begin()==imageZbar.symbol_end()){
            qDebug()<<"zbar scan fail";
        }
        else {
            result = QString::fromStdString(imageZbar.symbol_begin()->get_data());
        }

        /*
        for(;symbol != imageZbar.symbol_end(); ++symbol){
            qDebug()<<"zbar type:"<<QString::fromStdString(symbol->get_type_name())<<endl<<"code:"<<QString::fromStdString(symbol->get_data());
        }
        */
#else
        vector<Point> transform;
        Mat straight_barcode;
        if(qrcode.detect(subFrame2,transform)){
            string decode_info = qrcode.decode(subFrame2,transform,straight_barcode);
            if (!decode_info.empty()){
                result = QString::fromStdString(decode_info);
            }
        }

#endif
#if 1
        circle(sourceFrame,spos[0],10,Scalar(0,0,255),3);
        circle(sourceFrame,spos[1],10,Scalar(0,0,255),3);
        circle(sourceFrame,spos[2],10,Scalar(0,0,255),3);
        circle(sourceFrame,spos[3],10,Scalar(0,0,255),3);
        line(sourceFrame,spos[0],spos[1],Scalar(0,255,0));
        line(sourceFrame,spos[1],spos[2],Scalar(0,255,0));
        line(sourceFrame,spos[2],spos[3],Scalar(0,255,0));
        line(sourceFrame,spos[3],spos[0],Scalar(0,255,0));
#endif

        handleImage(handleFrame);

        img2 = QImage((const uchar*)handleFrame.data,handleFrame.cols,handleFrame.rows,QImage::Format_Grayscale8);

        qDebug()<<"decode_info:"<<result;
        Log::LogCam("decode_info:"+result);
        return result;
}


QString QRcoder::pierce(Mat &image){
    QString result;
    result = "lengxing";

    vector<Vec3f> circles;
    HoughCircles(image,circles,HOUGH_GRADIENT,1.3,500,230,100);
    qDebug()<<"circles num = "<<circles.size();
    for (size_t i = 0; i < circles.size(); i++){
        Point center(cvRound(circles[i][0]),cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        circle(image, center,radius-10,Scalar(0),3);
        qDebug()<<"x="<<center.x<<",y="<<center.y<<",r="<<radius;
    }

    return result;
}

void QRcoder::Qr(){
    mode = DECODE_MODE_QR;
    while(this->isRunning());
    this->start();
}

void QRcoder::Pierce(){
    mode = DECODE_MODE_PIERCE;
    while(this->isRunning());
    this->start();
}
