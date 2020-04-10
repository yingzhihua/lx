#include "log.h"

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
