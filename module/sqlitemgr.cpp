#include "sqlitemgr.h"
#include <QtDebug>
#include <QDate>
#include <QSqlRecord>
#include <QSqlError>
#include <QFile>
#include <QCoreApplication>

static QString path = "";

static QString userName;

static QString password;

SqliteMgr::SqliteMgr()
{

}

bool SqliteMgr::conn(QString _path, QString _userName, QString _password)
{
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
           return false;
        }
        else
        {
           qDebug()<<"database open success! " + path;
           return true;
        }
    }
    else
    {
        qDebug()<<"database is already opened! " + path;
        return true;
    }
}

bool SqliteMgr::clear(QString tableName)
{
    QSqlQuery query;
    bool success = query.exec("DELETE FROM "+tableName);
    if(success)
    {
        qDebug()<<QObject::tr("sql delete success!");
        return true;
    }
    else
    {
        qDebug()<<QObject::tr("sql delete fail!");
        return false;
    }
}

bool SqliteMgr::create(QString sql)
{
    QSqlQuery query;
    bool success = query.exec(sql);
    if(success)
    {
        qDebug()<<QObject::tr("sql create success!");
        return true;
    }
    else
    {
        qDebug()<<QObject::tr("sql create fail!");
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
        qDebug()<<QObject::tr("sql execute success!");
        return true;
    }
    else
    {
        QSqlError lastError = query.lastError();
        qDebug()<<QObject::tr("sql execute fail")<<lastError.text();
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

bool SqliteMgr::init()
{
    qDebug()<<"sqlite init: " + path+ " "+ userName +" "+password;
    if(!QString(path).isEmpty())
    {
        this->close();

        QFile file;
        if(file.remove(path))
        {
            qDebug()<<"remove file success " + path;

        }
        else {
            qDebug()<<"remove file fail " + path;

        }

        this->conn(path,userName, password);
        create("CREATE TABLE Assay(Assayid INTEGER PRIMARY KEY NOT NULL , AssayName TEXT NOT NULL, Panelid INT NOT NULL)");
        create("CREATE TABLE AssayItem(Itemid INTEGER PRIMARY KEY NOT NULL , Assayid INT NOT NULL, ItemName TEXT NOT NULL, ResultUnit TEXT NOT NULL, RefValueMin INT NOT NULL, RefValueMax INT NOT NULL)");
        create("CREATE TABLE Log(Logid INTEGER PRIMARY KEY NOT NULL , LogTime TEXT NOT NULL, UserName TEXT NOT NULL, LogType INT NOT NULL, Parm TEXT NOT NULL)");
        create("CREATE TABLE Panel(Panelid INTEGER PRIMARY KEY NOT NULL , PanelName TEXT NOT NULL, PanelCode TEXT NOT NULL)");
        create("CREATE TABLE PanelTest(Testid INTEGER PRIMARY KEY NOT NULL , PanelCode TEXT NOT NULL, SerialNo TEXT NOT NULL, ExpirationDate TEXT NOT NULL, SampleCode TEXT, SampleInfo TEXT, TestTime TEXT, UserName TEXT, ResultType INT)");
        create("CREATE TABLE Parm(Paramid INTEGER PRIMARY KEY NOT NULL , ParmKey TEXT NOT NULL, ParmValue TEXT NOT NULL)");
        create("CREATE TABLE TestResult(Resultid INTEGER PRIMARY KEY NOT NULL , Testid INT NOT NULL, Itemid INT NOT NULL, ResultIndex INT NOT NULL, ResultValue INT NOT NULL)");
        create("CREATE TABLE User(Userid INTEGER PRIMARY KEY NOT NULL , Name TEXT NOT NULL, Password TEXT, UserType INT NOT NULL)");
        create("CREATE TABLE CaliParam(ParamName TEXT NOT NULL, ParamValue INT NOT NULL)");
        return false;
    }
    else {
        return false;
    }
}
