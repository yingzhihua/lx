#include "log.h"
#include "sqlitemgr.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDateTime>
#include <QCoreApplication>

static QString dirname="";

Log::Log()
{

}

void Log::setDir(QString dirName)
{
    QDir dir(dirName);
    if (!dir.exists())
        dir.mkpath(dirName);
    dirname = dirName;
}

QString Log::getDir()
{
    if (dirname == "")
    {
        dirname = QCoreApplication::applicationDirPath();
        //dirname = "/share";
    }
    return dirname;
}

void Log::LogWithTime(QString log)
{
    QFile file;

    file.setFileName(getDir()+"/log.txt");
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append))
    {
        qDebug()<<"file open error!";
    }
    QTextStream textStream(&file);

    QDateTime current_time = QDateTime::currentDateTime();
    QString current_time_str = current_time.toString("yyyy-MM-dd hh:mm:ss.zzz ");
    textStream<<current_time_str<<log<<endl;
    textStream.flush();
    file.close();
}

void Log::LogCam(QString log)
{
    QFile file;

    file.setFileName(getDir()+"/camera.txt");
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append))
    {
        qDebug()<<"file open error!";
    }
    QTextStream textStream(&file);

    QDateTime current_time = QDateTime::currentDateTime();
    QString current_time_str = current_time.toString("yyyy-MM-dd hh:mm:ss.zzz ");
    textStream<<current_time_str<<log<<endl;
    textStream.flush();
    file.close();
}

void Log::LogByFile(QString filename, QString log)
{
    QFile file;

    file.setFileName(getDir()+"/"+filename);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append))
    {
        qDebug()<<"file open error!";
    }
    QTextStream textStream(&file);

    QDateTime current_time = QDateTime::currentDateTime();
    QString current_time_str = current_time.toString("yyyy-MM-dd hh:mm:ss.zzz ");
    textStream<<current_time_str<<log<<endl;
    textStream.flush();
    file.close();
}

void Log::LogTemp(QByteArray data)
{
    QFile file;

    file.setFileName(getDir()+"/temp.hex");
    if (!file.open(QIODevice::WriteOnly|QIODevice::Append))
    {
        qDebug()<<"temp file open error!";
    }
    file.write(data);
    file.close();
}

void Log::LogPos(QString log){
    QFile file;

    file.setFileName(getDir()+"/pos.csv");
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append))
    {
        qDebug()<<"file open error!";
    }
    QTextStream textStream(&file);
    textStream<<log<<endl;
    textStream.flush();
    file.close();
}

void Log::LogTime(QString log)
{
    QFile file;

    file.setFileName(getDir()+"/time.txt");
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append))
    {
        qDebug()<<"time file open error!";
    }
    QTextStream textStream(&file);

    QDateTime current_time = QDateTime::currentDateTime();
    QString current_time_str = current_time.toString("yyyy-MM-dd hh:mm:ss.zzz ");
    textStream<<current_time_str<<log<<endl;
    textStream.flush();
    file.close();
}

void Log::LogCData(QString log)
{
    QFile file;

    file.setFileName(QCoreApplication::applicationDirPath()+"/CriticalData.txt");
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append))
    {
        qDebug()<<"Critical file open error!";
    }
    QTextStream textStream(&file);

    QDateTime current_time = QDateTime::currentDateTime();
    QString current_time_str = current_time.toString("yyyy-MM-dd hh:mm:ss.zzz ");
    textStream<<current_time_str<<log<<endl;
    textStream.flush();
    file.close();
}

bool Log::Logdb(LOGTYPE logType){
    QString sql = QString("INSERT INTO Log (LogType, LogTime) VALUES(%1, '%2')").arg(logType).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    return SqliteMgr::sqlitemgrinstance->execute(sql);
}

bool Log::Logdb(LOGTYPE logType, int param1){
    QString sql = QString("INSERT INTO Log (LogType, iParam1, LogTime) VALUES(%1, %2, '%3')").arg(logType).arg(param1).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    return SqliteMgr::sqlitemgrinstance->execute(sql);
}

bool Log::Logdb(LOGTYPE logType, int param1, int param2){
    QString sql = QString("INSERT INTO Log (LogType, LogTime, iParam1, iParam2) VALUES(%1, '%2', %3, %4)").arg(logType).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(param1).arg(param2);
    qDebug()<<"logdb2"<<sql;
    return SqliteMgr::sqlitemgrinstance->execute(sql);
}

bool Log::Logdb(LOGTYPE logType, int param1, QString param2){
    QString sql = QString("INSERT INTO Log (LogType, LogTime, iParam1, tParam1) VALUES(%1, '%2', %3, '%4')").arg(logType).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(param1).arg(param2);
    qDebug()<<"logdb3"<<sql;
    return SqliteMgr::sqlitemgrinstance->execute(sql);
}

bool Log::Logdb(LOGTYPE logType, int param1, int param2, QString param3){
    QString sql = QString("INSERT INTO Log (LogType, LogTime, iParam1, iParam2, tParam1) VALUES(%1, '%2', %3, %4, '%5')").arg(logType).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(param1).arg(param2).arg(param3);
    return SqliteMgr::sqlitemgrinstance->execute(sql);
}

bool Log::Logdb(LOGTYPE logType, int param1, QString param2, QString param3){
    QString sql = QString("INSERT INTO Log (LogType, LogTime, iParam1, tParam1, tParam2) VALUES(%1, '%2', %3, '%4', '%5')").arg(logType).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(param1).arg(param2).arg(param3);
    return SqliteMgr::sqlitemgrinstance->execute(sql);
}

bool Log::Logdb(LOGTYPE logType, int param1, int param2, QString param3, QString param4){
    QString sql = QString("INSERT INTO Log (LogType, LogTime, iParam1, iParam2, tParam1, tParam2) VALUES(%1, '%2', %3, %4, '%5', '%6')").arg(logType).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(param1).arg(param2).arg(param3).arg(param4);
    return SqliteMgr::sqlitemgrinstance->execute(sql);
}
