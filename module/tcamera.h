#ifndef TCAMERA_H
#define TCAMERA_H

#include <QObject>

#include "imagecapture.h"
#include "ckcamera.h"

typedef enum{
    CAMERA_TYPE_V4L2,CAMERA_TYPE_DVP,CAMERA_TYPE_CK,CAMERA_EMPTY
} CAMERATYPE;

class TCamera : public QObject
{
    Q_OBJECT
public:
    explicit TCamera(QObject *parent = nullptr);
    int openCamera();
    int closeCamera();
    int stopCamera();
    bool capture(QString fileName, int nCount);
    bool preview();
    void *getyData();
    int getImageType();

    int getabsExpose();
    bool setabsExpose(int value);
    int getGain();
    bool setGain(int value);
    int getWhite();
    bool setWhite(int value);
    double getDefinition();
    double getDefinition2();
    bool saveRaw(QString filename);

    int imagetype;    

signals:
    void finishCapture(QByteArray result);
    void reView(QImage img);

public slots:
    void ActionFinish(QByteArray data){emit finishCapture(data);}
    void CameraView(QImage img){emit reView(img);}

private:
    CAMERATYPE cameraType;
    ImageCapture *imageCapture;
    CKCamera *ckCamera;
};

#endif // TCAMERA_H
