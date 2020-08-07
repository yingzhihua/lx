#ifndef PRINTMGR_H
#define PRINTMGR_H
#include <QThread>
#include <QMap>
#include <QImage>

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
    QString printDir;
    QImage flashdxIco;
    QImage hospitalIco;
    bool printPDF();
    bool printPTP();
    QString CreatePDF();
};

#endif // PRINTMGR_H
