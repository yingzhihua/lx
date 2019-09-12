#ifndef LOG_H
#define LOG_H

#include <QString>
class Log
{
public:
    Log();
    static void setDir(QString dirName);
    static QString getDir();
    static void pLog(QString log);
    static void LogWithTime(QString log);
    static void LogCam(QString log);
    static void LogTemp(QByteArray data);
    static void LogPos(QString log);
    static QString getPosName(int pos);
};

#endif // LOG_H
