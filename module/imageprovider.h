#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QImage>

class ImageProvider : public QQuickImageProvider
{
public:
    ImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &resquestSize);
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &resquestSize);
    QImage img;
    QImage anaMainImg;
};

#endif // IMAGEPROVIDER_H
