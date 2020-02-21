#ifndef TESTMGR_H
#define TESTMGR_H

#include <QObject>

class TestMgr : public QObject
{
    Q_OBJECT
public:
    explicit TestMgr(QObject *parent = nullptr);
    int TestCreate(QString nSerial);
    int TestClose(int type);
    void InsertData(int posIndex,int Itemid,int cycle,int value);
    int Testid;
signals:

public slots:
private:

};

#endif // TESTMGR_H
