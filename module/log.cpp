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

QString Log::getPosName(int pos){
    QString name = "Ortho";
    if (pos == 1)
        name = "IC";
    else if(pos == 2)
        name = "IFA";
    else if(pos == 3)
        name = "IFB";
    else if(pos == 4)
        name = "RSV-A";
    else if(pos == 5)
        name = "RSV-B";
    else if(pos == 6)
        name = "ADV-1";
    else if(pos == 7)
        name = "ADV-2";
    else if(pos == 8)
        name = "PIV-1";
    else if(pos == 9)
        name = "PIV-2";
    else if(pos == 10)
        name = "PIV-3";
    else if(pos == 11)
        name = "PIV-4";
    else if(pos == 12)
        name = "MP";
    else if(pos == 13)
        name = "BP";
    return name;
}
