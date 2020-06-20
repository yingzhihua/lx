#include "usermgr.h"
#include "../exglobal.h"

QList<QString> UserMgr::getAll()
{
    QString sql = "SELECT * FROM User";
    QSqlQuery query = SqliteMgr::sqlitemgrinstance->select(sql);
    QList<QString> ql;
    QString result = "";
    while(query.next())
    {
        ql<<query.value(1).toString();
    }
    return ql;
}

bool UserMgr::logon(QString name, QString password)
{
    QString sql = "SELECT * FROM User WHERE Name = '"+name+"' AND Password = '"+password+"' COLLATE NOCASE";
    QSqlQuery query = SqliteMgr::sqlitemgrinstance->select(sql);

    while(query.next())
    {
        ExGlobal::UserType = query.value(4).toInt();
        qDebug()<<"userType"<<ExGlobal::UserType;
        return true;
    }

    return false;
}

bool UserMgr::update(QString name, QString password){
    QString sql = "UPDATE User SET Password = '"+ password+"' WHERE Name = '"+ name +"'";
    return SqliteMgr::sqlitemgrinstance->execute(sql);
}

bool UserMgr::remove(int userid){
    QString sql = "DELETE FROM User WHERE Userid = "+QString::number(userid);
    return SqliteMgr::sqlitemgrinstance->execute(sql);
}

bool UserMgr::insert(User* user){
    QString sql = "INSERT INTO User (Name, Password, UserType) VALUES ('"+user->Name+"', '"+ user->Password+"', "+ QString::number(user->UserType)+")";
    return SqliteMgr::sqlitemgrinstance->execute(sql);
}
