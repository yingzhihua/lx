#ifndef CKCAMERA_H
#define CKCAMERA_H

#include <QThread>
#include <QImage>

#include "CKCameraInterface.h"

class CKCamera : public QThread
{
    Q_OBJECT
public:
    enum class CaptureMode{
        Idle,
        View,
        Capture
    };
    Q_ENUM(CaptureMode)
    explicit CKCamera(QObject *parent = nullptr);
    static bool readCamera();
    bool openCamera();
    bool closeCamera();
    bool stopCamera();
    bool capture(QString fileName, int nCount);
    bool preview(int viewType = 0);
    void *getyData();
    int getImageType(){return rawImageType;}

    bool setabsExpose(int value);
    bool setGain(int value);
    bool setWhite(int value);
    bool saveRaw(QString fileName);

    int imagetype;
    CaptureMode captureMode;
signals:
    void finishCapture(QByteArray result);
    void reView(QImage img);

protected:
    void run();

public slots:

private:
    QString filename;
    int count;
    bool stopping;
    int ViewType;
    uint8_t* rawData;
    uint8_t* wtobRawData;
    uint16_t* wRawData;
    int rawImageType;
    unsigned int *sum;
    HANDLE m_hCamera;
    uint32_t CKGetFrame();
    int process_image(int index, uint32_t datalength);
    void saveRaw(void *data, uint32_t datalength);
    bool waitStop();
};

#endif // CKCAMERA_H
