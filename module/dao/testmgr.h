#ifndef TESTMGR_H
#define TESTMGR_H

#include <QObject>

class TestMgr : public QObject
{
    Q_OBJECT
public:
    explicit TestMgr(QObject *parent = nullptr);
    int TestCreate(QString nSerial,QString BoxCode);
    void TestClose(int type);
    void InsertData(int posIndex,int Itemid,int cycle,int value);
signals:

public slots:
private:
    int Testid;
};

#endif // TESTMGR_H
