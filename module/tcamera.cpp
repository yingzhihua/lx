#include "tcamera.h"

#include "log.h"
#include "exglobal.h"
#include "imageanalysis.h"
#include <QDebug>


TCamera::TCamera(QObject *parent) : QObject(parent)
{
    cameraType = CAMERA_EMPTY;
    imagetype = 0;
    if (CKCamera::readCamera())
    {
        cameraType = CAMERA_TYPE_CK;
        ckCamera = new CKCamera();
        connect(ckCamera,&CKCamera::finishCapture,this,&TCamera::ActionFinish);
        connect(ckCamera,&CKCamera::reView,this,&TCamera::CameraView);
        qDebug()<<"Open CK Camera";
    }
    else if (ImageCapture::readCamera())
    {
        cameraType = CAMERA_TYPE_V4L2;
        imageCapture = new ImageCapture();
        connect(imageCapture,&ImageCapture::finishCapture,this,&TCamera::ActionFinish);
        connect(imageCapture,&ImageCapture::reView,this,&TCamera::CameraView);
        qDebug()<<"Open V4L2 Camera";
    }
    else
        qDebug()<<"No Camera";
}

int TCamera::openCamera(){
    if (cameraType == CAMERA_TYPE_CK)
        return ckCamera->openCamera();
    else if(cameraType == CAMERA_TYPE_V4L2)
        return imageCapture->openCamera();
    return 1;
}

int TCamera::closeCamera(){
    if (cameraType == CAMERA_TYPE_CK)
        return ckCamera->closeCamera();
    else if(cameraType == CAMERA_TYPE_V4L2)
        return imageCapture->closeCamera();
    return 1;
}

int TCamera::stopCamera(){
    if (cameraType == CAMERA_TYPE_CK)
        return ckCamera->stopCamera();
    else if(cameraType == CAMERA_TYPE_V4L2)
        return imageCapture->stopCamera();
    return 1;
}

bool TCamera::capture(QString fileName, int nCount){
    if (cameraType == CAMERA_TYPE_CK)
        return ckCamera->capture(fileName,nCount);
    else if(cameraType == CAMERA_TYPE_V4L2)
        return imageCapture->capture(fileName,nCount);
    return false;
}

bool TCamera::preview(){
    if (cameraType == CAMERA_TYPE_CK)
        return ckCamera->preview();
    else if(cameraType == CAMERA_TYPE_V4L2)
        return imageCapture->preview();
    return false;
}

int TCamera::getImageType(){
    if (cameraType == CAMERA_TYPE_CK)
        return ckCamera->getImageType();
    return 0;
}
void *TCamera::getyData()
{
    if (cameraType == CAMERA_TYPE_CK)
        return ckCamera->getyData();
    else if(cameraType == CAMERA_TYPE_V4L2)
        return imageCapture->getyData();
    return nullptr;
}

int TCamera::getabsExpose(){
    return ExGlobal::CamAbs;
}

bool TCamera::setabsExpose(int value){
    if (cameraType == CAMERA_TYPE_CK)
        return ckCamera->setabsExpose(value);
    if (cameraType == CAMERA_TYPE_V4L2)
        return imageCapture->setabsExpose(value);
    return false;
}

int TCamera::getGain(){
    return ExGlobal::CamGain;
}

bool TCamera::setGain(int value){
    if (cameraType == CAMERA_TYPE_CK)
        return ckCamera->setGain(value);
    if (cameraType == CAMERA_TYPE_V4L2)
        return imageCapture->setGain(value);
    return false;
}

int TCamera::getWhite(){
    return ExGlobal::CamWhiteBlance;
}

bool TCamera::setWhite(int value){
    if (cameraType == CAMERA_TYPE_CK)
        return ckCamera->setWhite(value);
    if (cameraType == CAMERA_TYPE_V4L2)
        return imageCapture->setWhite(value);
    return true;
}

bool TCamera::saveRaw(QString filename){
    if (cameraType == CAMERA_TYPE_CK)
        return ckCamera->saveRaw(filename);
    return false;
}
