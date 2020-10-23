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
    QString sql = QString("insert into PanelTest(PanelCode,SerialNo,BoxCode,TestTime,SampleInfo,Sampleid,UserName,ResultType,ValidTime,SampleRemark,SampleType) values('%1','%2','%3','%4','%5','%6','%7',%8,'%9','%10',%11)")
            .arg(ExGlobal::panelCode()).arg(nSerial).arg(ExGlobal::reagentBox()).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(ExGlobal::sampleInfo()).arg(ExGlobal::sampleCode())
            .arg(ExGlobal::User).arg(0).arg(ExGlobal::validDateTime.toString("yyyy-MM-dd hh:mm:ss")).arg(ExGlobal::sampleRemark()).arg(ExGlobal::SampleType);
    qDebug()<<sql;
    if(SqliteMgr::execute(sql))
    {
        qDebug()<<"get Test id";
        QSqlQuery query = SqliteMgr::select("select Testid from PanelTest order by Testid desc limit 1");
        //qDebug()<<"get record"<<query.size()<<query.next();
        if (query.next())
        {
            Testid = query.value(0).toInt();
            qDebug()<<"Testid"<<Testid;
        }
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
            .arg(panelCode).arg(0).arg(loopTestCount).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(ExGlobal::sampleInfo()).arg(ExGlobal::sampleCode())
            .arg(ExGlobal::User).arg(0).arg(ExGlobal::validDateTime.toString("yyyy-MM-dd hh:mm:ss"));
    qDebug()<<sql;
    if(SqliteMgr::execute(sql))
    {
        QSqlQuery query = SqliteMgr::select("select Testid from PanelTest order by Testid desc limit 1");
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
        SqliteMgr::execute(sql);
    }
}
void TestMgr::InsertData(int posIndex,int Itemid,int cycle,int value){
    if (Testid != -1){
        QString sql = QString("insert into TestResult(Testid,PosIndex,Itemid,cycle,TestValue) values(%1,%2,%3,%4,%5)")
                .arg(Testid).arg(posIndex).arg(Itemid).arg(cycle).arg(value);
        SqliteMgr::execute(sql);
    }
}

void TestMgr::InsertData(int cycle, QVector<int> value){
    if (Testid != -1){
        QString sql = QString("insert into TestResult(Testid,PosIndex,Itemid,cycle,TestValue,PosValue,PosNum,BgValue,BgNum) values(%1,%2,%3,%4,%5,%6,%7,%8,%9)")
                .arg(Testid).arg(value[6]).arg(value[5]).arg(cycle).arg(value[4]).arg(value[0]).arg(value[1]).arg(value[2]).arg(value[3]);
        SqliteMgr::execute(sql);
    }
}

int TestMgr::TestClose(int type){
    int result = Testid;
    if (Testid != -1){
        QString sql = QString("update PanelTest set ResultType=%1 where Testid=%2").arg(type).arg(Testid);
        SqliteMgr::execute(sql);
        Testid = -1;
    }
    return result;
}

void TestMgr::CheckTest(int testid){
    QSqlQuery query = SqliteMgr::select(QString("select * from PanelTest where Testid=%1").arg(testid));
    if (query.next())
    {
        QString sql = QString("update PanelTest set Checker='%1' where Testid=%2").arg(ExGlobal::User).arg(testid);
        SqliteMgr::execute(sql);
    }
}
