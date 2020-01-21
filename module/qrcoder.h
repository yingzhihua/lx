#ifndef QRCODER_H
#define QRCODER_H

#include <QThread>
#include <QImage>

#include "opencv2/videoio.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"

//#include "zbar.h"

using namespace cv;
using namespace std;
//using namespace zbar;

class QRcoder : public QThread
{
    Q_OBJECT
public:
    explicit QRcoder(QObject *parent = nullptr);
    static QImage img;
    static QImage img2;
    bool handleimage;
    bool scale;
    int binValue = 120;
    int poxValue = 200;

protected:
    void run();

signals:
    void finishQRcode(QByteArray result);

public slots:

};

#endif // QRCODER_H
