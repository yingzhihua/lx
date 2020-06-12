#include "ckcamera.h"

#include <QDebug>
#include <QDateTime>
#include <QDir>

#include "log.h"
#include "exglobal.h"

static QString cameraName = "";
static QByteArray resultData;
static int imageCount = 0;
CKCamera::CKCamera(QObject *parent) : QThread(parent)
{
    ExGlobal::bufrgb = (unsigned char *)malloc(2592 * 1944 * 3);
    ExGlobal::hbufrgb = (ExGlobal::bufrgb) + (1296*1944*3);

    rawData = new uint8_t[2592 * 1944];
    sum = new uint32_t[2592 * 1944];
    m_hCamera = nullptr;
    stopping = false;
    count = 0;
    captureMode = CaptureMode::Idle;
    resultData.resize(12);
    resultData[0] = '\xaa';
    resultData[1] = '\x03';
    resultData[6] = '\x00';
    resultData[7] = '\xA0';
    resultData[8] = '\x00';
    resultData[9] = '\x02';
    resultData[11] = '\x55';

    Log::LogByFile("CK.txt",QString("CK Camera Init imageCount=%1").arg(imageCount));
}

bool CKCamera::readCamera(){
    int cameraNum = 0;
    CameraEnumerateDevice(&cameraNum);
    qDebug()<<"CKReadCamera count="<<cameraNum;
    Log::LogByFile("CK.txt",QString("ReadCamera count=%1").arg(cameraNum));
    if (cameraNum > 0)
    {
        tDevEnumInfo devAllInfo;
        CameraGetEnumIndexInfo(cameraNum-1, &devAllInfo);
        cameraName = tr(devAllInfo.DevAttribute.acFriendlyName);
        qDebug()<<cameraName;
        return true;
    }
    return false;
}

bool CKCamera::openCamera(){
    CameraSdkStatus ret;
    tSdkCameraCapbility cap;

    Log::LogByFile("CK.txt",QString("openCamera m_hCamera=%1").arg(m_hCamera!=nullptr));
    if (m_hCamera != nullptr)
        return true;

    if (CameraInit(&m_hCamera, 0) != CAMERA_STATUS_SUCCESS)
    {
        Log::LogByFile("CK.txt",QString("openCamera CameraInit failed"));
        m_hCamera = nullptr;
        return false;
    }

    if (CameraGetCapability(m_hCamera, &cap) == CAMERA_STATUS_SUCCESS){
        for (int i = 0; i < cap.tDeviceCapbility.iBayerTypeDesc; i++){
            qDebug()<<"iIndex="<<cap.tDeviceCapbility.pBayerTypeDesc[i].iIndex<<"des:"<<cap.tDeviceCapbility.pBayerTypeDesc[i].acDescription<<"MediaType:"<<cap.tDeviceCapbility.pBayerTypeDesc[i].iMediaType;
        }
    }

    CameraSetSensorOutPixelFormat(m_hCamera,cap.tDeviceCapbility.pBayerTypeDesc[0].iMediaType);

    uint mediaType;
    if (CameraGetSensorOutPixelFormat(m_hCamera,&mediaType) == CAMERA_STATUS_SUCCESS)
        qDebug()<<"mediaType:"<<mediaType;

    //CameraSetIspOutFormat(m_hCamera, CAMERA_MEDIA_TYPE_BGR8);
    CameraSetIspOutFormat(m_hCamera, CAMERA_MEDIA_TYPE_MONO8);
    CameraSetAeState(m_hCamera,false);
    ret = CameraSetExposureTime(m_hCamera,ExGlobal::CamAbs*1000);
    CameraSetAnalogGain(m_hCamera,1000);
    CameraSetTriggerMode(m_hCamera,1);
    CameraSetFrameSpeed(m_hCamera,2);

    double exposureTime = 0;
    CameraGetExposureTime(m_hCamera, &exposureTime);
    qDebug()<<"Exposure Time="<<exposureTime<<"ret:"<<ret;
    Log::LogByFile("CK.txt",QString("openCamera Exposure Time==%1").arg(exposureTime));
    CameraPlay(m_hCamera);
    return true;
}

bool CKCamera::closeCamera(){
    qDebug()<<"CK closeCamera";
    Log::LogByFile("CK.txt",QString("closeCamera m_hCamera=%1").arg(m_hCamera!=nullptr));
    waitStop();
    if (m_hCamera != nullptr)
    {
        CameraUnInit(m_hCamera);
        m_hCamera = nullptr;
    }
    qDebug()<<"CK closeCamera finish";
    captureMode = CaptureMode::Idle;
    return true;
}

bool CKCamera::stopCamera(){
    qDebug()<<"CK stopCamera";
    Log::LogByFile("CK.txt",QString("stopCamera m_hCamera=%1").arg(m_hCamera!=nullptr));
    waitStop();
    qDebug()<<"CK stopCamera finish";
    captureMode = CaptureMode::Idle;
    return true;
}

static bool saverawimage = true;

uint32_t CKCamera::CKGetFrame(){
    CameraSdkStatus status;
    HANDLE hBuf;
    uint8_t* pbyBuffer = nullptr;
    stImageInfo imageInfo;
    imageCount++;
    Log::LogByFile("CK.txt",QString("CKGetFrame imageCount==%1,m_hCamera=%2").arg(imageCount).arg(m_hCamera!=nullptr));
    qDebug()<<"CKGetFrame"<<m_hCamera;
    status = CameraSoftTrigger(m_hCamera);
    if (status != CAMERA_STATUS_SUCCESS){
        Log::LogByFile("CK.txt",QString("CKGetFrame, CameraSoftTrigger fail, status =%1").arg(status));
        return 0;
    }

    status = CameraGetRawImageBuffer(m_hCamera, &hBuf, 2000);
    if (status != CAMERA_STATUS_SUCCESS)
    {
        Log::LogByFile("CK.txt",QString("CKGetFrame, CameraGetRawImageBuffer fail, status =%1").arg(status));
        return 0;
    }

    // 获取图像帧信息
    pbyBuffer = CameraGetImageInfo(m_hCamera, hBuf, &imageInfo);    
    memcpy(rawData,pbyBuffer,imageInfo.TotalBytes);    
    status = CameraReleaseFrameHandle(m_hCamera, hBuf);
    if (status != CAMERA_STATUS_SUCCESS)
    {
        Log::LogByFile("CK.txt",QString("CKGetFrame, CameraReleaseFrameHandle fail, status =%1").arg(status));
        return 0;
    }

    Log::LogByFile("CK.txt",QString("CKGetFrame, complete TotalBytes =%1").arg(imageInfo.TotalBytes));
    if (saverawimage == false)
    {
        saveRaw(rawData,imageInfo.TotalBytes);
        saverawimage = true;
    }
    return imageInfo.TotalBytes;
}

int CKCamera::process_image(int index, uint8_t *data, uint32_t datalength){
    QString dirName = "CK";
    int width = 2592;
    int height = 1944;

/*
    if (index == count){
        QImage img(bufy, width, height, QImage::Format_Grayscale8);
        img.save(Log::getDir()+"/"+dirName+"/"+filename+".bmp");
        Log::LogByFile(dirName+".txt","save file:"+Log::getDir()+"/"+dirName+"/"+filename+".bmp");
    }
*/
    if (ExGlobal::bChildImage){
        QString savefile = QString("%1/%2-%3.raw").arg(Log::getDir()).arg(filename).arg(index);
        QFile file(savefile);
        file.open(QIODevice::WriteOnly);
        file.write((const char *)data,datalength);
        file.close();
    }

    if (count != 1)
    {
        if (index == 1)
        {
            for (int j = 0; j < width*height; j++)
                sum[j] = data[j];
        }
        else {
            for (int j = 0; j < width*height; j++)
                sum[j] += data[j];
            if (count == index){
                for (int j = 0; j < width*height; j++)
                    rawData[j] = sum[j]/count;
            }
        }
    }

    if (index == count){
        QString savefile = QString("%1/%2.raw").arg(Log::getDir()).arg(filename);
        QFile file(savefile);
        file.open(QIODevice::WriteOnly);
        file.write((const char *)rawData,datalength);
        file.close();

        QImage img(rawData, width, height, QImage::Format_Grayscale8);
        savefile = QString("%1/%2.bmp").arg(Log::getDir()).arg(filename);
        img.save(savefile);
        Log::LogByFile("CK.txt","save file:"+savefile);
        resultData[10] = 0;
        emit finishCapture(resultData);
    }

    return 0;
}

void CKCamera::saveRaw(uint8_t *data, uint32_t datalength){
    QString savefile = QString("%1/test.raw").arg(Log::getDir());
    QFile file(savefile);
    file.open(QIODevice::WriteOnly);
    file.write((const char *)data,datalength);
    file.close();
}

bool CKCamera::capture(QString fileName, int nCount){
    qDebug()<<"CK capture"<<captureMode;
    Log::LogByFile("CK.txt",QString("capture"));
    waitStop();
    filename = fileName;
    count = nCount;    
    openCamera();
    captureMode = CaptureMode::Capture;
    stopping = false;
    this->start();
    return true;
}

bool CKCamera::preview(){
    qDebug()<<"CK preview"<<captureMode;
    Log::LogByFile("CK.txt",QString("preview"));
    if (captureMode == CaptureMode::View)
        return true;        
    waitStop();
    openCamera();
    count = 1;
    stopping = false;
    captureMode = CaptureMode::View;
    this->start();
    return true;
}

bool CKCamera::setabsExpose(int value){
    Log::LogByFile("CK.txt",QString("setabsExpose,value=%1").arg(value));
    if (openCamera())
    {
        if (CAMERA_STATUS_SUCCESS != CameraSetExposureTime(m_hCamera,value*1000))
            return false;

        double exposureTime = 0;
        CameraSdkStatus ret = CameraGetExposureTime(m_hCamera, &exposureTime);
        qDebug()<<"Exposure Time="<<exposureTime<<"ret:"<<ret;
        ExGlobal::updateCaliParam("CamAbs",value);
        Log::LogByFile("CK.txt",QString("setabsExpose,result=%1").arg(ret));
        return true;
    }
    return false;
}

bool CKCamera::setGain(int value){
    qDebug()<<"setGain:"<<value;
    return true;
}

bool CKCamera::setWhite(int value){
    qDebug()<<"setWhite:"<<value;
    return true;
}

void CKCamera::run(){
    QTime time;
    int elapse;
    int nFrame = 0;
    uint32_t dataLength;
    qDebug()<<"CK run"<<stopping<<count;
    while(!stopping&&nFrame<count){
        time.start();
        dataLength = CKGetFrame();
        if (dataLength != 0){
            elapse = time.elapsed();
            qDebug()<<"CKCamera::run,elapse="<<elapse<<"Frame="<<nFrame;
            if (captureMode == CaptureMode::View){
                QImage image(rawData, 2592, 1944, QImage::Format_Grayscale8);
                emit reView(image);
            }
            else if (captureMode == CaptureMode::Capture)
            {
                nFrame++;
                process_image(nFrame,rawData,dataLength);
                if (nFrame<count && elapse < 500){
                    QThread::msleep(500-elapse);
                }
            }
        }
        else {
            resultData[10] = 1;
            emit finishCapture(resultData);
            break;
        }

    }
    captureMode = CaptureMode::Idle;
    stopping = false;
}

bool CKCamera::waitStop(){
    stopping = true;
    for (int i = 0; i < 30; i++){
        if (!this->isRunning())
            return true;
        QThread::msleep(100);
    }
    return false;
}
