#ifndef CVCAPTURE_H
#define CVCAPTURE_H

#include <QThread>
#include <QImage>

#include "opencv2/videoio.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;
using namespace std;
class cvCapture : public QThread
{
    Q_OBJECT
public:
    explicit cvCapture(QObject *parent = nullptr);
    void setCurrCamera(int id){cameraid = id;}
    void setImageCount(int nCount){imageCount = nCount;}
    bool Running();

protected:
    void run();

private:
    int cameraid;
    int imageCount;
    VideoCapture cap;

signals:
    void captureNotify(QByteArray result);
};

#endif // CVCAPTURE_H
