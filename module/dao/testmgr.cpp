#include "testmgr.h"
#include "../sqlitemgr.h"
#include "../exglobal.h"
#include<QDateTime>
#include<QDebug>

TestMgr::TestMgr(QObject *parent) : QObject(parent)
{
    Testid = -1;
}

int TestMgr::TestCreate(QString nSerial){
    QString sql = QString("insert into PanelTest(PanelCode,SerialNo,BoxCode,TestTime,SampleInfo,Sampleid,UserName,ResultType) values('%1','%2','%3','%4','%5','%6','%7',%8)")
            .arg(ExGlobal::panelCode()).arg(nSerial).arg(ExGlobal::reagentBox()).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(ExGlobal::sampleInfo()).arg(ExGlobal::sampleCode()).arg(ExGlobal::User).arg(0);
    qDebug()<<sql;
    if(SqliteMgr::sqlitemgrinstance->execute(sql))
    {
        QSqlQuery query = SqliteMgr::sqlitemgrinstance->select("select Testid from PanelTest order by Testid desc limit 1");
        if (query.next())
            Testid = query.value(0).toInt();
        else
            Testid = -1;
    }
    else
        Testid = -1;

    qDebug()<<"Testid="<<Testid;
    return Testid;
}

int TestMgr::LoopTestCreate(QString panelCode, int loopTestCount){
    QString sql = QString("insert into PanelTest(PanelCode,SerialNo,BoxCode,TestTime,SampleInfo,Sampleid,UserName,ResultType) values('%1','%2','%3','%4','%5','%6','%7',%8)")
            .arg(panelCode).arg(0).arg(loopTestCount).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(ExGlobal::sampleInfo()).arg(ExGlobal::sampleCode()).arg(ExGlobal::User).arg(0);
    qDebug()<<sql;
    if(SqliteMgr::sqlitemgrinstance->execute(sql))
    {
        QSqlQuery query = SqliteMgr::sqlitemgrinstance->select("select Testid from PanelTest order by Testid desc limit 1");
        if (query.next())
            Testid = query.value(0).toInt();
        else
            Testid = -1;
    }
    else
        Testid = -1;

    qDebug()<<"Testid="<<Testid;
    return Testid;
}

void TestMgr::LoopTestFinishCycle(int nCycle){
    if (Testid != -1){
        QString sql = QString("update PanelTest set SerialNo='%1' where Testid=%2").arg(nCycle).arg(Testid);
        SqliteMgr::sqlitemgrinstance->execute(sql);
    }
}
void TestMgr::InsertData(int posIndex,int Itemid,int cycle,int value){
    if (Testid != -1){
        QString sql = QString("insert into TestResult(Testid,PosIndex,Itemid,cycle,TestValue) values(%1,%2,%3,%4,%5)")
                .arg(Testid).arg(posIndex).arg(Itemid).arg(cycle).arg(value);
        SqliteMgr::sqlitemgrinstance->execute(sql);
    }
}

int TestMgr::TestClose(int type){
    int result = Testid;
    if (Testid != -1){
        QString sql = QString("update PanelTest set ResultType=%1 where Testid=%2").arg(type).arg(Testid);
        SqliteMgr::sqlitemgrinstance->execute(sql);
        Testid = -1;
    }
    return result;
}
