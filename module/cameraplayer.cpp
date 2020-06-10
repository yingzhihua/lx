#include "cameraplayer.h"
#include "log.h"
#include <QDebug>
#include <QDateTime>
#include <QDir>

static int CameraIndex = 0;
static QString cameraName = "";

CameraPlayer::CameraPlayer(QObject *parent) : QThread(parent)
{
    bufy = new uint8_t[2592 * 1944 * 4];
    rawData = new uint8_t[2592 * 1944 * 2];
    m_hCamera = nullptr;
/*
    if (CKReadCamera())
    {
        qDebug()<<"CameraPlayer CK Camera";
        cameraType = CAMERA_TYPE_CK;
        return;
    }

    if(DVPReadCamera()){
        qDebug()<<"CameraPlayer DVP Camera";
        cameraType = CAMERA_TYPE_DVP;
        return;
    }    
    qDebug()<<"CameraPlayer NO Camera";
    cameraType = CAMERA_EMPTY;
    */
}

bool CameraPlayer::CameraStart(CaptureMode mode){
    captureMode = mode;
    stoping = false;
    if (cameraType == CAMERA_TYPE_CK)
        return CKOpenCamera();
    else if(cameraType == CAMERA_TYPE_DVP)
        return DVPOpenCamera();
    return false;
}

bool CameraPlayer::CameraStop(){
    stoping = true;
    waitStop();
    if (cameraType == CAMERA_TYPE_CK)
        return CKCloseCamera();
    else if(cameraType == CAMERA_TYPE_DVP)
        return DVPCloseCamera();
    return false;
}

bool CameraPlayer::waitStop(){
    for (int i = 0; i < 30; i++){
        if (!this->isRunning())
            return true;
        QThread::msleep(100);
    }
    return false;
}

int CameraPlayer::process_image(int index, uint8_t *data, int datalength){
    QString dirName;
    int width = 2592;
    int height = 1944;

    if (cameraType == CAMERA_TYPE_CK)
        dirName = "CK";
    else if(cameraType == CAMERA_TYPE_DVP)
        dirName = "DVP";


    QDir dir(Log::getDir()+"/"+dirName);
    if (!dir.exists())
        dir.mkdir(Log::getDir()+"/"+dirName);
/*
    if (index == count){
        QImage img(bufy, width, height, QImage::Format_Grayscale8);
        img.save(Log::getDir()+"/"+dirName+"/"+filename+".bmp");
        Log::LogByFile(dirName+".txt","save file:"+Log::getDir()+"/"+dirName+"/"+filename+".bmp");
    }
*/
    QString savefile = QString("%1/%2/%3-%4.raw").arg(Log::getDir()).arg(dirName).arg(filename).arg(index);
    QFile file(savefile);
    file.open(QIODevice::WriteOnly);
    file.write((const char *)data,datalength);
    file.close();

    QImage img(data, width, height, QImage::Format_Grayscale8);
    savefile = QString("%1/%2/%3-%4.bmp").arg(Log::getDir()).arg(dirName).arg(filename).arg(index);
    img.save(savefile);
    Log::LogByFile(dirName+".txt","save file:"+savefile);

    return 0;
}

//CKCamera
bool CameraPlayer::CKReadCamera(){
    int cameraNum = 0;
    CameraEnumerateDevice(&cameraNum);
    qDebug()<<"CKReadCamera count="<<cameraNum;
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

bool CameraPlayer::CKOpenCamera(){    
    if (m_hCamera != nullptr)
        return false;

    CameraSdkStatus ret = CameraInit(&m_hCamera, 0);
    if (ret != CAMERA_STATUS_SUCCESS)
    {
        m_hCamera = nullptr;
        return false;
    }

    tSdkCameraCapbility cap;
    if (CameraGetCapability(m_hCamera, &cap) == CAMERA_STATUS_SUCCESS){
        for (int i = 0; i < cap.tDeviceCapbility.iBayerTypeDesc; i++){
            qDebug()<<"iIndex="<<cap.tDeviceCapbility.pBayerTypeDesc[i].iIndex<<"des:"<<cap.tDeviceCapbility.pBayerTypeDesc[i].acDescription<<"MediaType:"<<cap.tDeviceCapbility.pBayerTypeDesc[i].iMediaType;
        }
    }

    CameraSetSensorOutPixelFormat(m_hCamera,cap.tDeviceCapbility.pBayerTypeDesc[1].iMediaType);

    uint mediaType;
    if (CameraGetSensorOutPixelFormat(m_hCamera,&mediaType) == CAMERA_STATUS_SUCCESS)
        qDebug()<<"mediaType:"<<mediaType;

    //CameraSetIspOutFormat(m_hCamera, CAMERA_MEDIA_TYPE_BGR8);
    CameraSetIspOutFormat(m_hCamera, CAMERA_MEDIA_TYPE_MONO8);
    CameraSetAeState(m_hCamera,false);
    ret = CameraSetExposureTime(m_hCamera,100*1000);
    CameraSetAnalogGain(m_hCamera,1000);
    CameraSetTriggerMode(m_hCamera,1);
    CameraSetFrameSpeed(m_hCamera,2);

    double exposureTime = 0;
    CameraGetExposureTime(m_hCamera, &exposureTime);
    qDebug()<<"Exposure Time="<<exposureTime<<"ret:"<<ret;
    CameraPlay(m_hCamera);
    return true;
}

bool CameraPlayer::CKCloseCamera(){
    if (m_hCamera != nullptr)
    {
        CameraUnInit(m_hCamera);
        m_hCamera = nullptr;
    }
    return true;
}

bool CameraPlayer::CKGetFrame(int index){
    CameraSdkStatus status;
    HANDLE hBuf;
    uint8_t* pbyBuffer;
    stImageInfo imageInfo;
    QTime time;
    int elapse1,elapse2;

    CameraIndex++;
    time.start();
    //qDebug()<<CameraIndex<<"CKGetFrame"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    CameraResetTimestamp(m_hCamera);
    CameraSoftTrigger(m_hCamera);
    status = CameraGetRawImageBuffer(m_hCamera, &hBuf, 2000);
    uint64_t sensorTime = 0;
    CameraGetImageTimestamp(m_hCamera,hBuf,&sensorTime);
    elapse1 = time.elapsed();    
    if (status != CAMERA_STATUS_SUCCESS)
    {
        Log::LogByFile("CK.txt",QString("%1,GetRaw fail status =%2").arg(CameraIndex).arg(status));
        return false;
    }
    // 获取图像帧信息    
    pbyBuffer = CameraGetImageInfo(m_hCamera, hBuf, &imageInfo);

    CameraGetImageTimestamp(m_hCamera,hBuf,&sensorTime);
    //status = CameraGetOutImageBuffer(m_hCamera, &imageInfo, pbyBuffer, bufy);    
    if (status == CAMERA_STATUS_SUCCESS)
    {
        process_image(index,pbyBuffer,imageInfo.TotalBytes);
        //return true;
    }
    elapse2 = time.elapsed();
    Log::LogByFile("CK.txt",QString("%1,GetRaw=%2,SaveImage=%3,sensorTime=%4").arg(CameraIndex).arg(elapse1).arg(elapse2-elapse1).arg(sensorTime));
    CameraReleaseFrameHandle(m_hCamera, hBuf);
    return false;
}

//DVPCamera
bool CameraPlayer::DVPReadCamera(){
    dvpStatus status;
    uint cameraNum = 0;
    dvpCameraInfo info;
    QStringList port;

    status = dvpRefresh(&cameraNum);
    if (status != DVP_STATUS_OK)
        return false;
    qDebug()<<"DVPReadCamera count="<<cameraNum;
    if (cameraNum > 0)
    {
        status = dvpEnum(cameraNum-1, &info);
        if (status != DVP_STATUS_OK)
            return false;
        cameraName = tr(info.FriendlyName);
        qDebug()<<cameraName;
        return true;
    }
    return false;
}

bool CameraPlayer::DVPOpenCamera(){
    dvpStatus status;
    dvpStreamState state;
    if (cameraName != "")
    {
        // 通过枚举到并选择的FriendlyName打开指定设备
        status = dvpOpenByName(cameraName.toLatin1().data()
,OPEN_NORMAL,&dvp_handle);
        if (status != DVP_STATUS_OK)
            return false;

        status = dvpGetStreamState(dvp_handle,&state);
        if (status != DVP_STATUS_OK)
            return false;

        if (state == STATE_STARTED)
            status = dvpStop(dvp_handle);

        DVPInitCamera();
    }
    return false;
}

bool CameraPlayer::DVPInitCamera(){
    // 初始化曝光模式，曝光时间，模拟增益，消频闪，分辨率
    //InitAEMode();
    //InitAETarget();
    //InitSpinExpoTime();
    //InitSpinGain();
    //InitAntiFlickMode();
    //InitROIMode();
    //InitColorSolution();
    dvpStatus status;
    QString strValue;
    dvpInt32  iAETarget;
    dvpIntDescr sAeTargetDescr;

    if (IsValidHandle(dvp_handle))
    {
        status = dvpGetAeTargetDescr(dvp_handle, &sAeTargetDescr);
        if (status != DVP_STATUS_OK)
        {
            qDebug("Get AE target description fails!");
            return false;
        }
        else
        {
            qDebug()<<"Ae min"<<sAeTargetDescr.iMin<<"max:"<<sAeTargetDescr.iMax;
        }

        status = dvpGetAeTarget(dvp_handle,&iAETarget);
        if (status != DVP_STATUS_OK)
        {
            qDebug("Get AE target fail!");
            return false;
        }
        else
        {
            qDebug()<<"Ae="<<iAETarget;
        }

        // 获取曝光时间的描述信息
        double fExpoTime;
        dvpDoubleDescr ExpoTimeDescr;
        status = dvpGetExposureDescr(dvp_handle, &ExpoTimeDescr);
        if (status != DVP_STATUS_OK)
        {
            qDebug("Get exposure time description fail!");
            return false;
        }
        else
        {
            qDebug()<<"ExpoTime,min:"<<ExpoTimeDescr.fMin<<"max:"<<ExpoTimeDescr.fMax;
        }
        dvpSetExposure(dvp_handle,100000);

        // 获取曝光时间的初值
        status = dvpGetExposure(dvp_handle, &fExpoTime);
        if (status != DVP_STATUS_OK)
        {
            qDebug("Get exposure time fail!");
            return false;
        }
        else
        {
            // 设置曝光时间拖动条初始值
            qDebug()<<"ExpoTime="<<fExpoTime;
        }

        float           fAnalogGain;
        dvpFloatDescr   AnalogGainDescr;
        status = dvpGetAnalogGainDescr(dvp_handle,&AnalogGainDescr);
        if (status != DVP_STATUS_OK)
        {
            qDebug("Get analog gain description fail!");
            return false;
        }
        else
        {
            qDebug()<<"AnalogGainDescr,min:"<<AnalogGainDescr.fMin<<"max:"<<AnalogGainDescr.fMax;
        }

        // 获取模拟增益并设置模拟增益的初始值
        status = dvpGetAnalogGain(dvp_handle, &fAnalogGain);
        if (status != DVP_STATUS_OK)
        {
            qDebug("Get analog gain fail!");
            return false;
        }
        else
        {
            qDebug()<<"AnalogGain="<<fAnalogGain;
        }

        dvpUint32 QuickRoiSel = 0;
        dvpQuickRoi QuickRoiDetail;
        dvpSelectionDescr QuickRoiDescr;
        status = dvpGetQuickRoiSelDescr(dvp_handle, &QuickRoiDescr);
        if (status != DVP_STATUS_OK)
        {
            qDebug("Get quick roi sel description fail!");
            return false;
        }
        else
        {
            for (unsigned int iNum = 0; iNum<QuickRoiDescr.uCount; iNum++)
            {
                status = dvpGetQuickRoiSelDetail(dvp_handle,iNum, &QuickRoiDetail);
                if (status == DVP_STATUS_OK)
                {
                    qDebug()<<iNum<<QuickRoiDetail.selection.string;
                }
            }
        }

        // 获取分辨率模式索引
        status = dvpGetResolutionModeSel(dvp_handle,&QuickRoiSel);
        if (status != DVP_STATUS_OK)
        {
            qDebug("Get roi sel fail!");
            return false;
        }
        else
        {
            qDebug()<<"ResolutionModeSel="<<QuickRoiSel;
        }

        dvpUint32 iNum;
        dvpUint32 iColorSolutionSel;
        dvpSelectionDescr sColorSolutionSelDescr;
        dvpSelection sColorSolutionSelDetail;
        status = dvpGetColorSolutionSelDescr(dvp_handle, &sColorSolutionSelDescr);
        if (status != DVP_STATUS_OK)
        {
            qDebug("Get color solution description fail!");
            return false;
        }

        for (iNum = 0; iNum < sColorSolutionSelDescr.uCount; iNum ++)
        {
            status = dvpGetColorSolutionSelDetail(dvp_handle, iNum, &sColorSolutionSelDetail);
            if (status == DVP_STATUS_OK)
            {
                qDebug()<<iNum<<sColorSolutionSelDetail.string;
            }
        }

        status = dvpGetColorSolutionSel(dvp_handle, &iColorSolutionSel);
        if (status != DVP_STATUS_OK)
        {
            qDebug("Get color solution sel fail!");
            return false;
        }
        else
        {
            qDebug()<<"ColorSolutionSel="<<iColorSolutionSel;
        }

        //status = dvpSetSourceFormat(dvp_handle,S_RAW8);
        //qDebug()<<"dvpSetSourceFormat="<<status;
        status = dvpSetTargetFormat(dvp_handle,S_RAW8);
        qDebug()<<"dvpSetTargetFormat="<<status;
        status = dvpSetAeOperation(dvp_handle,AE_OP_OFF);
        qDebug()<<"dvpSetAeOperation="<<status;
        bool bTrigStatus;
        status = dvpGetTriggerState(dvp_handle,&bTrigStatus);
        qDebug()<<"dvpGetTriggerState"<<status<<bTrigStatus;
        if (status != DVP_STATUS_FUNCTION_INVALID)
            status = dvpSetTriggerState(dvp_handle,true);
        return true;
    }
    return false;
}

bool CameraPlayer::DVPCloseCamera(){
    dvpStatus status;
    dvpStreamState state;
    if (IsValidHandle(dvp_handle))
    {
        status = dvpGetStreamState(dvp_handle,&state);
        status = dvpSaveConfig(dvp_handle, 0);
        status = dvpClose(dvp_handle);
        dvp_handle = 0;
        qDebug()<<"dvp_close"<<status;
    }
    return true;
}

bool CameraPlayer::DVPGetFrame(int index){
    dvpStatus status;    
    dvpFrameBuffer raw,out;
    QTime time;
    timespec spec1,spec2;
    int elapse1,elapse2;
    CameraIndex++;
    time.start();    
    double timeValue = 0;
    status = dvpGetTimerValue(dvp_handle,&timeValue);
    qDebug()<<"getTimer:"<<status<<timeValue;
    clock_gettime(CLOCK_MONOTONIC,&spec1);
    dvpTriggerFire(dvp_handle);
    status = dvpGetFrameBuffer(dvp_handle,&raw,&out,30000);
    clock_gettime(CLOCK_MONOTONIC,&spec2);
    elapse1 = time.elapsed();
    qDebug()<<"status="<<status<<"FrameID:"<<raw.frame.uFrameID<<"raw format:"<<raw.frame.format<<"bits="<<raw.frame.bits<<"uBytes="<<raw.frame.uBytes<<"timestamp"<<raw.frame.uTimestamp;
    qDebug()<<"status="<<status<<"FrameID:"<<out.frame.uFrameID<<"raw format:"<<out.frame.format<<"bits="<<out.frame.bits<<"uBytes="<<out.frame.uBytes<<"timestamp"<<out.frame.uTimestamp;
    Log::LogByFile("DVP.txt",QString("%1,status:%2,raw format=%3,Timestamp=%4,clock=%5,%6,clock2=%7,%8").arg(CameraIndex).arg(status).arg(raw.frame.format).arg(raw.frame.uTimestamp).arg(spec1.tv_sec).arg(spec1.tv_nsec).arg(spec2.tv_sec).arg(spec2.tv_nsec));
    process_image(index,raw.pBuffer,raw.frame.uBytes);
    elapse2 = time.elapsed();
    Log::LogByFile("DVP.txt",QString("%1,GetRaw=%2,SaveImage=%3,sensorTime=%4").arg(CameraIndex).arg(elapse1).arg(elapse2-elapse1).arg(raw.frame.uTimestamp-timeValue));
    return true;
}

void CameraPlayer::run(){
    qDebug()<<"CameraPlayer count="<<count<<" time:"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");

    if (cameraType == CAMERA_TYPE_DVP){
        dvpStatus status;
        dvpStreamState state;
        if (IsValidHandle(dvp_handle)){            
            status = dvpGetStreamState(dvp_handle,&state);
            if (state == STATE_STOPED)
                status = dvpStart(dvp_handle);
        }
    }

    QTime time;
    int elapse;
    for (int i = 1; i <= count; i++){
        if (stoping)
            break;
        time.start();
        qDebug()<<"run"<<CameraIndex<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        if (cameraType == CAMERA_TYPE_CK)
            CKGetFrame(i);
        else if(cameraType == CAMERA_TYPE_DVP)
            DVPGetFrame(i);
        elapse = time.elapsed();

        if (elapse < 500){
            QThread::msleep(500-elapse);
        }
    }

    if (cameraType == CAMERA_TYPE_DVP){
        dvpStop(dvp_handle);
    }
}
