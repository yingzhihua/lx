#ifndef TESTMGR_H
#define TESTMGR_H

#include <QObject>

class TestMgr : public QObject
{
    Q_OBJECT
public:
    explicit TestMgr(QObject *parent = nullptr);
    int TestCreate(QString nSerial);
    int LoopTestCreate(QString panelCode, int loopTestCount);
    void LoopTestFinishCycle(int nCycle);
    int TestClose(int type);
    void InsertData(int posIndex,int Itemid,int cycle,int value);
    void CheckTest(int testid);
    int Testid;
signals:

public slots:
private:

};

#endif // TESTMGR_H
