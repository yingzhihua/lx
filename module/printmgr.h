#ifndef PRINTMGR_H
#define PRINTMGR_H
#include <QThread>
#include <QMap>

class printmgr : public QThread
{
    Q_OBJECT
public:
    explicit printmgr(QObject *parent = nullptr);
    QMap<QString,int> itemMap;
    QString testTime;
    QString user;
    QString sampInfo;
    QString sampCode;

protected:
    void run();

signals:
    void finishPrint(QByteArray result);

};

#endif // PRINTMGR_H
