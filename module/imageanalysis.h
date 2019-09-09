#ifndef IMAGEANALYSIS_H
#define IMAGEANALYSIS_H

#include <QObject>
#include "opencv2/opencv.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;
using namespace std;

class ImageAnalysis : public QObject
{
    Q_OBJECT
public:
    explicit ImageAnalysis(QObject *parent = nullptr);
    static QString QRDecode(QString img);
    void FirstImage(void *data, int imageType);
    void AddImage(void *data, int imageType);
    QImage getMainImg(int type);
    void SetDebugPos(size_t x, size_t y){debugx = x;debugy = y;}
private:
    int subsize = 160;
    size_t debugx = 9;
    size_t debugy = 8;
    int imageCount = 0;
    int mpp = 120;
    int md1 = 10;
    int md2 = 10;
    int gridRows = 11;
    int gridCols = 11;
    Mat firstImg,mask1,mask2;
    vector<int> x,y;
    void SpotMask(Mat img, Mat &mask1, Mat &mask2,std::vector<int>& x,std::vector<int>& y,int subsize,int d1, int d2);
    void FindPeaks(int *number, int numberLen, int mpp, std::vector<int>& pos);
    void FindGrid(Mat img, int mpp, std::vector<int>& x, std::vector<int>& y);

signals:

public slots:
};

#endif // IMAGEANALYSIS_H
