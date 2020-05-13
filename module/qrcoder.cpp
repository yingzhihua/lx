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
    int nresult = 0;
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
    //cap.set(CAP_PROP_EXPOSURE,2000);

    qDebug()<<"width:"<<cap.get(CAP_PROP_FRAME_WIDTH)<<" height:"<<cap.get(CAP_PROP_FRAME_HEIGHT)<<" Frame:"<<cap.get(CAP_PROP_FPS)<<" auto_expoure:"<<cap.get(CAP_PROP_AUTO_EXPOSURE)<<" exp:"<<cap.get(CAP_PROP_EXPOSURE);


    for (;;) {
        Mat Frame;
        cap>>Frame;
        if (Frame.empty())
            break;

        qDebug()<<"read Frame "<<count<<",time:"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz")<<",cols="<<Frame.cols<<",rows="<<Frame.rows<<"scale:"<<scale;
        Log::LogCam(QString("read Frame %1,time:%2,cols=%3,rows=%4,type=%5").arg(count).arg(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")).arg(Frame.cols).arg(Frame.rows).arg(Frame.type()));

        Mat sourceFrame = Mat(Frame.rows, Frame.cols, Frame.type(), ExGlobal::bufrgb);
        cvtColor(Frame, sourceFrame, COLOR_BGR2RGB);
        img = QImage((const uchar*)sourceFrame.data,sourceFrame.cols,sourceFrame.rows,QImage::Format_RGB888);

        //Mat handleFrame = Mat(Frame.rows,Frame.cols,Frame.type(),ExGlobal::hbufrgb);
        //cvtColor(sourceFrame, handleFrame, COLOR_RGB2GRAY);

        if (mode == DECODE_MODE_PIERCE)
            nresult = pierce(sourceFrame,result);
        else if (mode == DECODE_MODE_QR)            
            //result = QrDecode(sourceFrame);
        nresult = pierce(sourceFrame,result);

        if (!result.isEmpty()||++count > 3)
        {
            qDebug()<<"result="<<result<<",count="<<count;
            break;
        }
    }
    cap.release();

    qDebug()<<"end time:"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    Log::LogCam("QRcoder end:"+QDateTime::currentDateTime().toString("hh:mm:ss.zzz"));

    if (result.isEmpty())
    {
        res.append("Cannot identify");
        qDebug()<<"result is empty";
    }
    else
    {
        res[2] = '\x01';
        nresult |= 0x01;
        res.append(result);
        qDebug()<<"result="<<result;
    }

    res[2] = static_cast<char>(nresult);
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
    HoughCircles(image,circles,HOUGH_GRADIENT,1.3,500,230,40,50,100);
    //HoughCircles(image,circles,HOUGH_GRADIENT,1.3,500,230,100);
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
        warpPerspective(image,handleFrame,H,handleFrame.size());
    }else{
        handleFrame = Mat(image.rows,image.cols,image.type(),ExGlobal::hbufrgb);
        image.copyTo(handleFrame);
        //cvtColor(image, handleFrame, COLOR_RGB2GRAY);
    }

    cvtColor(handleFrame, handleFrame, COLOR_RGB2GRAY);
    //equalizeHist(handleFrame,handleFrame);
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
            }image
        }

#endif
#if 1
        circle(image,spos[0],10,Scalar(0,0,255),3);
        circle(image,spos[1],10,Scalar(0,0,255),3);
        circle(image,spos[2],10,Scalar(0,0,255),3);
        circle(image,spos[3],10,Scalar(0,0,255),3);
        line(image,spos[0],spos[1],Scalar(0,255,0));
        line(image,spos[1],spos[2],Scalar(0,255,0));
        line(image,spos[2],spos[3],Scalar(0,255,0));
        line(image,spos[3],spos[0],Scalar(0,255,0));
#endif

        //handleImage(handleFrame);

        img2 = QImage((const uchar*)handleFrame.data,handleFrame.cols,handleFrame.rows,QImage::Format_Grayscale8);

        qDebug()<<"decode_info:"<<result;
        Log::LogCam("decode_info:"+result);
        return result;
}


int QRcoder::pierce(Mat &image, QString &qrStr){
    int result = 0;
    //result = "lengxing";

    Mat handleFrame = Mat(image.rows,image.cols,image.type(),ExGlobal::hbufrgb);
    cvtColor(image, handleFrame, COLOR_RGB2GRAY);
    qDebug()<<"pierce start:"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
#if defined(USE_ZBAR)
    zbar::ImageScanner scanner;
    scanner.set_config(zbar::ZBAR_NONE,zbar::ZBAR_CFG_ENABLE,1);

    zbar::Image imageZbar(handleFrame.cols,handleFrame.rows,"Y800",handleFrame.data,handleFrame.cols*handleFrame.rows);
    scanner.scan(imageZbar);
    zbar::Image::SymbolIterator symbol = imageZbar.symbol_begin();
    if (imageZbar.symbol_begin()==imageZbar.symbol_end()){
        qDebug()<<"zbar scan fail";
    }
    else {
        qrStr = QString::fromStdString(imageZbar.symbol_begin()->get_data());
        result = 1;
        qDebug()<<"decode_info:"<<qrStr;
    }
    /*
    for(;symbol != imageZbar.symbol_end(); ++symbol){
        qDebug()<<"zbar type:"<<QString::fromStdString(symbol->get_type_name())<<endl<<"code:"<<QString::fromStdString(symbol->get_data());
    }
    */
#endif
    qDebug()<<"pierce zbar:"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    //imclose
    //*
    bitwise_not(handleFrame,handleFrame);
    Mat element = getStructuringElement(MORPH_ELLIPSE,Size(30,30));
    morphologyEx(handleFrame,handleFrame,MORPH_CLOSE,element);
    bitwise_not(handleFrame,handleFrame);
    //*/

    //adaptiveThreshold(handleFrame,handleFrame,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY,11,-2);
#if 0
    vector<vector<Point>> contours;
    vector<Vec4i> hi;
    findContours(handleFrame,contours,hi,RETR_LIST,CHAIN_APPROX_NONE);
    qDebug()<<"contorus.size="<<contours.size();
    for (size_t i = 0; i < contours.size(); i++)
        qDebug()<<"s="<<contourArea(contours[i])<<"L="<<arcLength(contours[i],true);
#endif

#if 0
    vector<Vec2f> lines;
    HoughLines(handleFrame,lines,1,CV_PI/150,250,0,0);
    qDebug()<<"lines num="<<lines.size();
    for (size_t i = 0; i < lines.size();i++){
        float rho = lines[i][0];
        float theta = lines[i][1];
        Point pt1,pt2;
        double a = cos(theta),b=sin(theta);
        double x0 = a*rho,y0=b*rho;
        pt1.x = cvRound(x0+1000*(-b));
        pt1.y = cvRound(y0+1000*a);
        pt2.x = cvRound(x0-1000*(-b));
        pt2.y = cvRound(y0-1000*a);
        line(image,pt1,pt2,Scalar(0),3);
    }
#endif

#if 1
    vector<Vec3f> circles;
    double meanValue = 0.0;
    HoughCircles(handleFrame,circles,HOUGH_GRADIENT,1.5,50,100,50,50,150);
    qDebug()<<"circles num = "<<circles.size();
    for (size_t i = 0; i < circles.size(); i++){
        Point center(cvRound(circles[i][0]),cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        int length = cvRound(circles[i][2]/1.5f);
        circle(image, center,radius-10,Scalar(0),3);
        rectangle(image,Point(center.x-length,center.y-length),Point(center.x+length,center.y+length),Scalar(0),3);
        Mat imageSobel;
        Sobel(handleFrame(Rect(center.x-length,center.y-length,length*2,length*2)),imageSobel,CV_8U,1,1);
        meanValue = mean(imageSobel)[0];

        qDebug()<<"x="<<center.x<<",y="<<center.y<<",r="<<radius<<",meanValue="<<meanValue;
    }

    if (circles.size()==1)
    {
        result += 2;
        if (meanValue > 0.14)
            result += 4;
        handleFrame = Mat(500,100,image.type(),ExGlobal::hbufrgb);
        Point left_top(cvRound(circles[0][0]) + 490,cvRound(circles[0][1]) - 580);
        rectangle(image,left_top,Point(left_top.x+100,left_top.y+500),Scalar(255),3);
        cvtColor(image(Rect(left_top.x,left_top.y,100,500)), handleFrame, COLOR_RGB2GRAY);

        if (haveliquids(handleFrame))
            result += 8;
    }

#endif
    img2 = QImage((const uchar*)handleFrame.data,handleFrame.cols,handleFrame.rows,QImage::Format_Grayscale8);
    qDebug()<<"pierce end:"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    return result;
}

bool QRcoder::haveliquids(Mat &image)
{
    bool result = false;
    GaussianBlur(image, image, Size(25,25),0);
    threshold(image,image,0,255,THRESH_BINARY|THRESH_OTSU);

    vector<vector<Point>> contours;
    vector<Vec4i> hi;
    findContours(image,contours,hi,RETR_LIST,CHAIN_APPROX_NONE);
    qDebug()<<"contorus.size="<<contours.size();
    if (contours.size() < 2)
        return result;

    vector<int> maxminvalue;
    for (size_t i = 0; i < contours.size(); i++)
    {
        qDebug()<<"s="<<contourArea(contours[i])<<"L="<<arcLength(contours[i],true);
        if (contourArea(contours[i]) < 800)
            continue;
        int maxValue = contours[i][0].y;
        int minValue = contours[i][0].y;
        for (size_t j = 0; j < contours[i].size(); j++)
        {
            if (maxValue < contours[i][j].y)
                maxValue = contours[i][j].y;
            if (minValue > contours[i][j].y)
                minValue = contours[i][j].y;
        }
        maxminvalue.push_back(maxValue);
        maxminvalue.push_back(minValue);
    }

    qDebug()<<maxminvalue;
    if (maxminvalue.size()<4)
        return false;

    for (size_t i = 0; i < maxminvalue.size()/2; i++){
        size_t j;
        int minDistance = 1000;
        int minValue = maxminvalue[i*2+1];
        for (j = 0; j < maxminvalue.size()/2; j++)
        {
            if (i != j)
            {
                int tempValue = minValue - maxminvalue[j*2];
                if (tempValue < 0)
                    break;
                else if(tempValue < minDistance)
                    minDistance = tempValue;
            }
        }
        qDebug()<<"i="<<i<<"j="<<j<<"minValue="<<minDistance;
        if (j == maxminvalue.size()/2)
            return minDistance > 80;
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
