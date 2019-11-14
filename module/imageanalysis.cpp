#include "QZXing.h"
#include <QDebug>
#include <QImage>
#include <QDateTime>

#include "imageanalysis.h"
static const int shd = 170;
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

static int getPosValue(Mat source, Mat &mask1, Mat &mask2){
    int value = 0;
    int value2 = 0;
    int valuenum = 0;
    int value2num = 0;
    for (int i = 0; i < source.rows; i++)
    {
        const uchar* mask1data = mask1.ptr(i);
        const uchar* mask2data = mask2.ptr(i);
        uchar* sourcedata = source.ptr(i);
        for (int j = 0; j < source.cols; j++)
        {
            if (*mask1data != 0)
            {
                value += *sourcedata;
                valuenum++;
            }
            else if (*mask2data != 0)
            {
                value2 += *sourcedata;
                value2num++;
            }
            mask1data++;
            mask2data++;
            sourcedata++;
        }
    }
    if (valuenum == 0 || value2num == 0)
        return 0;
    value2 = value2*valuenum/value2num;
    return (value-value2)*10/valuenum;
}
static bool cvDebug = false;
ImageAnalysis::ImageAnalysis(QObject *parent) : QObject(parent)
{
}

QString ImageAnalysis::QRDecode(QImage img){
    QString result;
    QZXing decoder;
    decoder.setDecoder(QZXing::DecoderFormat_QR_CODE);
    result = decoder.decodeImage(img);
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
    else if(light == 4)
        tempImg = tempImg*15;

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
            if (indata == nullptr || indata2 == nullptr)
                break;
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
    x.clear();
    y.clear();
    posValue.clear();
    posItem.clear();
    posIndex.clear();
    if (imageType == 0)
        flip(Mat(1944,2592,CV_8U,data),firstImg,0);
        //Mat(1944,2592,CV_8U,data).copyTo(firstImg);
    FindGrid(firstImg,mpp,x,y);
    if (x.size()<gridCols || y.size()<gridRows)
        return;

    mask1 = Mat::zeros(firstImg.rows,firstImg.cols,firstImg.type());
    mask2 = Mat::zeros(firstImg.rows,firstImg.cols,firstImg.type());
    SpotMask(firstImg,mask1,mask2, x,y,subsize,md1,md2);
    qDebug()<<"FirstImage starttime="<<start_time.toString("hh:mm:ss.zzz");
    qDebug()<<"FirstImage endtime="<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
}

void ImageAnalysis::AddImage(void *data, int imageType){
    qDebug()<<"AddImage:";
    QDateTime start_time = QDateTime::currentDateTime();
    if (imageCount < 1 || x.size()<gridCols || y.size()<gridRows)
        return;
    imageCount++;
    posValue.clear();
    posItem.clear();
    posIndex.clear();
    if (imageType == 0)
        flip(Mat(1944,2592,CV_8U,data),firstImg,0);
        //Mat(1944,2592,CV_8U,data).copyTo(firstImg);

    GaussianBlur(firstImg, firstImg, Size(25,25),0);
    for (int j = 0; j < gridRows; j++)
    {
        for (int i = 0; i < gridCols; i++)
        {
            if (basePos[j][i].x != 0 && maskPos.at<uchar>(j,i) != 0 && maskPos.at<uchar>(j,i) != 1){
                int value = SpotCal(i,j);
                posItem.push_back(maskPos.at<uchar>(j,i));
                posValue.push_back(value);
                posIndex.push_back(j*gridCols+i);
                qDebug()<<"AddImage,maskPos="<<maskPos.at<uchar>(j,i)<<",value="<<value;
            }
        }
    }
    qDebug()<<"AddImage starttime="<<start_time.toString("hh:mm:ss.zzz");
    qDebug()<<"AddImage endtime="<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
}

int ImageAnalysis::subImageHandle(bool debug, size_t posX, size_t posY, Mat &img, Mat &subimg, int thrsh,vector<vector<Point>> &contours,Point &centerPoint){
    vector<Vec4i> hi;
    int center = -1;
    int topsub = y[posY] - (subsize>>1);
    int bottom = y[posY] + (subsize>>1);
    int leftsub = x[posX] - (subsize>>1);
    int rightsub = x[posX] + (subsize>>1);
    if(topsub < 0 || bottom >=img.rows || leftsub < 0 || rightsub >= img.cols)
        return -2;

    if (debug)
    {
        namedWindow("source Image",0);
        resizeWindow("source Image",subsize<<1,subsize<<1);
        imshow("source Image",img(Rect(leftsub,topsub,subsize+1,subsize+1))*3);
    }

    normalize(img(Rect(leftsub,topsub,subsize+1,subsize+1)),subimg,0,255,NORM_MINMAX);
    if (debug)
    {
        namedWindow("sub Image",0);
        resizeWindow("sub Image",subsize<<1,subsize<<1);
        imshow("sub Image",subimg);
    }    

    threshold(subimg,subimg,thrsh,255,THRESH_BINARY);
    if (debug)
    {
        namedWindow("threshold Image",0);
        resizeWindow("threshold Image",subsize<<1,subsize<<1);
        imshow("threshold Image",subimg);
    }

    //imclose
    Mat element = getStructuringElement(MORPH_ELLIPSE,Size(9,9));
    morphologyEx(subimg,subimg,MORPH_CLOSE,element);
    //morphologyEx(subImage,subImage,MORPH_OPEN,element);
    if (debug)
    {
        namedWindow("Close Image",0);
        resizeWindow("Close Image",subsize<<1,subsize<<1);
        imshow("Close Image",subimg);
    }

    //imfill
#if 0
    Mat mfloodFill = Mat::zeros(subsize,subsize,img.type());
    bitwise_not(subImage,mfloodFill);
    floodFill(mfloodFill,Point(0,0),Scalar(0));
    subImage |= mfloodFill;
    if (j == debugx && i == debugy)
        imshow("fill Image",subImage);
#endif

    findContours(subimg,contours,hi,RETR_EXTERNAL,CHAIN_APPROX_NONE);
    for (size_t z = 0; z < contours.size(); z++)
    {
        bool valid = true;
        for (size_t t = 0; t < contours[z].size(); t++)
        {
            if (contours[z][t].x == 0 || contours[z][t].x == subimg.cols - 1 || contours[z][t].y == 0 || contours[z][t].y == subimg.rows - 1)
            {
                valid = false;
                //floodFill(subimg,Point(contours[z][t].x,contours[z][t].y),Scalar(0));
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

    if (debug){
        vector<vector<Point>> contours;
        vector<Vec4i> hi;
        findContours(subimg,contours,hi,RETR_EXTERNAL,CHAIN_APPROX_NONE);

        for (size_t z = 0; z < contours.size(); z++)
        {
            if (contours[z].size()>10){
                RotatedRect box = fitEllipse(contours[z]);
                qDebug()<<"box:width="<<box.size.width<<",height="<<box.size.height;
                ellipse(subimg,box,Scalar(120),-1);
            }
        }

        namedWindow("fitEllipse Image",0);
        resizeWindow("fitEllipse Image",subsize<<1,subsize<<1);
        imshow("fitEllipse Image",subimg);
    }

    return center;
}

void ImageAnalysis::UpdateMask(int top, int left, vector<vector<Point>> &contours, int center){
    Mat subImage1 = Mat::zeros(subsize+1,subsize+1,mask1.type());
    Mat subImage2 = Mat::zeros(subsize+1,subsize+1,mask1.type());
    Mat subImage3 = Mat::zeros(subsize+1,subsize+1,mask1.type());
    Mat element2 = getStructuringElement(MORPH_ELLIPSE,Size(md1+1,md1+1));
    Mat element3 = getStructuringElement(MORPH_ELLIPSE,Size(md2+1,md2+1));

    drawContours(subImage1,contours,center,Scalar(1),-1);

    dilate(subImage1,subImage1,element2);
    dilate(subImage1,subImage2,element3);
    dilate(subImage2,subImage3,element3);
    mask1(Rect(left,top,subsize+1,subsize+1)) += subImage1;
    mask2(Rect(left,top,subsize+1,subsize+1)) = subImage3 - subImage2;
}

void ImageAnalysis::EllipseMask(int top, int left, vector<Point> &contour){
    Mat subImage1 = Mat::zeros(subsize+1,subsize+1,mask1.type());
    Mat subImage2 = Mat::zeros(subsize+1,subsize+1,mask1.type());
    Mat subImage3 = Mat::zeros(subsize+1,subsize+1,mask1.type());
    Mat element2 = getStructuringElement(MORPH_ELLIPSE,Size(md1+11,md1+11));
    Mat element3 = getStructuringElement(MORPH_ELLIPSE,Size(md2+1,md2+1));

    RotatedRect box = fitEllipse(contour);
    ellipse(subImage1,box,Scalar(1),-1);

    dilate(subImage1,subImage1,element2);
    dilate(subImage1,subImage2,element3);
    dilate(subImage2,subImage3,element3);
    mask1(Rect(left,top,subsize+1,subsize+1)) += subImage1;
    mask2(Rect(left,top,subsize+1,subsize+1)) = subImage3 - subImage2;
}

void ImageAnalysis::SpotMask(Mat &img, Mat &mask1, Mat &mask2, vector<int> &x, vector<int> &y, int subsize, int d1, int d2){
    int w = img.cols;
    int h = img.rows;

    Mat subImage = Mat::zeros(subsize+1,subsize+1,img.type());
    vector<vector<Point>> matPoint(y.size(),vector<Point>(x.size()));
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

            vector<vector<Point>> contours;
            int center = subImageHandle(false,j,i,img,subImage,shd,contours,matPoint[i][j]);

            if (center >= 0)
                UpdateMask(topsub, leftsub,contours,center);
        }
    }

    topPointx = 0;
    topPointy = 0;
    int pointCount = 0;
    for (size_t z = 0; z < x.size()-gridCols + 1; z++)
    {
        int tempPointCount = 0;
        for(size_t i = z; i < z+gridCols; i++)
        {
            for (size_t j = 0; j < y.size(); j++)
            {
                if (matPoint[j][i].x != 0 || matPoint[j][i].y != 0)
                    tempPointCount++;
            }
        }
        if (tempPointCount > pointCount)
        {
            pointCount = tempPointCount;
            topPointx = static_cast<int>(z);
        }
    }
    
    pointCount = 0;
    for (size_t z = 0; z < y.size()-gridRows + 1; z++){
        int tempPointCount = 0;
        for (size_t j = z; j < z + gridRows; j++){
            for (size_t i = 0; i < x.size(); i++)
            {
                if (matPoint[j][i].x != 0 || matPoint[j][i].y != 0)
                    tempPointCount++;
            }
        }
        if (tempPointCount > pointCount)
        {
            pointCount = tempPointCount;
            topPointy = static_cast<int>(z);
        }
    }
    qDebug()<<"topPointx:"<<topPointx<<"topPointy:"<<topPointy;

    for (size_t i = 0; i < y.size(); i++)
    {
        for (size_t j = 0; j < x.size(); j++){
            if ((i < topPointy || i >= topPointy+gridRows) || (j < topPointx || j >= topPointx+gridCols) || maskPos.at<uchar>(i-topPointy,j-topPointx) == 0)
            {
                if (matPoint[i][j].x != 0 || matPoint[i][j].y != 0)
                {
                    int topsub = y[i] - (subsize>>1);
                    int leftsub = x[j] - (subsize>>1);
                    mask1(Rect(leftsub,topsub,subsize+1,subsize+1)) = 0;
                    mask2(Rect(leftsub,topsub,subsize+1,subsize+1)) = 0;
                }
            }
        }
    }

    for (size_t i = topPointy; i < topPointy+gridRows; i++)
    {
        for (size_t j = topPointx; j < topPointx+gridCols; j++)
        {
            basePos[i-topPointy][j-topPointx].x = matPoint[i][j].x;
            basePos[i-topPointy][j-topPointx].y = matPoint[i][j].y;
            qDebug()<<"basePos["<<i-topPointy<<"]["<<j-topPointx<<"] = "<<"("<<basePos[i-topPointy][j-topPointx].x<<","<<basePos[i-topPointy][j-topPointx].y<<")";
            if (maskPos.at<uchar>(i-topPointy,j-topPointx) != 0/* && maskPos.at<uchar>(i-topPointy,j-topPointx) != 14*/){
                int topsub = y[i] - (subsize>>1);
                int leftsub = x[j] - (subsize>>1);                
                if (basePos[i-topPointy][j-topPointx].x == 0){
                    vector<vector<Point>> contours;
                    int center = -1;
                    for (int z = 1; z < 7 && center == -1; z++)
                    {
                        center = subImageHandle(false,j,i,img,subImage,shd-20*z,contours,basePos[i-topPointy][j-topPointx]);
                        qDebug()<<"reset Image,shd="<<(shd-20*z)<<",center="<<center;
                        if (center >= 0)
                            UpdateMask(topsub, leftsub,contours,center);
                    }

                    for (int z = 1; z < 3 && center == -1; z++)
                    {
                        center = subImageHandle(false,j,i,img,subImage,shd+20*z,contours,basePos[i-topPointy][j-topPointx]);
                        qDebug()<<"reset Image,shd="<<(shd+20*z)<<",center="<<center;
                        if (center >= 0)
                            UpdateMask(topsub, leftsub,contours,center);
                    }

                    if (center == -1){
                        subImageHandle(false,j,i,img,subImage,shd,contours,basePos[i-topPointy][j-topPointx]);
                        for (size_t z = 0; z < contours.size(); z++){
                            bool valid = true;
                            for (size_t t = 0; t < contours[z].size(); t++)
                            {
                                if (contours[z][t].x == 0 || contours[z][t].x == subImage.cols - 1 || contours[z][t].y == 0 || contours[z][t].y == subImage.rows - 1)
                                {
                                    valid = false;
                                    break;
                                }
                            }

                            if (valid){
                                double s = contourArea(contours[z]);
                                qDebug()<<"\tellipse mask,s="<<s<<",z="<<z<<",contours[z].size="<<contours[z].size();
                                if (s>600 && s<6000 && contours[z].size()>10)
                                {
                                    if (center == -1)
                                    {
                                        GetCenter(contours[z],basePos[i-topPointy][j-topPointx]);
                                        center = static_cast<int>(z);
                                    }
                                    else {
                                        Point point;
                                        GetCenter(contours[z],point);
                                        if (abs(point.x - (subsize>>1))+abs(point.y - (subsize>>1)) < abs(basePos[i-topPointy][j-topPointx].x - (subsize>>1))+abs(basePos[i-topPointy][j-topPointx].y - (subsize>>1)))
                                        {
                                            center = static_cast<int>(z);
                                            basePos[i-topPointy][j-topPointx].x = point.x;
                                            basePos[i-topPointy][j-topPointx].y = point.y;
                                        }
                                    }
                                }
                            }
                        }
                        if (center >=0){
                            RotatedRect box = fitEllipse(contours[center]);
                            EllipseMask(topsub,leftsub,contours[center]);
                        }
                    }
                }
                if (basePos[i-topPointy][j-topPointx].x != 0 && maskPos.at<uchar>(i-topPointy,j-topPointx) != 1){
                    Mat submask1 = mask1(Rect(leftsub,topsub,subsize+1,subsize+1));
                    Mat submask2 = mask2(Rect(leftsub,topsub,subsize+1,subsize+1));
                    int value = getPosValue(img(Rect(leftsub,topsub,subsize+1,subsize+1)),submask1,submask2);
                    posItem.push_back(maskPos.at<uchar>(i-topPointy,j-topPointx));
                    posValue.push_back(value);
                    posIndex.push_back((i-topPointy)*gridCols+j-topPointx);
                    qDebug()<<"maskPos="<<maskPos.at<uchar>(i-topPointy,j-topPointx)<<",value="<<value;
                }
            }
        }
    }
    qDebug()<<"posValue,size="<<posValue.size();
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
}

void ImageAnalysis::SetMask(void *data, int maskType){    
    if (maskType == 0){        
        gridRows = 11;
        gridCols = 11;
        maskPos = Mat::zeros(11,11,CV_8UC1);
        //basePos = new Point*[11];
        //for (int i = 0; i < 11; i++)
        //    basePos[i] = new Point[11];
        memcpy(maskPos.data,data,121);
        cout<<maskPos<<endl;        
    }
}

int ImageAnalysis::SpotCal(int PosX, int PosY){    
    int topsub = y[PosY+topPointy] - (subsize>>1);
    int bottom = y[PosY+topPointy] + (subsize>>1);
    int leftsub = x[PosX+topPointx] - (subsize>>1);
    int rightsub = x[PosX+topPointx] + (subsize>>1);

    if (PosY+topPointy == debugy && PosX+topPointx == debugx)
        qDebug()<<"SpotCal,(x,y)="<<PosX<<","<<PosY<<" basePos:"<<basePos[PosY][PosX].x<<","<<basePos[PosY][PosX].y<<"\ttop:"<<topsub<<",left:"<<leftsub<<",bottom:"<<bottom<<",right:"<<rightsub;
    if (topsub < 0 || bottom >= firstImg.rows || leftsub < 0 || rightsub >= firstImg.cols)
    {
        qDebug()<<"topsub="<<topsub<<",bottom="<<bottom<<",leftsub="<<leftsub<<"rightsub="<<rightsub;
        return 0;
    }
    Mat subImage = Mat::zeros(subsize+1,subsize+1,firstImg.type());    
    vector<vector<Point>> contours;
    Point centerPoint;
    int center = subImageHandle(false,PosX+topPointx,PosY+topPointy,firstImg,subImage,shd,contours,centerPoint);

    if (PosY+topPointy == debugy && PosX+topPointx == debugx)
        qDebug()<<"center:"<<center<<",x="<<centerPoint.x<<",y="<<centerPoint.y;
    if (center == -1 || abs(centerPoint.x - basePos[PosY][PosX].x)>20 || abs(centerPoint.y - basePos[PosY][PosX].y)>20)
    {
        centerPoint.x = basePos[PosY][PosX].x;
        centerPoint.y = basePos[PosY][PosX].y;
    }

    Mat subMask1 = mask1(Rect(leftsub,topsub,subsize+1,subsize+1));
    Mat subMask2 = mask2(Rect(leftsub,topsub,subsize+1,subsize+1));
    leftsub = leftsub - basePos[PosY][PosX].x + centerPoint.x;
    if (leftsub < 0) leftsub = 0;
        rightsub = rightsub - basePos[PosY][PosX].x + centerPoint.x;
    if (rightsub >= firstImg.cols)
        leftsub = leftsub - (rightsub-firstImg.cols+1);
    topsub = topsub - basePos[PosY][PosX].y + centerPoint.y;
    if (topsub < 0) topsub = 0;
    bottom = bottom - basePos[PosY][PosX].y + centerPoint.y;
    if (bottom >= firstImg.rows)
        topsub = topsub - (bottom-firstImg.rows+1);
    Mat subSource = firstImg(Rect(leftsub,topsub,subsize+1,subsize+1));

    return getPosValue(subSource,subMask1,subMask2);
}

void ImageAnalysis::SetDebugPos(size_t x, size_t y)
{
    debugx = x;
    debugy = y;
    Mat subImage = Mat::zeros(subsize+1,subsize+1,firstImg.type());
    vector<vector<Point>> contours;
    Point centerPoint;
    subImageHandle(true,x,y,firstImg,subImage,thrsh,contours,centerPoint);
}
