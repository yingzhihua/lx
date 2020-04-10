#ifndef IMAGECAPTURE_H
#define IMAGECAPTURE_H

#include <QThread>
#include <QImage>

#include "DVPCamera.h"

struct buffer
{
    void * start;
    size_t length;
};

typedef enum{
    IO_METHOD_READ,IO_METHOD_MMAP,
} io_method;

typedef enum{
    CAMERA_V4L2,CAMERA_DVP,CAMERA_CK,
} CAMERA_TYPE;

class ImageCapture : public QThread
{
    Q_OBJECT
public:
    enum class CaptureMode{
        Idle,
        View,
        Capture
    };
    Q_ENUM(CaptureMode)
    explicit ImageCapture(QObject *parent = nullptr);
    int start_capturing(CaptureMode mode);
    int stop_capturing();
    void setFileName(QString fileName){filename = fileName;}
    void setImageCount(int nCount){count = nCount;}    
    int getabsExpose();
    bool setabsExpose(int value);
    int getGain();
    bool setGain(int value);
    int getWhite();
    bool setWhite(int value);
    void *getyData(){return bufy;}
    bool waitStop();
    double getDefinition();
    double getDefinition2();
    int imagetype;

protected:
    void run();

signals:
    void finishCapture(QByteArray result);
    void reView(QImage img);

private:
    int open_device();    
    void close_device();    
    int init_device();    
    int init_read(unsigned int buffer_size);
    int init_mmap();
    int uninit_device();    
    int read_frame(int index);
    int clear_frame();
    int process_image(int index, const void *p, int size);
    io_method io;    
    CAMERA_TYPE cameraType;

    int fd;
    int count;
    buffer *buffers;
    unsigned int n_buffers;
    //unsigned char *bufrgb;
    unsigned char *bufy;
    unsigned short *buf2y;
    unsigned int *sum;
    QString filename;
    CaptureMode captureMode;    
    bool stopping;
    int internal_stop_capturing();
    void recordParam();
#if 1
    int dvp_open_device();
    int dvp_close_device();
    int dvp_init_device();
    int dvp_uninit_device();
    dvpHandle dvp_handle;
    QString m_FriendlyName;
    bool IsValidHandle(dvpHandle handle)
    {
        bool bValidHandle;
        dvpIsValid(handle, &bValidHandle);
        return bValidHandle;
    }
#endif
};

#endif // IMAGECAPTURE_H
