#include "imageprovider.h"

ImageProvider::ImageProvider():QQuickImageProvider (QQuickImageProvider::Image)
{

}

QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &resquestSize){    
    Q_UNUSED(size)
    Q_UNUSED(resquestSize)
    if (id == "view")
        return img;
    return anaMainImg;
}

QPixmap ImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &resquestSize){    
    Q_UNUSED(size)
    Q_UNUSED(resquestSize)
    if (id == "view")
        return QPixmap::fromImage(img);
    return QPixmap::fromImage(anaMainImg);
}
