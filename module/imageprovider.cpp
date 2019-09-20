#include "imageprovider.h"
#include "qrcoder.h"

#include <QDebug>
#include <QCoreApplication>
ImageProvider::ImageProvider():QQuickImageProvider (QQuickImageProvider::Image)
{

}

QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &resquestSize){    
    Q_UNUSED(size)
    Q_UNUSED(resquestSize)    
    if (id == "view")
        return img;
    else if (id == "qr")
        return QRcoder::img;
    else if (id == "qr2")
        return QRcoder::img2;

    return anaMainImg;
}

QPixmap ImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &resquestSize){    
    Q_UNUSED(size)
    Q_UNUSED(resquestSize)    
    if (id == "view")
        return QPixmap::fromImage(img);
    else if (id == "qr")
        return QPixmap::fromImage(QRcoder::img);
    else if (id == "qr2")
        return QPixmap::fromImage(QRcoder::img2);
    return QPixmap::fromImage(anaMainImg);
}
