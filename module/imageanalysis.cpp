
//#include "QZXing.h"
#include <QDebug>
#include <QImage>
#include <QDateTime>

#include "imageanalysis.h"
#include "exglobal.h"

static const int shd = 170;
int ImageAnalysis::imageWidth = 2592;
int ImageAnalysis::imageHeight = 1944;
int ImageAnalysis::calWidth = 600;

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


static void GetCenter(Mat &img, Point &center){
    Moments mo = cv::moments(img);
    center.x = mo.m10/mo.m00;
    center.y = mo.m01/mo.m00;
}

static int GetPosValue(int posIndex, Mat source, Mat &mask1, Mat &mask2, QVector<int> &posValue){
    int64 value = 0;

    for (int i = 0; i < 5; i++)
        posValue[i] = 0;

    if (source.type() == CV_8U)
    {
        for (int i = 0; i < source.rows; i++)
        {
            const uchar* mask1data = mask1.ptr(i);
            const uchar* mask2data = mask2.ptr(i);
            uchar* sourcedata = source.ptr(i);
            for (int j = 0; j < source.cols; j++,mask1data++,mask2data++,sourcedata++)
            {
                if (*mask1data == posIndex)
                {
                    posValue[0] += *sourcedata;
                    posValue[1]++;
                }
                else if (*mask2data == posIndex+1)
                {
                    posValue[2] += *sourcedata;
                    posValue[3]++;
                }
            }
        }
    }
    else{
        for (int i = 0; i < source.rows; i++)
        {
            const uchar* mask1data = mask1.ptr(i);
            const uchar* mask2data = mask2.ptr(i);
            ushort* sourcedata = source.ptr<ushort>(i);
            for (int j = 0; j < source.cols; j++,mask1data++,mask2data++,sourcedata++)
            {
                if (*mask1data == posIndex)
                {
                    posValue[0] += *sourcedata;
                    posValue[1]++;
                }
                else if (*mask2data == posIndex+1)
                {
                    posValue[2] += *sourcedata;
                    posValue[3]++;
                }
            }
        }
    }

    if (posValue[3] == 0 || posValue[1] == 0)
        return 0;
    value = posValue[2];
    value = value*posValue[1]/posValue[3];
    posValue[4] = (posValue[0] - value)*10/posValue[1];
    return posValue[4];
}

ImageAnalysis::ImageAnalysis(QObject *parent) : QObject(parent)
{
    firstImg = Mat::zeros(imageHeight,imageWidth,CV_8U);
}

QString ImageAnalysis::QRDecode(QImage img){
    QString result;
    //QZXing decoder;
    //decoder.setDecoder(QZXing::DecoderFormat_QR_CODE);
    //result = decoder.decodeImage(img);
    return result;
}

QImage ImageAnalysis::getMaskImg(){
    Mat tempImg = mask1+mask2;
    //QImage image(tempImg.cols,tempImg.rows,QImage::Format_Indexed8);
    QImage image(tempImg.cols,tempImg.rows,QImage::Format_Grayscale8);
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
            line(tempImg,Point(x[i],0),Point(x[i],tempImg.rows-1),Scalar(120),4);
            putText(tempImg,to_string(i),Point(x[i],40),FONT_HERSHEY_PLAIN,4,Scalar(255),4);
        }
        for(size_t i = 0; i < y.size(); i++){
            line(tempImg,Point(0,y[i]),Point(tempImg.cols-1,y[i]),Scalar(120),4);
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
                if (*indata != 0)
                    //*outdata = 255;
                    *outdata = 150;
                if (*indata2 > 10)
                    *outdata = 250;
                else if(*indata2 > 0)
                    *outdata = 50;
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
    Mat temp;
    imageCount = 1;
    x.clear();
    y.clear();

    qDebug()<<"FirstImage:"<<imageType;
    if (imageType == 0)
    {
        flip(Mat(imageHeight,imageWidth,CV_8U,data),firstImg,0);
        temp = firstImg;
    }
    else if(imageType == 1)
    {
        temp = Mat(imageHeight,imageWidth,CV_16U,data);
        for (int i = 0; i < imageHeight; i++)
        {
            uchar *outdata = firstImg.ptr<uchar>(i);
            const uint16_t *indata = temp.ptr<uint16_t>(i);
            for (int j = 0; j < imageWidth; j++,outdata++,indata++){
                *outdata = (*indata)>>4;
            }
        }
    }
    //Mat(1944,2592,CV_8U,data).copyTo(firstImg);
    GetCenter(temp,firstCenter);
    qDebug()<<"firstCenter:"<<firstCenter.x<<","<<firstCenter.y;

    FindGrid(firstImg,mpp,x,y);
    if (x.size()<gridCols || y.size()<gridRows)
        return;

    mask1 = Mat::zeros(firstImg.rows,firstImg.cols,firstImg.type());
    mask2 = Mat::zeros(firstImg.rows,firstImg.cols,firstImg.type()/*CV_16U*/);
    SpotMask(firstImg, x,y,subsize);
    pushData(temp,firstCenter);
}

void ImageAnalysis::AddImage(void *data, int imageType){    
    Point addImageCenter;
    Mat temp;
    if (imageCount < 1 || x.size()<gridCols || y.size()<gridRows)
        return;
    imageCount++;

    if (imageType == 0)
    {
        flip(Mat(imageHeight,imageWidth,CV_8U,data),firstImg,0);
        temp = firstImg;
    }
    else if(imageType == 1)
    {
        temp = Mat(imageHeight,imageWidth,CV_16U,data);
        for (int i = 0; i < imageHeight; i++)
        {
            uchar *outdata = firstImg.ptr<uchar>(i);
            const uint16_t *indata = temp.ptr<uint16_t>(i);
            for (int j = 0; j < imageWidth; j++,outdata++,indata++){
                *outdata = (*indata)>>4;
            }
        }
    }
    //Mat(1944,2592,CV_8U,data).copyTo(firstImg);

    GetCenter(temp,addImageCenter);
    pushData(temp,addImageCenter);
}

void ImageAnalysis::pushData(Mat &img, Point offset){
    posValue.clear();
    posItem.clear();
    posIndex.clear();
    PosValue.clear();

    for (int j = 0; j < gridRows; j++)
    {
        for (int i = 0; i < gridCols; i++)
        {
            if (basePos[j][i].x != 0 && maskPos.at<uchar>(j,i) != 0){
                QVector<int> value(QVector<int>(7));
                value[5] = maskPos.at<uchar>(j,i);
                value[6] = j*gridCols+i;
                SpotCal(i,j,img, offset,value);
                posItem.push_back(maskPos.at<uchar>(j,i));
                posValue.push_back(value[4]);
                posIndex.push_back(j*gridCols+i);
                PosValue.push_back(value);
            }
        }
    }
}

void ImageAnalysis::subImageRemoveBound(Mat &subimg, int thrsh){
    bool haveBound = true;
    while(haveBound){
        vector<Vec4i> hi;
        vector<vector<Point>> contours;
        normalize(subimg,subimg,0,255,NORM_MINMAX);
        threshold(subimg,subimg,thrsh,255,THRESH_BINARY);
        Mat element = getStructuringElement(MORPH_ELLIPSE,Size(9,9));
        morphologyEx(subimg,subimg,MORPH_CLOSE,element);
        findContours(subimg,contours,hi,RETR_EXTERNAL,CHAIN_APPROX_NONE);
        haveBound = false;
        for (size_t z = 0; z < contours.size(); z++){
            for (size_t t = 0; t < contours[z].size(); t++)
            {
                if (contours[z][t].x == 0 || contours[z][t].x == subimg.cols - 1 || contours[z][t].y == 0 || contours[z][t].y == subimg.rows - 1)
                {
                    drawContours(subimg,contours,static_cast<int>(z),Scalar(0),-1);
                    haveBound = true;
                    break;
                }
            }
        }
        if (haveBound == false){

        }
    }
}

int ImageAnalysis::subImageHandle(bool debug, size_t posX, size_t posY, Mat &img, Mat &subimg,vector<vector<Point>> &contours,Point &centerPoint){
    vector<Vec4i> hi;
    int center = -1;
    int topsub = y[posY] - (subsize>>1);
    int bottom = y[posY] + (subsize>>1);
    int leftsub = x[posX] - (subsize>>1);
    int rightsub = x[posX] + (subsize>>1);
    if(topsub < 0 || bottom >=img.rows || leftsub < 0 || rightsub >= img.cols)
        return -2;

    img(Rect(leftsub,topsub,subsize+1,subsize+1)).copyTo(subimg);

    if (debug)
    {
        qDebug()<<"centerPos"<<centerPoint.x<<centerPoint.y;
        namedWindow("source Image",0);
        resizeWindow("source Image",subsize<<1,subsize<<1);
        imshow("source Image",img(Rect(leftsub,topsub,subsize+1,subsize+1))*3);

        namedWindow("source2",0);
        resizeWindow("source2",subsize<<1,subsize<<1);
        imshow("source2",subimg*3);
    }

    normalize(subimg,subimg,0,255,NORM_MINMAX);
    if (debug)
    {
        namedWindow("sub Image",0);
        resizeWindow("sub Image",subsize<<1,subsize<<1);
        imshow("sub Image",subimg);
    }    

    //threshold(subimg,subimg,thrsh,255,THRESH_BINARY);
    //adaptiveThreshold(subimg,subimg,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,25,0);
    adaptiveThreshold(subimg,subimg,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY,51,0);

    if (debug)
    {
        namedWindow("threshold Image",0);
        resizeWindow("threshold Image",subsize<<1,subsize<<1);
        imshow("threshold Image",subimg);
    }    

    //imclose
    Mat element = getStructuringElement(MORPH_ELLIPSE,Size(9,9));
    morphologyEx(subimg,subimg,MORPH_OPEN,element);
    morphologyEx(subimg,subimg,MORPH_CLOSE,element);
    //morphologyEx(subimg,subimg,MORPH_CLOSE,element);
    //morphologyEx(subimg,subimg,MORPH_OPEN,element);
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
    if (debug)
        qDebug()<<"findContours"<<contours.size();
    for (size_t z = 0; z < contours.size(); z++)
    {
        bool valid = true;
        for (size_t t = 0; t < contours[z].size(); t++)
        {
            if (contours[z][t].x == 0 || contours[z][t].x == subimg.cols - 1 || contours[z][t].y == 0 || contours[z][t].y == subimg.rows - 1)
            {
                valid = false;
                if (debug)
                    qDebug()<<z<<"out of range";
                //floodFill(subimg,Point(contours[z][t].x,contours[z][t].y),Scalar(0));
                break;
            }
        }
        if (valid){
            double s = contourArea(contours[z]);
            double l = arcLength(contours[z],true);
            if (debug)
                qDebug()<<"s"<<s<<"l"<<l<<4*3.14*s/l/l;
            if (s>1000 && s<10000 && (4*3.14*s/l/l)>0.65)
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
        if (center != -1){
            Mat showImg = img(Rect(leftsub,topsub,subsize+1,subsize+1))*3;
            drawContours(showImg,contours,center,Scalar(255),1);
            namedWindow("cImage",0);
            resizeWindow("cImage",subsize<<1,subsize<<1);
            imshow("cImage",showImg);

            Mat subImage1 = Mat::zeros(subsize+1,subsize+1,subimg.type());
            Mat subImage2 = Mat::zeros(subsize+1,subsize+1,subimg.type());
            Mat subImage3 = Mat::zeros(subsize+1,subsize+1,subimg.type());
            Mat element2 = getStructuringElement(MORPH_ELLIPSE,Size(md1+md2+1,md1+md2+1));
            Mat element3 = getStructuringElement(MORPH_ELLIPSE,Size(md2+1,md2+1));
            drawContours(subImage1,contours,center,Scalar(150),-1);

            //dilate(subImage1,subImage1,element2);
            dilate(subImage1,subImage2,element2);
            dilate(subImage2,subImage3,element3);
            subImage1 += subImage3 - subImage2;
            namedWindow("c2Image",0);
            resizeWindow("c2Image",subsize,subsize);
            imshow("c2Image",subImage1);
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

void ImageAnalysis::UpdateMask(int index, int top, int left, vector<vector<Point>> &contours, int center){
    Mat subImage1 = Mat::zeros(subsize+1,subsize+1,firstImg.type());
    Mat subImage2 = Mat::zeros(subsize+1,subsize+1,firstImg.type());
    Mat subImage3 = Mat::zeros(subsize+1,subsize+1,firstImg.type());
    Mat element2 = getStructuringElement(MORPH_ELLIPSE,Size(md1+md2+1,md1+md2+1));
    Mat element3 = getStructuringElement(MORPH_ELLIPSE,Size(md2+1,md2+1));

    drawContours(subImage1,contours,center,Scalar(index),-1);

    qDebug()<<"UpdateMask:"<<index<<"top="<<top<<"left="<<left<<contours[center].size()<<center;
    //dilate(subImage1,subImage1,element2);
    dilate(subImage1,subImage2,element2);
    dilate(subImage2,subImage3,element3);
    mask1(Rect(left,top,subsize+1,subsize+1)) += subImage1;

    Mat maskTemp = mask2(Rect(left,top,subsize+1,subsize+1));
    Mat subTemp = subImage2 - subImage1;
    Mat subTemp2 = (subImage3 - subImage2);
    for (int i = 0; i < maskTemp.rows; i++)
    {
        const uchar* indata = subTemp.ptr(i);
        const uchar* indata2 = subTemp2.ptr(i);
        uchar* outdata = maskTemp.ptr(i);
        if (indata == nullptr)
            break;
        for (int j = 0; j < maskTemp.cols; j++)
        {
            if (*indata != 0)
                *outdata = 255;
            else if(*indata2 != 0 && *outdata == 0)
                *outdata = *indata2+1;
            indata++;
            indata2++;
            outdata++;
        }
    }
    //mask2(Rect(left,top,subsize+1,subsize+1)) += subImage2 - subImage1;
    //mask2(Rect(left,top,subsize+1,subsize+1)) += (subImage3 - subImage2)*index;
}

void ImageAnalysis::EllipseMask(int index, int top, int left, vector<Point> &contour){
    Mat subImage1 = Mat::zeros(subsize+1,subsize+1,firstImg.type());
    Mat subImage2 = Mat::zeros(subsize+1,subsize+1,firstImg.type());
    Mat subImage3 = Mat::zeros(subsize+1,subsize+1,firstImg.type());
    Mat element2 = getStructuringElement(MORPH_ELLIPSE,Size(md1+11,md1+11));
    Mat element3 = getStructuringElement(MORPH_ELLIPSE,Size(md2+1,md2+1));

    RotatedRect box = fitEllipse(contour);
    ellipse(subImage1,box,Scalar(1),-1);

    dilate(subImage1,subImage1,element2);
    dilate(subImage1,subImage2,element3);
    dilate(subImage2,subImage3,element3);
    mask1(Rect(left,top,subsize+1,subsize+1)) += subImage1*index;
    mask2(Rect(left,top,subsize+1,subsize+1)) += subImage2 - subImage1;
    mask2(Rect(left,top,subsize+1,subsize+1)) += (subImage3 - subImage2)*index;
}

void ImageAnalysis::SpotMask(Mat &img, vector<int> &x, vector<int> &y, int subsize){
    int w = img.cols;
    int h = img.rows;

    Mat subImage = Mat::zeros(subsize+1,subsize+1,img.type());
    vector<vector<Point>> matPoint(y.size(),vector<Point>(x.size()));
    vector<vector<Point>> contourArr;
    QHash<int,int> contourIndex;
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
            int center = subImageHandle(false,j,i,img,subImage,contours,matPoint[i][j]);

            if (center >= 0)
            {
                contourArr.push_back(contours[center]);
                contourIndex[i*x.size()+j] = contourArr.size()-1;
            }
                //UpdateMask(i*x.size()+j,topsub, leftsub,contours,center);
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
        qDebug()<<"tempPointCount"<<tempPointCount<<"pointCount"<<pointCount;
        if (tempPointCount > pointCount)
        {
            pointCount = tempPointCount;
            topPointy = static_cast<int>(z);
        }
    }
    qDebug()<<"topPointx:"<<topPointx<<"topPointy:"<<topPointy;
    qDebug()<<"contourIndex"<<contourIndex;

    for (size_t i = topPointy; i < topPointy+gridRows; i++)
    {
        for (size_t j = topPointx; j < topPointx+gridCols; j++)
        {
            basePos[i-topPointy][j-topPointx].x = matPoint[i][j].x;
            basePos[i-topPointy][j-topPointx].y = matPoint[i][j].y;
            qDebug()<<"basePos["<<i-topPointy<<"]["<<j-topPointx<<"] = "<<"("<<basePos[i-topPointy][j-topPointx].x<<","<<basePos[i-topPointy][j-topPointx].y<<")";

            if (maskPos.at<uchar>(i-topPointy,j-topPointx) != 0){
                int topsub = y[i] - (subsize>>1);
                int leftsub = x[j] - (subsize>>1);
                /*
                if (basePos[i-topPointy][j-topPointx].x == 0){
                    vector<vector<Point>> contours;
                    int center = -1;

                    if (center == -1){
                        subImageHandle(false,j,i,img,subImage,contours,basePos[i-topPointy][j-topPointx]);
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
                            EllipseMask(i*x.size()+j,topsub,leftsub,contours[center]);
                        }
                    }
                }
                //*/

                if (basePos[i-topPointy][j-topPointx].x != 0){
                    int nMaskIndex = (i-topPointy)*gridCols + j-topPointx;
                    nMaskIndex = (nMaskIndex+1)*2;
                    UpdateMask(nMaskIndex,topsub,leftsub,contourArr,contourIndex[i*x.size()+j]);
                }
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

int ImageAnalysis::SpotCal(int PosX, int PosY, Mat &img, Point& offset, QVector<int> &value){
    int topsub = y[PosY+topPointy] - (subsize>>1);
    int bottom = y[PosY+topPointy] + (subsize>>1);
    int leftsub = x[PosX+topPointx] - (subsize>>1);
    int rightsub = x[PosX+topPointx] + (subsize>>1);

    if (PosY+topPointy == debugy && PosX+topPointx == debugx)
        qDebug()<<"SpotCal,(x,y)=("<<PosX<<","<<PosY<<") basePos:("<<basePos[PosY][PosX].x<<","<<basePos[PosY][PosX].y<<")\ttop:"<<topsub<<",left:"<<leftsub<<",bottom:"<<bottom<<",right:"<<rightsub;
    if (topsub < 0 || bottom >= firstImg.rows || leftsub < 0 || rightsub >= firstImg.cols)
    {
        qDebug()<<"topsub="<<topsub<<",bottom="<<bottom<<",leftsub="<<leftsub<<"rightsub="<<rightsub;
        return 0;
    }

    Mat subMask1 = mask1(Rect(leftsub,topsub,subsize+1,subsize+1));
    Mat subMask2 = mask2(Rect(leftsub,topsub,subsize+1,subsize+1));
    leftsub = leftsub - firstCenter.x + offset.x;
    if (leftsub < 0) leftsub = 0;
    rightsub = rightsub - firstCenter.x + offset.x;
    if (rightsub >= firstImg.cols)
        leftsub = leftsub - (rightsub-firstImg.cols+1);
    topsub = topsub - firstCenter.y + offset.y;
    if (topsub < 0) topsub = 0;
    bottom = bottom - firstCenter.y + offset.y;
    if (bottom >= firstImg.rows)
        topsub = topsub - (bottom-firstImg.rows+1);
    Mat subSource = img(Rect(leftsub,topsub,subsize+1,subsize+1));
/*
    if (PosY*gridRows + PosX == 57 && (imageCount > 10 && imageCount < 15))
    {
        QString winName = "subSource"+QString::number(imageCount);
        namedWindow(winName.toStdString(),0);
        resizeWindow(winName.toStdString(),subSource.cols<<1,subSource.rows<<1);
        //imshow(winName.toStdString(),subSource*3);
        imshow(winName.toStdString(),firstImg*3);
        qDebug()<<winName;
    }
    //*/
    return GetPosValue((PosY*gridCols + PosX + 1)*2,subSource,subMask1,subMask2,value);
}

void ImageAnalysis::SetDebugPos(size_t x, size_t y)
{
    debugx = x;
    debugy = y;
    Mat subImage = Mat::zeros(subsize+1,subsize+1,firstImg.type());
    vector<vector<Point>> contours;
    Point centerPoint;
    subImageHandle(true,x,y,firstImg,subImage,contours,centerPoint);
}

double ImageAnalysis::GetDefinition(void *data, int imageType){
    QTime time;
    time.start();

    double meanValue = 0.0;
    Mat imageSobel;
    Mat imageGrey;
    if (imageType == 0){
        Mat(imageHeight,imageWidth,CV_8U,data)(Rect(ExGlobal::FocusX,ExGlobal::FocusY,ExGlobal::FocusWidth,ExGlobal::FocusHeight)).copyTo(imageGrey);
        Sobel(imageGrey,imageSobel,CV_8U,1,1);
    }
    else if(imageType == 1){
        Mat(imageHeight,imageWidth,CV_16U,data)(Rect(ExGlobal::FocusX,ExGlobal::FocusY,ExGlobal::FocusWidth,ExGlobal::FocusHeight)).copyTo(imageGrey);
        Sobel(imageGrey,imageSobel,CV_16U,1,1);
    }

    meanValue = mean(imageSobel)[0];
    qDebug()<<"GetDefinition,elapsed="<<time.elapsed();
    return meanValue;
}

double ImageAnalysis::GetDefinition2(void *data, int imageType){
    QTime time;
    time.start();

    double meanValue = 0.0;
    Mat imageSobel;
    Mat imageGrey;
    if (imageType == 0){
        Mat(imageHeight,imageWidth,CV_8U,data)(Rect(ExGlobal::FocusX,ExGlobal::FocusY,ExGlobal::FocusWidth,ExGlobal::FocusHeight)).copyTo(imageGrey);
        Laplacian(imageGrey,imageSobel,CV_8U);
    }
    else if(imageType == 1){
        Mat(imageHeight,imageWidth,CV_16U,data)(Rect(ExGlobal::FocusX,ExGlobal::FocusY,ExGlobal::FocusWidth,ExGlobal::FocusHeight)).copyTo(imageGrey);
        Laplacian(imageGrey,imageSobel,CV_16U);
    }
    meanValue = mean(imageSobel)[0];
    qDebug()<<"GetDefinition2,elapsed="<<time.elapsed();
    return meanValue;
}

double ImageAnalysis::GetDefinition3(void *data, int imageType){
    QTime time;
    time.start();

    double meanValue = 0.0;
    Mat imageSobel;
    Mat imageGrey;
    Mat imageMeanStdValue;
    if (imageType == 0){
        Mat(imageHeight,imageWidth,CV_8U,data)(Rect(ExGlobal::FocusX,ExGlobal::FocusY,ExGlobal::FocusWidth,ExGlobal::FocusHeight)).copyTo(imageGrey);
    }
    else if(imageType == 1){
        Mat(imageHeight,imageWidth,CV_16U,data)(Rect(ExGlobal::FocusX,ExGlobal::FocusY,ExGlobal::FocusWidth,ExGlobal::FocusHeight)).copyTo(imageGrey);
    }

    meanStdDev(imageGrey,imageSobel,imageMeanStdValue);
    meanValue = imageMeanStdValue.at<double>(0,0);
    qDebug()<<"GetDefinition3,elapsed="<<time.elapsed();
    return meanValue*meanValue;
}

double ImageAnalysis::GetMeanLight(void *data, int imageType){
    QTime time;
    time.start();
    double meanValue = 0.0;
    Mat imageGrey;
    if (imageType == 0){
        Mat(imageHeight,imageWidth,CV_8U,data)(Rect(ExGlobal::FocusX,ExGlobal::FocusY,ExGlobal::FocusWidth,ExGlobal::FocusHeight)).copyTo(imageGrey);
    }
    else if(imageType == 1){
        Mat(imageHeight,imageWidth,CV_16U,data)(Rect(ExGlobal::FocusX,ExGlobal::FocusY,ExGlobal::FocusWidth,ExGlobal::FocusHeight)).copyTo(imageGrey);
    }
    meanValue = mean(imageGrey)[0];
    qDebug()<<"GetMeanLight,elapsed="<<time.elapsed();
    return meanValue;
}

double ImageAnalysis::GetCircularSize(void *data, int imageType){
    QTime time;
    time.start();
    int x = ExGlobal::FocusX;
    int y = ExGlobal::FocusY;
    int w = ExGlobal::FocusWidth;
    int h = ExGlobal::FocusHeight;
    double result = 0;
    Mat imageGrey = Mat::zeros(h,w,CV_8U);
    if (imageType == 0)
        Mat(imageHeight,imageWidth,CV_8U,data)(Rect(x,y,w,h)).copyTo(imageGrey);
    else if(imageType == 1)
    {
        Mat temp = Mat(imageHeight,imageWidth,CV_16U,data)(Rect(x,y,w,h));
        for (int i = 0; i < h; i++)
        {
            uchar *outdata = imageGrey.ptr<uchar>(i);
            const uint16_t *indata = temp.ptr<uint16_t>(i);
            for (int j = 0; j < w; j++,outdata++,indata++){
                *outdata = ((*indata)>>4)&0xFF;
            }
        }
    }
    GaussianBlur(imageGrey, imageGrey, Size(25,25),0);
    normalize(imageGrey,imageGrey,0,255,NORM_MINMAX);
    adaptiveThreshold(imageGrey,imageGrey,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY,51,0);

    vector<Vec4i> hi;
    vector<vector<Point>> contours;
    findContours(imageGrey,contours,hi,RETR_EXTERNAL,CHAIN_APPROX_NONE);
    double Sm = 0;
    int Ss = 0;
    for (size_t z = 0; z < contours.size(); z++)
    {
        double s = contourArea(contours[z]);
        if (s>2000)
        {
            Sm += s;
            Ss++;
        }
    }
    qDebug()<<"GetCircularSize,elapsed="<<time.elapsed()<<"Sm="<<Sm<<"Ss="<<Ss;
    if (Ss > 3)
        result = Sm/Ss;
    return -result;
}
