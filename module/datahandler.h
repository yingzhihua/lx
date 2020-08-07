#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QObject>
#include "opencv2/opencv.hpp"
#include "opencv2/objdetect.hpp"

using namespace cv;
using namespace std;
class DataHandler
{
public:
    DataHandler();
    static bool HandleData(int testId, QHash<int,vector<Point>> &posArr);
    static void setBaseDataIndex(int min, int max);
    static void FillTestData(int Testid, QHash<int,vector<Point>> &posArr);
    static void BaseLineFit(QHash<int, vector<Point>> &posArr);
    static void RefPosFit(QHash<int, vector<Point>> &posArr);
    static QHash<int,int> getPosItemid();

private:    
    static void drawPosArr(cv::String winName, QHash<int, vector<Point>> posArr);
};

#endif // DATAHANDLER_H
