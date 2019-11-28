#include "testmgr.h"
#include "../sqlitemgr.h"
#include "../exglobal.h"
#include<QDateTime>
#include<QDebug>

TestMgr::TestMgr(QObject *parent) : QObject(parent)
{
    Testid = -1;
}

int TestMgr::TestCreate(QString nSerial,QString BoxCode){
    QString sql = QString("insert into PanelTest(PanelCode,SerialNo,BoxCode,TestTime,SampleInfo,UserName,ResultType) values('%1','%2','%3','%4','%5','%6',%7)")
            .arg(ExGlobal::panelCode()).arg(nSerial).arg(BoxCode).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg("Sample Info").arg(ExGlobal::User).arg(0);
    qDebug()<<sql;
    if(sqlitemgrinstance->execute(sql))
    {
        QSqlQuery query = sqlitemgrinstance->select("select Testid from PanelTest order by Testid desc limit 1");
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

void TestMgr::InsertData(int posIndex,int Itemid,int cycle,int value){
    if (Testid != -1){
        QString sql = QString("insert into TestResult(Testid,PosIndex,Itemid,cycle,TestValue) values(%1,%2,%3,%4,%5)")
                .arg(Testid).arg(posIndex).arg(Itemid).arg(cycle).arg(value);
        sqlitemgrinstance->execute(sql);
    }
}

void TestMgr::TestClose(int type){
    if (Testid != -1){
        QString sql = QString("update PanelTest set ResultType=%1 where Testid=%2").arg(type).arg(Testid);
        sqlitemgrinstance->execute(sql);
        Testid = -1;
    }
}
