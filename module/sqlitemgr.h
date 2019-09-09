#ifndef SQLITEMGR_H
#define SQLITEMGR_H
#include <QQmlEngine>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QCoreApplication>
#include <QDebug>
class SqliteMgr : public QObject
{
    Q_OBJECT
public:

    SqliteMgr();

    Q_INVOKABLE bool init();

    Q_INVOKABLE bool conn(QString path, QString userName, QString password);

    Q_INVOKABLE bool create(QString sql);

    Q_INVOKABLE QSqlQuery select(QString sql);

    Q_INVOKABLE bool execute(QString sql);

    Q_INVOKABLE bool close();

    Q_INVOKABLE bool clear(QString tableName);



public slots:


private:

    QSqlDatabase database;

};

static SqliteMgr * sqlitemgrinstance;

static QObject *sqlitemgr_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    if(sqlitemgrinstance == nullptr)
    {
        qDebug()<<"inner sqlitemgrinstance is nullptr";
    }
    else
    {
        qDebug()<<"inner sqlitemgrinstance is not nullptr";
    }
    return sqlitemgrinstance;
}

#endif // SQLITEMGR_H
