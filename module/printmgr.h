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
    QString checker;
    QString sampInfo;
    QString sampCode;

protected:
    void run();

signals:
    void finishPrint(QByteArray result);

private:
    bool printPDF();
    bool printPTP();

};

#endif // PRINTMGR_H
