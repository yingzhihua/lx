#ifndef SQLITEMGR_H
#define SQLITEMGR_H

#include <QSqlQuery>
#include <QString>

class SqliteMgr
{    
public:

    SqliteMgr();

    static bool conn(QString path, QString userName, QString password);

    static bool create(QString sql);

    static QSqlQuery select(QString sql);

    static bool execute(QString sql);

    static bool close();

    static bool clear(QString tableName);

    static bool StartTransations();
    static bool EndTransations();

};

#endif // SQLITEMGR_H
