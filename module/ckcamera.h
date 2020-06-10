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
    bool preview();
    void *getyData(){return rawData;}

    bool setabsExpose(int value);
    bool setGain(int value);
    bool setWhite(int value);

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
    uint8_t* rawData;
    unsigned int *sum;
    HANDLE m_hCamera;
    uint32_t CKGetFrame();
    int process_image(int index, uint8_t *data, uint32_t datalength);
    void saveRaw(uint8_t *data, uint32_t datalength);    
    bool waitStop();
};

#endif // CKCAMERA_H
