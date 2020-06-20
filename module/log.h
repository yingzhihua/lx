#ifndef LOG_H
#define LOG_H

#include <QString>

typedef enum{
    ERROR_CODE_TEMP_CONNECT=0x101,
    ERROR_CODE_TEMP_DIFF=0x102,
    ERROR_CODE_TEMP_ERR=0x103,
    ERROR_CODE_CTRL_CONNECT=0x201,
    ERROR_CODE_CAM_CONNECT=0x301,
    ERROR_CODE_FILE_READ=0x401,
    ERROR_CODE_FILE_FORMAT=0x402,
    ERROR_CODE_FILE_MODEL=0x403,
    ERROR_CODE_DISK_FULL=0x404
} ERROR_CODE;

typedef enum{
    LOGTYPE_POWERON,
    LOGTYPE_POWERDOWN,
    LOGTYPE_LOGIN,
    LOGTYPE_ERR,
    LOGTYPE_PRINT,
    LOGTYPE_CHECK,
    LOGTYPE_UNCHECK
} LOGTYPE;

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
    static void LogTime(QString log);
    static void LogByFile(QString filename, QString log);

    static bool Logdb(LOGTYPE logType);
    static bool Logdb(LOGTYPE logType, int param1);
    static bool Logdb(LOGTYPE logType, int param1, int param2);
    static bool Logdb(LOGTYPE logType, int param1, QString param2);
    static bool Logdb(LOGTYPE logType, int param1, QString param2, QString param3);
    static bool Logdb(LOGTYPE logType, int param1, int param2, QString param3);
    static bool Logdb(LOGTYPE logType, int param1, int param2, QString param3, QString param4);
};

#endif // LOG_H
