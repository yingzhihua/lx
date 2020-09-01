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
    static bool SaveData(int testId);    
    static QHash<int,int> getPosItemid();
    static void setAssayItem(QMap<int, QString> assayItem);
};

#endif // DATAHANDLER_H
