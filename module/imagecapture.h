#ifndef IMAGECAPTURE_H
#define IMAGECAPTURE_H

#include <QThread>
#include <QImage>

struct buffer
{
    void * start;
    size_t length;
};

typedef enum{
    IO_METHOD_READ,IO_METHOD_MMAP,
} io_method;

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
    bool Running();

protected:
    void run();

signals:
    void finishCapture(QByteArray result);
    void reView(QImage img);

private:
    int open_device();
    int init_device();
    int init_read(unsigned int buffer_size);
    int init_mmap();
    int read_frame(int index);
    int clear_frame();
    int process_image(int index, const void *p, int size);
    io_method io;    
    int fd;
    int count;
    buffer *buffers;
    unsigned int n_buffers;
    unsigned char *bufrgb;
    unsigned char *bufy;
    unsigned short *buf2y;
    unsigned int *sum;
    QString filename;
    CaptureMode captureMode;    
};

#endif // IMAGECAPTURE_H
