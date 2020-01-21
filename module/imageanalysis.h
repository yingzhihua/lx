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
    void FirstImage(void *data, int imageType);
    void AddImage(void *data, int imageType);

    QImage getMainImg(int type,int light);
    void SetMask(void *data, int maskType);    
    void SetDebugPos(size_t x, size_t y);
    QVector<int> getItem(){return posItem;}
    QVector<int> getValue(){return posValue;}
    QVector<int> getIndex(){return posIndex;}
    int getImageCount(){return imageCount;}

    int thrsh = 170;
private:
    int subsize = 160;
    size_t debugx = 9;
    size_t debugy = 8;
    int imageCount = 0;
    int mpp = 100;
    int md1 = 10;
    int md2 = 20;
    int gridRows = 11;
    int gridCols = 11;
    int topPointx = 0;
    int topPointy = 0;
    Mat firstImg,mask1,mask2,maskPos;
    //Point **basePos;
    Point basePos[11][11];
    Point firstCenter;

    vector<int> x,y;
    QVector<int> posItem,posValue,posIndex;

    void SpotMask(Mat &img, Mat &mask1, Mat &mask2,std::vector<int>& x,std::vector<int>& y,int subsize,int d1, int d2);
    void FindPeaks(int *number, int numberLen, int mpp, std::vector<int>& pos);
    void FindGrid(Mat img, int mpp, std::vector<int>& x, std::vector<int>& y);
    int SpotCal(int x, int y);
    int SpotCal(int x, int y, Point& offset);
    int subImageHandle(bool debug, size_t x, size_t y, Mat &img, Mat &subimg, int thrsh,vector<vector<Point>> &contours,Point &centerPoint);
    void UpdateMask(int top, int left, vector<vector<Point>> &contours, int center);
    void EllipseMask(int top, int left, vector<Point> &contour);

    int refPoint = -1;
    int refValue,refValue0;
signals:

public slots:
};

#endif // IMAGEANALYSIS_H
