//#include "QZXing.h"
#include <QDebug>
#include <QImage>
#include <QDateTime>

#include "imageanalysis.h"
static void GetCenter(vector<Point> &c, Point &center)
{
    int pointCount = c.size();
    int x = 0;
    int y = 0;
    for (int i = 0; i < pointCount; i++){
        x += c[i].x;
        y += c[i].y;
    }
    center.x = x/pointCount;
    center.y = y/pointCount;
}

static bool cvDebug = false;
ImageAnalysis::ImageAnalysis(QObject *parent) : QObject(parent)
{

}
QString ImageAnalysis::QRDecode(QString img){
    QString result;


    return result;
}

QImage ImageAnalysis::getMainImg(int type,int light){
    qDebug()<<"getMainImg,type="<<type<<",light:"<<light;
    Mat tempImg = firstImg.clone();
    if(light == 1)
        tempImg = tempImg*3;
    else if(light == 2)
        tempImg = tempImg*6;
    else if(light == 3)
        tempImg = tempImg*10;

    if (type == 0){
        for(size_t i = 0; i < x.size(); i++){
            line(tempImg,Point(x[i],0),Point(x[i],tempImg.rows-1),Scalar(255),4);
            putText(tempImg,to_string(i),Point(x[i],40),FONT_HERSHEY_PLAIN,4,Scalar(255),4);
        }
        for(size_t i = 0; i < y.size(); i++){
            line(tempImg,Point(0,y[i]),Point(tempImg.cols-1,y[i]),Scalar(255),4);
            putText(tempImg,to_string(i),Point(0,y[i]),FONT_HERSHEY_PLAIN,4,Scalar(255),4);
        }

        for (int i = 0; i < tempImg.rows; i++)
        {
            const uchar* indata = mask1.ptr(i);
            const uchar* indata2 = mask2.ptr(i);
            uchar* outdata = tempImg.ptr(i);
            for (int j = 0; j < tempImg.cols; j++)
            {
                if (*indata != 0 || *indata2 != 0)
                    *outdata = 255;
                indata++;
                indata2++;
                outdata++;
            }
        }
    }

    QImage image(tempImg.cols,tempImg.rows,QImage::Format_Indexed8);
    image.setColorCount(256);
    for (int i = 0; i < 256; i++)
        image.setColor(i,qRgb(i,i,i));

    uchar *pSrc = tempImg.data;
    for (int row = 0; row < tempImg.rows; row++)
    {
        uchar *pDest = image.scanLine(row);
        memcpy(pDest, pSrc, tempImg.cols);
        pSrc += tempImg.step;
    }
    return image;
}

void ImageAnalysis::FirstImage(void *data, int imageType){
    QDateTime start_time = QDateTime::currentDateTime();
    imageCount = 1;
    if (imageType == 0)
        firstImg = Mat(1944,2592,CV_8U,data);
    FindGrid(firstImg,mpp,x,y);
    if (x.size()<gridCols || y.size()<gridRows)
        return;

    mask1 = Mat::zeros(firstImg.rows,firstImg.cols,firstImg.type());
    mask2 = Mat::zeros(firstImg.rows,firstImg.cols,firstImg.type());
    SpotMask(firstImg,mask1,mask2, x,y,subsize,md1,md2);
    qDebug()<<"FirstImage starttime="<<start_time.toString("hh:mm:ss.zzz");
    qDebug()<<"FirstImage endtime="<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");

    if (cvDebug){
        line(firstImg,Point(x[debugx] - (subsize>>1),y[debugy] - (subsize>>1)),Point(x[debugx] + (subsize>>1),y[debugy] - (subsize>>1)),Scalar(255));
        line(firstImg,Point(x[debugx] - (subsize>>1),y[debugy] - (subsize>>1)),Point(x[debugx] - (subsize>>1),y[debugy] + (subsize>>1)),Scalar(255));
        line(firstImg,Point(x[debugx] + (subsize>>1),y[debugy] - (subsize>>1)),Point(x[debugx] + (subsize>>1),y[debugy] + (subsize>>1)),Scalar(255));
        line(firstImg,Point(x[debugx] - (subsize>>1),y[debugy] + (subsize>>1)),Point(x[debugx] + (subsize>>1),y[debugy] + (subsize>>1)),Scalar(255));

        namedWindow("Input Image",0);
        cv::resizeWindow("Input Image",1000,800);
        imshow("Input Image",firstImg*3);
        namedWindow("mask Image",0);
        cv::resizeWindow("mask Image",1000,800);
        imshow("mask Image",mask1*2);
        namedWindow("mask2 Image",0);
        cv::resizeWindow("mask2 Image",1000,800);
        imshow("mask2 Image",(mask1+mask2)*2);
    }
}

void ImageAnalysis::SpotMask(Mat img, Mat &mask1, Mat &mask2, vector<int> &x, vector<int> &y, int subsize, int d1, int d2){
    int w = img.cols;
    int h = img.rows;

    Mat subImage = Mat::zeros(subsize+1,subsize+1,img.type());
    vector<vector<Point>> matPoint(x.size(),vector<Point>(y.size()));
    for (size_t i = 0; i < y.size(); i++)
    {
        int topsub = y[i] - (subsize>>1);
        int bottom = y[i] + (subsize>>1);
        if (topsub < 0 || bottom >= h)
            continue;

        for (size_t j = 0; j < x.size(); j++){
            int leftsub = x[j] - (subsize>>1);
            int rightsub = x[j] + (subsize>>1);

            if (leftsub < 0 || rightsub >= w)
                continue;
            if (cvDebug && j == debugx && i == debugy)
            {
                qDebug()<<"source image,x="<<leftsub<<",y="<<topsub;
                imshow("source Image",img(Rect(leftsub,topsub,subsize+1,subsize+1))*3);
            }

            normalize(img(Rect(leftsub,topsub,subsize+1,subsize+1)),subImage,0,255,NORM_MINMAX);
            if (cvDebug && j == debugx && i == debugy)
                imshow("sub Image",subImage);
            medianBlur(subImage,subImage,5);

            threshold(subImage,subImage,150,255,THRESH_BINARY);
            if (cvDebug && j == debugx && i == debugy)
                imshow("threshold Image",subImage);

            //imclose
            Mat element = getStructuringElement(MORPH_RECT,Size(15,15));
            morphologyEx(subImage,subImage,MORPH_OPEN,element);
            if (cvDebug && j == debugx && i == debugy)
                imshow("Close Image",subImage);

            //imfill
#if 0
            Mat mfloodFill = Mat::zeros(subsize,subsize,img.type());
            bitwise_not(subImage,mfloodFill);
            floodFill(mfloodFill,Point(0,0),Scalar(0));
            subImage |= mfloodFill;
            if (j == debugx && i == debugy)
                imshow("fill Image",subImage);
#endif
            vector<vector<Point>> contours;
            vector<Vec4i> hi;
            findContours(subImage,contours,hi,RETR_EXTERNAL,CHAIN_APPROX_NONE);
            qDebug()<<"findContours:("<<j<<","<<i<<"),Countours:"<<contours.size();

#if 0
            for (size_t z = 0; z < contours.size(); z++)
            {
                bool valid = true;
                for (size_t t = 0; t < contours[z].size(); t++)
                {
                    if (contours[z][t].x == 0 || contours[z][t].x == subImage.cols - 1 || contours[z][t].y == 0 || contours[z][t].y == subImage.rows - 1)
                    {
                        valid = false;
                        floodFill(subImage,Point(contours[z][t].x,contours[z][t].y),Scalar(0));
                        break;
                    }
                }
                if (valid)
                {
                    double s = contourArea(contours[z]);
                    double l = arcLength(contours[z],true);
                    qDebug()<<"\t"<<z<<","<<"area:"<<s<<",len:"<<l<<",s/l="<<(4*3.14*s/l/l);
                    if (s>600 && s<6000 && (4*3.14*s/l/l)>0.7)
                    {
                        Mat subImage1 = Mat::zeros(subsize+1,subsize+1,img.type());
                        Mat subImage2 = Mat::zeros(subsize+1,subsize+1,img.type());
                        Mat subImage3 = Mat::zeros(subsize+1,subsize+1,img.type());
                        Mat element2 = getStructuringElement(MORPH_ELLIPSE,Size(d1+1,d1+1));
                        Mat element3 = getStructuringElement(MORPH_ELLIPSE,Size(d2+1,d2+1));

                        drawContours(subImage1,contours,z,Scalar(i*x.size()+j),-1);
                        qDebug()<<"\t\t"<<"fill color"<<(i*x.size()+j);

                        dilate(subImage1,subImage1,element2);
                        dilate(subImage1,subImage2,element3);
                        dilate(subImage2,subImage3,element3);
                        mask1(Rect(leftsub,topsub,subsize+1,subsize+1)) += subImage1;
                        mask2(Rect(leftsub,topsub,subsize+1,subsize+1)) = subImage3 - subImage2;

                        if (cvDebug && j == debugx && i == debugy)
                        {
                            imshow("dilate1",subImage1);
                            imshow("dilate2",subImage2);
                            imshow("dilate3",subImage3);
                        }
                    }
                }
            }
#else
            int center = -1;
            Point centerPoint;
            for (size_t z = 0; z < contours.size(); z++)
            {
                bool valid = true;
                for (size_t t = 0; t < contours[z].size(); t++)
                {
                    if (contours[z][t].x == 0 || contours[z][t].x == subImage.cols - 1 || contours[z][t].y == 0 || contours[z][t].y == subImage.rows - 1)
                    {
                        valid = false;
                        floodFill(subImage,Point(contours[z][t].x,contours[z][t].y),Scalar(0));
                        break;
                    }
                }
                if (valid){
                    double s = contourArea(contours[z]);
                    double l = arcLength(contours[z],true);
                    if (s>600 && s<6000 && (4*3.14*s/l/l)>0.7)
                    {
                        if (center == -1)
                        {
                            GetCenter(contours[z],centerPoint);
                            center = static_cast<int>(z);
                        }
                        else {
                            Point point;
                            GetCenter(contours[z],point);
                            if (abs(point.x - (subsize>>1))+abs(point.y - (subsize>>1)) < abs(centerPoint.x - (subsize>>1))+abs(centerPoint.y - (subsize>>1)))
                            {
                                center = static_cast<int>(z);
                                centerPoint.x = point.x;
                                centerPoint.y = point.y;
                            }
                        }
                    }
                }
            }

            if (center != -1)
            {
                matPoint[j][i].x = centerPoint.x;
                matPoint[j][i].y = centerPoint.y;
                Mat subImage1 = Mat::zeros(subsize+1,subsize+1,img.type());
                Mat subImage2 = Mat::zeros(subsize+1,subsize+1,img.type());
                Mat subImage3 = Mat::zeros(subsize+1,subsize+1,img.type());
                Mat element2 = getStructuringElement(MORPH_ELLIPSE,Size(d1+1,d1+1));
                Mat element3 = getStructuringElement(MORPH_ELLIPSE,Size(d2+1,d2+1));

                drawContours(subImage1,contours,center,Scalar(i*x.size()+j),-1);
                qDebug()<<"\t\t"<<"fill color"<<(i*x.size()+j);

                dilate(subImage1,subImage1,element2);
                dilate(subImage1,subImage2,element3);
                dilate(subImage2,subImage3,element3);
                mask1(Rect(leftsub,topsub,subsize+1,subsize+1)) += subImage1;
                mask2(Rect(leftsub,topsub,subsize+1,subsize+1)) = subImage3 - subImage2;

            }
        }
#endif
    }

    int topPointx = 0;
    int topPointy = 0;
    int pointCount = 0;
    for (size_t z = 0; z < x.size()-gridCols; z++)
    {
        int tempPointCount = 0;
        for(int i = z; i < z+gridCols; i++)
        {
            for (size_t j = 0; j < y.size(); j++)
            {
            if (matPoint[i][j].x != 0 || matPoint[i][j].y != 0)
                tempPointCount++;
            }
        }
        if (tempPointCount > pointCount)
        {
            pointCount = tempPointCount;
            topPointx = z;
        }
    }

    pointCount = 0;
    for (size_t z = 0; z < y.size()-gridRows; z++){
        int tempPointCount = 0;
        for (int j = z; j < z + gridRows; j++){
            for (size_t i = 0; i < x.size(); i++)
            {
            if (matPoint[i][j].x != 0 || matPoint[i][j].y != 0)
                tempPointCount++;
            }
        }
        if (tempPointCount > pointCount)
        {
            pointCount = tempPointCount;
            topPointy = z;
        }
    }
    qDebug()<<"topPointx:"<<topPointx<<"topPointy:"<<topPointy;

    for (size_t i = 0; i < y.size(); i++)
    {
        for (size_t j = 0; j < x.size(); j++){
            if ((i < topPointy || i >= topPointy+gridRows) || (j < topPointx || j >= topPointx+gridCols)
                ||(i == topPointy && j == topPointx)||(i == topPointy+gridRows-1 && j == topPointx+gridCols-1)
                    ||(i == topPointy+1 && j == topPointx)||(i == topPointy && j == topPointx+1)
                    ||(i == topPointy+gridRows-2 && j == topPointx+gridCols-1)||(i == topPointy+gridRows-1 && j == topPointx+gridCols-2)
                    ||(i == topPointy+1 && j == topPointx+gridCols-1)||(i == topPointy+gridRows-2 && j == topPointx)
                    ||(i == topPointy+gridRows-2 && j == topPointx+gridCols-1)||(i == topPointy+gridRows-1 && j == topPointx+gridCols-2)
                    ||(i == topPointy+gridRows-1 && j == topPointx+1)||(i == topPointy+gridRows-2 && j == topPointx)
                        ||(i == topPointy+gridRows-1 && j == topPointx)||(i == topPointy && j == topPointx+gridCols-1))
            if (matPoint[j][i].x != 0 || matPoint[j][i].y != 0)
            {
                int topsub = y[i] - (subsize>>1);
                int bottom = y[i] + (subsize>>1);
                int leftsub = x[j] - (subsize>>1);
                int rightsub = x[j] + (subsize>>1);
                mask1(Rect(leftsub,topsub,subsize+1,subsize+1)) = 0;
                mask2(Rect(leftsub,topsub,subsize+1,subsize+1)) = 0;
            }
        }
    }
}

void ImageAnalysis::FindPeaks(int *number, int numberLen, int mpp, vector<int> &pos){
    int *sign = new int[numberLen];
    int diff;
    for (int i = 1; i < numberLen; i++){
        diff = number[i] - number[i-1];
        if (diff > 0)
            sign[i-1] = 1;
        else if(diff < 0)
            sign[i-1] = -1;
        else
            sign[i-1] = 0;
    }
    for (int i = 1; i < numberLen - 1; i++)
    {
        if (sign[i]<sign[i-1])
            pos.push_back(i);
    }
    for (size_t i = 1; i < pos.size(); i++)
    {
        for (size_t j = i; j > 0; j--)
        {
            if (number[pos[j]] > number[pos[j-1]]){
                int temp = pos[j-1];
                pos[j-1] = pos[j];
                pos[j] = temp;
            }
        }
    }

    for (size_t i = 0; i < pos.size()-1; i++)
    {
        if (pos[i] == 0)
            continue;
        for (size_t j = i+1; j < pos.size(); j++)
        {
            if (pos[j] < pos[i]+ mpp && pos[j] > pos[i] - mpp)
                pos[j] = 0;
        }
    }

    for (vector<int>::iterator it = pos.begin(); it != pos.end();)
    {
        if (*it == 0)
            it = pos.erase(it);
        else
            ++it;
    }
    sort(pos.begin(),pos.end());
}

void ImageAnalysis::FindGrid(Mat img, int mpp, vector<int> &x, vector<int> &y){
    int w = img.cols;
    int h = img.rows;
    int *xnumber = new int[w];
    int *ynumber = new int[h];
    //Mat destImg;
    //mean
    GaussianBlur(img, img, Size(25,25),0);
    Mat_<uchar>::iterator it = img.begin<uchar>();
    for (int i = 0; i < h; i++)
    {
        ynumber[i] = 0;
        for(int j = 0; j < w; j++)
            ynumber[i] += *(it + i*w + j);
    }
    for (int i = 0; i < w; i++)
    {
        xnumber[i] = 0;
        for(int j = 0; j < h; j++)
            xnumber[i] += *(it + j*w + i);
    }

    FindPeaks(xnumber,w,mpp,x);
    FindPeaks(ynumber,h,mpp,y);
    qDebug()<<"xnumber:";
    for (size_t i = 0; i < x.size(); i++)
    {
        if (i > 0)
            qDebug()<<i<<"- "<<x[i]<<","<<xnumber[x[i]]<<",Dx="<<(x[i]-x[i-1]);
        else
            qDebug()<<i<<"- "<<x[i]<<","<<xnumber[x[i]]<<",Dx="<<x[i];
    }
    qDebug()<<"ynumber:";
    for (size_t i = 0; i < y.size(); i++)
    {
        if (i > 0)
            qDebug()<<i<<"- "<<y[i]<<","<<ynumber[y[i]]<<",Dy="<<(y[i]-y[i-1]);
        else
            qDebug()<<i<<"- "<<y[i]<<","<<ynumber[y[i]]<<",Dy="<<y[i];
    }
    qDebug()<<"findgrid:"<<*(it+w*y[8]+x[8]);
}
