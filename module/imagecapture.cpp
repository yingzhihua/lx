#include "imagecapture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <QDebug>
#include <QImage>
#include <QDir>
#include <QMutex>

#include <linux/videodev2.h>
#include "log.h"
#include "exglobal.h"

#define CLEAR(x) memset (&(x),0,sizeof(x))
#define CAPTURE_COUNT 1
#define device_name "/dev/video0"

static bool camerainited = false;
static QByteArray resultData;
static QMutex qmutex;
static int xioctl(int fd, unsigned long request, void * arg){
    int r;
    do{
        r = ioctl(fd, request, arg);
    } while(-1 == r && EINTR == errno);
    return r;
}

static int convert_yuv_to_rgb_pixel(int y, int u, int v)
{
    unsigned int pixel32 = 0;
    unsigned char *pixel = (unsigned char *)&pixel32;
    int r, g, b;
    r = y + (1.370705 * (v-128));
    g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
    b = y + (1.732446 * (u-128));
    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;
    if(r < 0) r = 0;
    if(g < 0) g = 0;
    if(b < 0) b = 0;
    pixel[0] = r * 220 / 256;
    pixel[1] = g * 220 / 256;
    pixel[2] = b * 220 / 256;
    return pixel32;
}

static int convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
    unsigned int in, out = 0;
    unsigned int pixel_16;
    unsigned char pixel_24[3];
    unsigned int pixel32;
    int y0, u, y1, v;
    for(in = 0; in < width * height * 2; in += 4) {
        pixel_16 = yuv[in + 3] << 24 |
                   yuv[in + 2] << 16 |
                   yuv[in + 1] <<  8 |
                   yuv[in + 0];
        y0 = (pixel_16 & 0x000000ff);
        u  = (pixel_16 & 0x0000ff00) >>  8;
        y1 = (pixel_16 & 0x00ff0000) >> 16;
        v  = (pixel_16 & 0xff000000) >> 24;
        pixel32 = convert_yuv_to_rgb_pixel(y0, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
        pixel32 = convert_yuv_to_rgb_pixel(y1, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
    }
    return 0;
}

static int convert_yuv_to_rgb(unsigned int *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
    unsigned int in, out = 0;
    unsigned int pixel_16;
    unsigned char pixel_24[3];
    unsigned int pixel32;
    int y0, u, y1, v;
    for(in = 0; in < width * height * 2; in += 4) {
        pixel_16 = yuv[in + 3] << 24 |
                   yuv[in + 2] << 16 |
                   yuv[in + 1] <<  8 |
                   yuv[in + 0];
        y0 = (pixel_16 & 0x000000ff);
        u  = (pixel_16 & 0x0000ff00) >>  8;
        y1 = (pixel_16 & 0x00ff0000) >> 16;
        v  = (pixel_16 & 0xff000000) >> 24;
        pixel32 = convert_yuv_to_rgb_pixel(y0, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
        pixel32 = convert_yuv_to_rgb_pixel(y1, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
    }
    return 0;
}
static int convert_yuv_to_y_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
    unsigned int in, out = 0;
    for(in = 0; in < width * height; in++) {
        rgb[out++] = yuv[(in<<1)];
    }
    return 0;
}

static int convert_yuv_to_2y_buffer(unsigned char *yuv, unsigned short *rgb, unsigned int width, unsigned int height)
{
    unsigned int in, out = 0;
    for(in = 0; in < width * height; in++) {
        rgb[out++] = yuv[(in<<1)]*256;
    }
    return 0;
}

ImageCapture::ImageCapture(QObject *parent) : QThread(parent),io(IO_METHOD_MMAP),count(1)
{    
    filename = "undefine";
//return;
    if (open_device() != 0)
        return;

    if (init_device() != 0)
        return;

    camerainited = true;
    captureMode = CaptureMode::Idle;
}

int ImageCapture::open_device(){
    struct stat st;

    if (-1==stat(device_name,&st))
    {
        qDebug()<<"Cannot identify dev,errno:"<<errno;
        Log::LogCam(QString("open_device,Cannot identify dev,errno:%1").arg(errno));
        return -1;
    }
    if (!S_ISCHR(st.st_mode))
    {
        qDebug()<<"dev is no devive, return";
        Log::LogCam("open_device,dev is no devive, return -1");
        return -1;
    }
    fd = open(device_name,O_RDWR|O_NONBLOCK,0);
    if (-1==fd)
    {
        qDebug()<<"Cannot open video,errno:"<<errno;
        Log::LogCam(QString("open_device,Cannot open video,errno:%1").arg(errno));
        return -1;
    }
    return 0;
}

int ImageCapture::init_device(){
    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;
    struct v4l2_streamparm streamparam;
    unsigned int min;

    if (-1 == xioctl(fd,VIDIOC_QUERYCAP,&cap))
    {
        qDebug()<<"QUERYCAP err:"<<errno;
        Log::LogCam(QString("init_device,QUERYCAP,errno:%1").arg(errno));
        return -1;
    }
    if (!(cap.capabilities&V4L2_CAP_VIDEO_CAPTURE))
    {
        qDebug()<<"dev is no video capture device, return";
        Log::LogCam("open_device,dev is no video capture device, return -1");
        return -1;
    }
    if (!(cap.capabilities&V4L2_CAP_READWRITE))
        qDebug()<<"dev does not support read io";
    else
        io = IO_METHOD_READ;

    if (!(cap.capabilities&V4L2_CAP_STREAMING))
    {
        qDebug()<<"dev does not support streaming io, return";
        Log::LogCam("open_device,dev does not support streaming io, return -1");
        return -1;
    }
    //set frame rat
#if 0
    CLEAR(streamparam);
    streamparam.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(fd,VIDIOC_G_PARM,&streamparam)){
        qDebug()<<"VIDIOC_G_PARM error, return";
        Log::LogCam("init_device,VIDIOC_G_PARM error, return -1");
        return -1;
    }
    else {
        Log::LogCam(QString("init_device,Get frame:%1/%2,capalibility=%3,capturemode=%4").arg(streamparam.parm.capture.timeperframe.numerator).arg(streamparam.parm.capture.timeperframe.denominator)
                    .arg(streamparam.parm.capture.capability).arg(streamparam.parm.capture.capturemode));
        qDebug()<<"Get frame "<<streamparam.parm.capture.timeperframe.numerator<<"/"<<streamparam.parm.capture.timeperframe.denominator;
    }

    CLEAR(streamparam);
    streamparam.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    streamparam.parm.capture.capturemode = 1;
    streamparam.parm.capture.timeperframe.numerator = 6;
    streamparam.parm.capture.timeperframe.denominator = 3;
    if (-1 == xioctl(fd,VIDIOC_S_PARM,&streamparam)){
        qDebug()<<"VIDIOC_S_PARM error, return";
        Log::LogCam("init_device,VIDIOC_S_PARM error, return -1");
        return -1;
    }

    CLEAR(streamparam);
    streamparam.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(fd,VIDIOC_G_PARM,&streamparam)){
        qDebug()<<"VIDIOC_G_PARM error, return";
        Log::LogCam("init_device,VIDIOC_G_PARM error, return -1");
        return -1;
    }
    else {
        Log::LogCam(QString("init_device,after Get frame:%1/%2,capalibility=%3,capturemode=%4").arg(streamparam.parm.capture.timeperframe.numerator).arg(streamparam.parm.capture.timeperframe.denominator)
                    .arg(streamparam.parm.capture.capability).arg(streamparam.parm.capture.capturemode));
        qDebug()<<"after set param,Get frame "<<streamparam.parm.capture.timeperframe.numerator<<"/"<<streamparam.parm.capture.timeperframe.denominator;
    }
#endif

    CLEAR(cropcap);
    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (0==xioctl(fd,VIDIOC_CROPCAP,&cropcap)){
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c = cropcap.defrect;
        if (-1==xioctl(fd,VIDIOC_S_CROP,&crop)){
            switch (errno) {
            case EINVAL:
                qDebug()<<"Cropping not supported";
                break;
            default:
                qDebug()<<"VIDIOC_S_CROP Errors ignored";
                break;
            }
        }
    }else {
        qDebug()<<"VIDIOC_CROPCAP Errors ignored";
    }

    CLEAR(fmt);
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = cropcap.defrect.width;
    fmt.fmt.pix.height = cropcap.defrect.height;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    if (-1 == xioctl(fd,VIDIOC_S_FMT,&fmt))
    {
        qDebug()<<"VIDIOC_S_FMT error, return";
        Log::LogCam("init_device,VIDIOC_S_FMT error, return -1");
        return -1;
    }
    qDebug()<<"bytesperline:"<<fmt.fmt.pix.bytesperline<<"height:"<<fmt.fmt.pix.height<<"width:"<<fmt.fmt.pix.width;
    min = fmt.fmt.pix.width*2;
    if (fmt.fmt.pix.bytesperline<min)
        fmt.fmt.pix.bytesperline = min;
    min = fmt.fmt.pix.bytesperline*fmt.fmt.pix.height;
    if (fmt.fmt.pix.sizeimage < min)
        fmt.fmt.pix.sizeimage = min;
#if 1
    struct v4l2_control ctrl;
    struct v4l2_queryctrl setting;

    ctrl.id = V4L2_CID_EXPOSURE_AUTO;
    ctrl.value = V4L2_EXPOSURE_MANUAL;
    if (-1 == xioctl(fd,VIDIOC_S_CTRL,&ctrl))
    {
        qDebug()<<"VIDIOC_S_CTRL error";
        Log::LogCam("init_device,VIDIOC_S_CTRL error");
    }

    ctrl.id = V4L2_CID_EXPOSURE_AUTO;
    if (-1 == xioctl(fd,VIDIOC_G_CTRL,&ctrl))
    {
        qDebug()<<"VIDIOC_G_CTRL error";
        Log::LogCam("init_device,VIDIOC_G_CTRL error");
    }
    else {
        Log::LogCam(QString("init_device,Get Exposure auto type:%1").arg(ctrl.value));
        qDebug()<<"Get Exposure auto type:"<<ctrl.value;
    }

#if 0
    ctrl.id = V4L2_CID_EXPOSURE;
    if (-1 == xioctl(fd,VIDIOC_G_CTRL,&ctrl))
    {
        qDebug()<<"V4L2_CID_EXPOSURE error";
        Log::LogCam("init_device,V4L2_CID_EXPOSURE error");
    }
    else {
        qDebug()<<"Get Exposure:"<<ctrl.value;
        Log::LogCam(QString("init_device,Get Exposure:%1").arg(ctrl.value));
    }

    setting.id = V4L2_CID_EXPOSURE;
    if (-1 == xioctl(fd,VIDIOC_QUERYCTRL,&setting))
    {
        qDebug()<<"V4L2_CID_EXPOSURE query error";
        Log::LogCam("init_device,V4L2_CID_EXPOSURE query error");
    }
    else {
        qDebug()<<"query Exposure:max="<<setting.maximum<<",min="<<setting.minimum<<",step="<<setting.step<<",default="<<setting.default_value;
        Log::LogCam(QString("init_device,query Exposure:max=%1,min=%2,step=%3,default=%4").arg(setting.maximum).arg(setting.minimum).arg(setting.step).arg(setting.default_value));
    }

    ctrl.id = V4L2_CID_EXPOSURE;
    ctrl.value = -4;
    if (-1 == xioctl(fd,VIDIOC_S_CTRL,&ctrl))
    {
        qDebug()<<"VIDIOC_S_CTRL V4L2_CID_EXPOSURE error";
        Log::LogCam("init_device,VIDIOC_S_CTRL V4L2_CID_EXPOSURE error");
    }
#endif

    setting.id = V4L2_CID_EXPOSURE_ABSOLUTE;
    if (-1 == xioctl(fd,VIDIOC_QUERYCTRL,&setting))
    {
        qDebug()<<"V4L2_CID_EXPOSURE_ABSOLUTE query error";
        Log::LogCam("init_device,V4L2_CID_EXPOSURE_ABSOLUTE query error");
    }
    else {
        qDebug()<<"query ABSOLUTE:max="<<setting.maximum<<",min="<<setting.minimum<<",step="<<setting.step<<",default="<<setting.default_value;
        Log::LogCam(QString("init_device,query ABSOLUTE:max=%1,min=%2,step=%3,default=%4").arg(setting.maximum).arg(setting.minimum).arg(setting.step).arg(setting.default_value));
    }

    //set ABSOLUTE
#if 1
    ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
    ctrl.value = ExGlobal::CamAbs;

    if (-1 == xioctl(fd,VIDIOC_S_CTRL,&ctrl))
    {
        qDebug()<<"VIDIOC_S_CTRL V4L2_CID_EXPOSURE_ABSOLUTE error";
        Log::LogCam("init_device,VIDIOC_S_CTRL V4L2_CID_EXPOSURE_ABSOLUTE error");
    }

    ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
    if (-1 == xioctl(fd,VIDIOC_G_CTRL,&ctrl))
    {
        qDebug()<<"V4L2_CID_EXPOSURE error";
        Log::LogCam("init_device,V4L2_CID_EXPOSURE error");
    }
    else {
        qDebug()<<"Get ABS Exposure:"<<ctrl.value;
        Log::LogCam(QString("init_device,Get ABS Exposure:%1").arg(ctrl.value));
    }
#endif

    //set GAIN
#if 1
    setting.id = V4L2_CID_GAIN;
    if (-1 == xioctl(fd,VIDIOC_QUERYCTRL,&setting))
    {
        qDebug()<<"V4L2_CID_GAIN query error";
        Log::LogCam("init_device,V4L2_CID_GAIN query error");
    }
    else {
        qDebug()<<"query GAIN:max="<<setting.maximum<<",min="<<setting.minimum<<",step="<<setting.step<<",default="<<setting.default_value;
        Log::LogCam(QString("init_device,query GAIN:max=%1,min=%2,step=%3,default=%4").arg(setting.maximum).arg(setting.minimum).arg(setting.step).arg(setting.default_value));
    }


    ctrl.id = V4L2_CID_GAIN;
    ctrl.value = ExGlobal::CamGain;
    if (-1 == xioctl(fd,VIDIOC_S_CTRL,&ctrl))
    {
        qDebug()<<"VIDIOC_S_CTRL V4L2_CID_GAIN error";
        Log::LogCam("init_device,VIDIOC_S_CTRL V4L2_CID_GAIN error");
    }

    ctrl.id = V4L2_CID_GAIN;
    if (-1 == xioctl(fd,VIDIOC_G_CTRL,&ctrl))
    {
        qDebug()<<"V4L2_CID_GAIN error";
        Log::LogCam("init_device,V4L2_CID_GAIN error");
    }
    else {
        qDebug()<<"Get GAIN:"<<ctrl.value;
        Log::LogCam(QString("init_device,Get GAIN:%1").arg(ctrl.value));
    }
#endif

#endif

    bufrgb = (unsigned char *)malloc(fmt.fmt.pix.height * fmt.fmt.pix.width * 3);
    bufy = (unsigned char *)malloc(fmt.fmt.pix.height * fmt.fmt.pix.width);
    buf2y = (unsigned short *)malloc(fmt.fmt.pix.height * fmt.fmt.pix.width * 2);
    sum = (unsigned int *)malloc(fmt.fmt.pix.height * fmt.fmt.pix.width * 8);
    if (io == IO_METHOD_READ)
        return init_read(fmt.fmt.pix.sizeimage);
    else
        return init_mmap();
}

int ImageCapture::init_read(unsigned int buffer_size){
    buffers = (buffer *)calloc(CAPTURE_COUNT,sizeof(*buffers));
    if (!buffers)
    {
        qDebug()<<"Out of memory, return";
        return -1;
    }
    for (int i = 0; i < CAPTURE_COUNT; i++){
        buffers[i].length = buffer_size;
        buffers[i].start = malloc(buffer_size);
        if (!buffers[i].start)
        {
            qDebug()<<"Out of memory, return";
            return -1;
        }
    }
    return 0;
}

int ImageCapture::init_mmap(){
    struct v4l2_requestbuffers req;
    CLEAR(req);

    req.count = 2;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (-1 == xioctl(fd, VIDIOC_REQBUFS,&req)){
        if (EINVAL == errno)
        qDebug()<<"dev does not support memory mapping, return";
        else
            qDebug()<<"VIDIOC_REQBUFS error, return";
        Log::LogCam(QString("init_mmap,VIDIOC_REQBUFS errno:%1").arg(errno));
        return -1;
    }
    if (req.count<2)
    {
        qDebug()<<"Insufficient buffer memory, return";
        Log::LogCam("init_mmap,Insufficient buffer memory, return -1");
        return -1;
    }
    resultData.resize(12);
    resultData[0] = '\xaa';
    resultData[1] = '\x03';
    resultData[6] = '\x00';
    resultData[7] = '\xA0';
    resultData[8] = '\x00';
    resultData[9] = '\x02';
    resultData[11] = '\x55';

    buffers = (buffer *)calloc(req.count,sizeof(*buffers));
    if (!buffers){
        qDebug()<<"Out of memory,return";
        Log::LogCam("init_mmap,Out of memory,return -1");
        return -1;
    }
    for (n_buffers = 0; n_buffers < req.count; ++n_buffers){
        struct v4l2_buffer buf;
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;
        if (-1 == xioctl(fd,VIDIOC_QUERYBUF, &buf))
        {
            qDebug()<<"VIDIOC_QUERYBUF, return";
            Log::LogCam("init_mmap,VIDIOC_QUERYBUF return -1");
            return -1;
        }
        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = mmap(NULL,buf.length,PROT_READ|PROT_WRITE,MAP_SHARED,fd,buf.m.offset);
        if(MAP_FAILED == buffers[n_buffers].start)
        {
            qDebug()<<"mmap error,return";
            Log::LogCam("init_mmap,mmap error,return -1");
            return -1;
        }
    }
    return 0;
}

int ImageCapture::start_capturing(CaptureMode mode)
{
    qDebug()<<"start_capturing,camerainited:"<<camerainited<<"captureMode:"<<captureMode;
    Log::LogCam(QString("start_capturing,camerainited:%1").arg(camerainited));
    if (camerainited == false || captureMode != CaptureMode::Idle)
        return -1;
    captureMode = mode;
    if (io == IO_METHOD_MMAP)
    {
        enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        for (unsigned int i = 0; i < n_buffers; ++i){
            struct v4l2_buffer buf;
            CLEAR(buf);

            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index = i;
            if (-1 == xioctl(fd, VIDIOC_QBUF,&buf))
            {
                qDebug()<<"start_capturing,VIDIOC_QBUF error,return -1";
                Log::LogCam("start_capturing,VIDIOC_QBUF error,return -1");
                return -1;
            }
        }

        if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
        {
            qDebug()<<"start_capturing,VIDIOC_STREAMON error,return -1";
            Log::LogCam("start_capturing,VIDIOC_STREAMON error,return -1");
            return -1;
        }
    }
    return 0;
}

int ImageCapture::stop_capturing()
{
    qDebug()<<"stop_capturing,captureMode:"<<captureMode;
    if (captureMode == CaptureMode::View)
    {
        captureMode = CaptureMode::Idle;
        return 0;
    }
    captureMode = CaptureMode::Idle;
    if (io == IO_METHOD_MMAP){
        enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
        {
            qDebug()<<"VIDIOC_STREAMON error,return";
            Log::LogCam("stop_capturing,VIDIOC_STREAMON error,return -1");
            return -1;
        }
    }
    return 0;
}

int ImageCapture::read_frame(int index){
    if (io == IO_METHOD_READ){
        if (-1 == read(fd, buffers[0].start,buffers[0].length)){
            if (errno == EAGAIN)
                return 0;
            else
            {
                qDebug()<<"read error,return";
                return -1;
            }
        }
    }else {
        struct v4l2_buffer buf;

        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)){
            if (errno == EAGAIN)
                return 0;
            else
            {
                qDebug()<<"VIDIOC_DQBUF return -1,error="<<errno;
                Log::LogCam(QString("read_frame,VIDIOC_DQBUF errno:%1,return -1").arg(errno));
                return -1;
            }
        }
        if (buf.index >= n_buffers)
        {
            qDebug()<<"buf.index error,return";
            Log::LogCam("read_frame,buf.index error,return -1");
            return -1;
        }
        qDebug()<<"readframe,sec:"<<buf.timestamp.tv_sec<<"usec:"<<buf.timestamp.tv_usec;
        Log::LogCam(QString("read_frame,timestamp:sec:%1,usec:%2").arg(buf.timestamp.tv_sec).arg(buf.timestamp.tv_usec));
        if (process_image(index, buffers[buf.index].start,buf.length))
        {
            Log::LogCam("read_frame,process image error, return -1");
            return -1;
        }

        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
        {            
            Log::LogCam(QString("read_frame,VIDIOC_QBUF errno:%1,return -1").arg(errno));
            qDebug()<<"VIDIOC_QBUF return -1, errno="<<errno;
            return -1;
        }
    }
    return 0;
}

int ImageCapture::clear_frame(){
    if (io == IO_METHOD_MMAP){

        struct v4l2_buffer buf;
        qDebug()<<"clear frame, n_buffers="<<n_buffers;
        Log::LogCam(QString("clear_frame,n_buffers=%1").arg(n_buffers));
        for(int i = 0; i<=n_buffers; i++)
        {
            CLEAR(buf);
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)){
                if (errno == EAGAIN)
                {
                    qDebug()<<"VIDIOC_DQBUF error==EAGAIN,return";
                    Log::LogCam("clear_frame,VIDIOC_DQBUF error==EAGAIN,return 0");
                    return 0;
                }
                else
                {
                    qDebug()<<"VIDIOC_DQBUF return -1,error="<<errno;
                    Log::LogCam(QString("clear_frame,VIDIOC_DQBUF errno:%1,return -1").arg(errno));
                    return -1;
                }
            }
            if (buf.index >= n_buffers)
            {
                qDebug()<<"buf.index error,return";
                Log::LogCam("clear_frame,buf.index error,return -1");
                return -1;
            }
            qDebug()<<"readframe,sec:"<<buf.timestamp.tv_sec<<"usec:"<<buf.timestamp.tv_usec<<"index:"<<buf.index;
            Log::LogCam(QString("clear_frame,timestamp:sec:%1,usec:%2,index:%3").arg(buf.timestamp.tv_sec).arg(buf.timestamp.tv_usec).arg(buf.index));

            if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
            {
                qDebug()<<"VIDIOC_QBUF return -1, error="<<errno;
                Log::LogCam(QString("clear_frame,VIDIOC_QBUF errno:%1,return -1").arg(errno));
                return -1;
            }
        }
    }
    return 0;
}


int ImageCapture::process_image(int i, const void *p, int size){
    if(captureMode == CaptureMode::View)
    {
        convert_yuv_to_rgb_buffer((unsigned char *)p,bufrgb,2592,1944);
        QImage image(bufrgb,2592,1944,QImage::Format_RGB888);
        emit reView(image);
    }
    else{
        char filenames[100] = {0};
        unsigned char *data = (unsigned char *)p;

#if 0
        sprintf(filenames,"%s/%s_%02d.bmp",Log::getDir().toLatin1().data(),filename.toLatin1().data(),i);
        convert_yuv_to_rgb_buffer((unsigned char *)p,bufrgb,2592,1944);
        QImage image(bufrgb,2592,1944,QImage::Format_RGB888);
        image.save(filenames);
#endif
#if 0
        sprintf(filenames,"%s/%s_%02d.y",Log::getDir().toLatin1().data(),filename.toLatin1().data(),i);
        convert_yuv_to_y_buffer((unsigned char *)p,bufy,2592,1944);
        FILE *file_f = fopen(filenames,"w");
        fwrite(bufy,size>>1,1,file_f);
        fclose(file_f);
#endif

        if (count == 1)
        {
            convert_yuv_to_y_buffer((unsigned char *)p,bufy,2592,1944);
            convert_yuv_to_2y_buffer((unsigned char *)p,buf2y,2592,1944);
            convert_yuv_to_rgb_buffer((unsigned char *)p,bufrgb,2592,1944);
        }
        else
        {
            if (i == 1)
                memset((void *)sum,0,2592*1944*8);

            for (int j = 0; j < 2592*1944*2; j++)
                sum[j] += data[j];

            if(i == count){
                unsigned short temp;
                for (int j = 0; j < 2592*1944; j++)
                {
                    bufy[j] = sum[j<<1]/count;
                    buf2y[j] = sum[j<<1]*256/count;
                    temp = buf2y[j]>>8;
                    buf2y[j] = temp+(buf2y[j]<<8);
                }
                //qDebug()<<"buf2y="<<buf2y[2592*500+1000]<<",bufy="<<bufy[2592*500+1000]<<",sum="<<sum[(2592*500+1000)<<1];

                for (int j = 0; j < 2592*1944*2; j++)
                    sum[j] = sum[j]/count;
                convert_yuv_to_rgb(sum,bufrgb,2592,1944);
            }
        }


        if (i == count)
        {
            sprintf(filenames,"%s/%s.y",Log::getDir().toLatin1().data(),filename.toLatin1().data());
            FILE *file_s = fopen(filenames,"w");
            fwrite(bufy,size>>1,1,file_s);
            fclose(file_s);
            Log::LogCam("Image Capture "+QString::fromUtf8(filenames));
            //imageAna.FirstImage(bufrgb,0);
#if 0
            sprintf(filenames,"%s/%s.2y",Log::getDir().toLatin1().data(),filename.toLatin1().data());
            FILE *file_2s = fopen(filenames,"w");
            fwrite(buf2y,size,1,file_2s);
            fclose(file_2s);
#endif
#if 1
            sprintf(filenames,"%s/%s.bmp",Log::getDir().toLatin1().data(),filename.toLatin1().data());            
            QImage image(bufrgb,2592,1944,QImage::Format_RGB888);
            image.save(filenames);
#endif   
        }
    }

    return 0;
}

void ImageCapture::run()
{
    if (camerainited == false)
        return;

    //if (captureMode == CaptureMode::Capture)
        clear_frame();

    for (int i = 1; i <= count; i++)
    {
        qDebug()<<"Capture:"<<i;
        Log::LogCam(QString("run,Capture:%1").arg(i));
        fd_set fds;
        struct timeval tv;
        int r;
        qmutex.lock();
        FD_ZERO(&fds);
        FD_SET(fd,&fds);
        tv.tv_sec = 40;
        tv.tv_usec = 0;
        r = select(fd+1,&fds,NULL,NULL,&tv);
        if (-1 == r){
            if (EINTR == errno)
                continue;
            qDebug()<<"select err";
            Log::LogCam("run,select err");
            resultData[10] = 1;
            qmutex.unlock();
            emit finishCapture(resultData);
            return;
        }

        if (0 == r) //select timeout
        {
            resultData[10] = 2;
            qmutex.unlock();
            emit finishCapture(resultData);
            qDebug()<<"selet timeout";
            Log::LogCam("run,selet timeout");
            return;
        }

        if (read_frame(i))
        {
            resultData[10] = 3;
            qmutex.unlock();
            Log::LogCam("run,read frame err");
            emit finishCapture(resultData);
            return;
        }

        if (captureMode == CaptureMode::View)
            i = 1;
        else if (captureMode == CaptureMode::Idle)
        {
            qmutex.unlock();
            stop_capturing();
            return;
        }
        qmutex.unlock();
    }
    resultData[10] = 0;
    emit finishCapture(resultData);
}

int ImageCapture::getabsExpose(){
    struct v4l2_control ctrl;
    struct v4l2_queryctrl setting;

    if (camerainited == false)
        return -1;

    qmutex.lock();
    ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
    if (-1 == xioctl(fd,VIDIOC_G_CTRL,&ctrl))
    {
        Log::LogCam("getabsExpose,V4L2_CID_EXPOSURE_ABSOLUTE error");
        ctrl.value = -1;
    }
    qDebug()<<"getabsExpose:"<<ctrl.value;
    qmutex.unlock();
    return ctrl.value;
}

bool ImageCapture::setabsExpose(int value){
    struct v4l2_control ctrl;

    qDebug()<<"setabsExpose camerainited:"<<camerainited;
    if (camerainited == false)
        return false;
    qmutex.lock();
    ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
    ctrl.value = value;

    if (-1 == xioctl(fd,VIDIOC_S_CTRL,&ctrl))
    {
        qDebug()<<"setabsExpose VIDIOC_S_CTRL V4L2_CID_EXPOSURE_ABSOLUTE error";
        Log::LogCam("setabsExpose,VIDIOC_S_CTRL V4L2_CID_EXPOSURE_ABSOLUTE error");
    }
    qmutex.unlock();
    qDebug()<<"setabsExpose value:"<<value;
    ExGlobal::updateCaliParam("CamAbs",value);   
    return true;
}

int ImageCapture::getGain(){
    struct v4l2_control ctrl;

    if (camerainited == false)
        return -1;
    qmutex.lock();
    ctrl.id = V4L2_CID_GAIN;
    if (-1 == xioctl(fd,VIDIOC_G_CTRL,&ctrl))
    {
        Log::LogCam("getGain,V4L2_CID_GAIN error");
        ctrl.value = -1;
    }
    qmutex.unlock();
    qDebug()<<"getGain:"<<ctrl.value;
    return ctrl.value;
}

bool ImageCapture::setGain(int value){
    struct v4l2_control ctrl;

    qDebug()<<"setGain camerainited:"<<camerainited;
    if (camerainited == false)
        return false;

    qmutex.lock();
    ctrl.id = V4L2_CID_GAIN;
    ctrl.value = value;

    if (-1 == xioctl(fd,VIDIOC_S_CTRL,&ctrl))
    {
        qDebug()<<"setGain VIDIOC_S_CTRL V4L2_CID_GAIN error";
        Log::LogCam("setGain,VIDIOC_S_CTRL V4L2_CID_GAIN error");
    }
    qmutex.unlock();
    qDebug()<<"setGain value:"<<value;
    ExGlobal::updateCaliParam("CamGain",value);
    return true;
}

bool ImageCapture::Running(){
    int count = 0;
    while(this->isRunning() && count<30){
        qDebug()<<"Running:"<<count;
        count++;
        QThread::msleep(100);
    }
    if (count<100)
        return false;
    return true;
}
