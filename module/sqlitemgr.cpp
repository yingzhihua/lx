#include "sqlitemgr.h"
#include <QtDebug>
#include <QDate>
#include <QSqlRecord>
#include <QSqlError>
#include <QFile>
#include <QCoreApplication>
#include <QSqlDatabase>

static QString path = "";

static QString userName;

static QString password;

static QSqlDatabase database;

SqliteMgr::SqliteMgr()
{

}

bool SqliteMgr::conn(QString _path, QString _userName, QString _password)
{    
    bool result = false;
    QFile dbFile(_path);
    bool databaseExist = dbFile.exists();
    database =QSqlDatabase::addDatabase("QSQLITE");
    path = _path;
    userName = _userName;
    password = _password;

    database.setDatabaseName(path);
    database.setUserName(userName);
    database.setPassword(password);

    if(!database.isOpen())
    {
        if(!database.open())
        {
           QSqlError err = database.lastError();
           qDebug()<<"database open fail! " + path<<err;
           result = false;
        }
        else
        {
           qDebug()<<"database open success! " + path;
           result = true;
        }
    }
    else
    {
        qDebug()<<"database is already opened! " + path;
        result = true;
    }

    if (result && !databaseExist){
        QFile sqlFile(QCoreApplication::applicationDirPath()+"/db.sql");
        if (sqlFile.open(QIODevice::ReadOnly | QIODevice::Text)){
            QString sql = sqlFile.readAll();
            QStringList sqlarr = sql.split(";");
            for (int i = 0; i < sqlarr.size() - 1; i++)
                qDebug()<<sqlarr[i];
            //execute(sql);
        }
    }
    return result;
}

bool SqliteMgr::clear(QString tableName)
{
    QSqlQuery query;
    bool success = query.exec("DELETE FROM "+tableName);
    if(success)
    {
        qDebug()<<"sql delete success!";
        return true;
    }
    else
    {
        qDebug()<<"sql delete fail!";
        return false;
    }
}

bool SqliteMgr::create(QString sql)
{
    QSqlQuery query;
    bool success = query.exec(sql);
    if(success)
    {
        qDebug()<<"sql create success!";
        return true;
    }
    else
    {
        qDebug()<<"sql create fail!";
        return false;
    }
}

QSqlQuery SqliteMgr::select(QString sql)
{
    QSqlQuery query;

    query.exec(sql);

    return query;
}

bool SqliteMgr::execute(QString sql)
{
    QSqlQuery query;
    bool success = query.exec(sql);
    if(success)
    {
        qDebug()<<"sql execute success!";
        return true;
    }
    else
    {
        QSqlError lastError = query.lastError();
        qDebug()<<"sql execute fail"<<lastError.text();
        return false;
    }
}

bool SqliteMgr::close()
{
    database.close();
    if(!database.isOpen())
    {
        qDebug()<<"database close success!";
        return true;
    }
    else
    {
        qDebug()<<"database close fail!";
        return false;
    }
}

bool SqliteMgr::StartTransations(){
    if (QSqlDatabase::database().transaction())
        return true;
    return false;
}

bool SqliteMgr::EndTransations(){
    if (!QSqlDatabase::database().commit()){
        QSqlDatabase::database().rollback();
        return false;
    }
    return true;
}
