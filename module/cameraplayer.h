#ifndef CAMERAPLAYER_H
#define CAMERAPLAYER_H
#include <QThread>
#include <QImage>

#include "DVPCamera.h"
#include "CKCameraInterface.h"

typedef enum{
    CAMERA_TYPE_V4L2,CAMERA_TYPE_DVP,CAMERA_TYPE_CK,CAMERA_EMPTY
} CAMERATYPE;

class CameraPlayer : public QThread
{
    Q_OBJECT
public:
    enum class CaptureMode{
        Idle,
        View,
        Capture
    };
    Q_ENUM(CaptureMode)

    explicit CameraPlayer(QObject *parent = nullptr);
    bool CameraStart(CaptureMode mode);
    bool CameraStop();
    void setFileName(QString fileName){filename = fileName;}
    void setImageCount(int nCount){count = nCount;}
    bool waitStop();

protected:
    void run();

private:
    CAMERATYPE cameraType;
    QString cameraName;
    QString filename;
    int count;
    CaptureMode captureMode;
    uint8_t* bufy;
    uint8_t* rawData;
    bool stoping;
    int process_image(int index, uint8_t *data, int datalength);

    //CKCamera
    HANDLE m_hCamera;    
    bool CKReadCamera();
    bool CKOpenCamera();
    bool CKCloseCamera();
    bool CKGetFrame(int index);

    //DVPCamera
    dvpHandle dvp_handle;    
    bool IsValidHandle(dvpHandle handle)
    {
        bool bValidHandle;
        dvpIsValid(handle, &bValidHandle);
        return bValidHandle;
    }
    bool DVPReadCamera();
    bool DVPOpenCamera();
    bool DVPInitCamera();
    bool DVPCloseCamera();
    bool DVPGetFrame(int index);
};

#endif // CAMERAPLAYER_H
