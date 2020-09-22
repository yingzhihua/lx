#ifndef IMAGEANALYSIS_H
#define IMAGEANALYSIS_H

#include <QObject>
#include "opencv2/opencv.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"
#include <QVector>

using namespace cv;
using namespace std;

class ImageAnalysis : public QObject
{
    Q_OBJECT
public:
    explicit ImageAnalysis(QObject *parent = nullptr);
    static QString QRDecode(QImage img);
    static double GetDefinition(void *data, int imageType);
    static double GetDefinition2(void *data, int imageType);
    static double GetDefinition3(void *data, int imageType);
    static double GetMeanLight(void *data, int imageType);
    static double GetCircularSize(void *data, int imageType);
    void FirstImage(void *data, int imageType);
    void AddImage(void *data, int imageType);

    QImage getMainImg(int type,int light);
    QImage getMaskImg();
    void SetMask(void *data, int maskType);    
    void SetDebugPos(size_t x, size_t y);
    QVector<int> getItem(){return posItem;}
    QVector<int> getValue(){return posValue;}
    QVector<int> getIndex(){return posIndex;}
    QVector<QVector<int>> getPosValueArr(){return PosValue;}
    int getImageCount(){return imageCount;}
    void paramSet(int posWidth, int posSpace, int bgWidth, int bgSpace){mpp = posWidth;subsize=posSpace;md2=bgWidth;md1=bgSpace;}
    void imageSet(int width, int height){imageWidth = width;imageHeight=height;}
    void setCalWidth(int width){calWidth = width;}
    int thrsh = 170;
private:
    int subsize = 140;
    size_t debugx = 9;
    size_t debugy = 8;
    int imageCount = 0;
    int mpp = 80;
    int md1 = 10;
    int md2 = 12;
    int gridRows = 11;
    int gridCols = 11;
    int topPointx = 0;
    int topPointy = 0;
    static int imageWidth;
    static int imageHeight;
    static int calWidth;
    Mat firstImg,mask1,mask2,maskPos;
    //Point **basePos;
    Point basePos[11][11];
    Point firstCenter;

    vector<int> x,y;
    QVector<int> posItem,posValue,posIndex;
    QVector<QVector<int>> PosValue;

    void SpotMask(Mat &img,std::vector<int>& x,std::vector<int>& y,int subsize);
    void FindPeaks(int *number, int numberLen, int mpp, std::vector<int>& pos);
    void FindGrid(Mat img, int mpp, std::vector<int>& x, std::vector<int>& y);    
    int SpotCal(int x, int y, Mat &img, Point& offset, QVector<int> &value);
    void pushData(Mat &img, Point offset);
    int subImageHandle(bool debug, size_t x, size_t y, Mat &img, Mat &subimg,vector<vector<Point>> &contours,Point &centerPoint);
    void subImageRemoveBound(Mat &subimg, int thrsh);
    void UpdateMask(int index, int top, int left, vector<vector<Point>> &contours, int center);
    void EllipseMask(int index, int top, int left, vector<Point> &contour);

signals:

public slots:
};

#endif // IMAGEANALYSIS_H
