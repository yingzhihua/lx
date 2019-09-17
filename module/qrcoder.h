#ifndef QRCODER_H
#define QRCODER_H

#include <QObject>

class QRcoder : public QObject
{
    Q_OBJECT
public:
    explicit QRcoder(QObject *parent = nullptr);
    static QString QRDecode();

signals:

public slots:
};

#endif // QRCODER_H
