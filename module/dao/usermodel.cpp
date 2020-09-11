#include "usermodel.h"
#include "../exglobal.h"
#include "../log.h"

#include<QDebug>
UserModel::UserModel(QObject *parent):QAbstractListModel (parent)
{
    roles[RolesUserid] = "Userid";
    roles[RolesName] = "Name";
    roles[RolesPassword] = "Password";
    roles[RolesDisplayName] = "DisplayName";
    roles[RoleUserType] = "UserType";    
}

bool UserModel::Exist(int id){
    foreach(MUser user, m_display_list){
        if (user.Userid == id)
            return true;
    }
    return false;
}

void UserModel::LoadUser(){
    QSqlQuery query = SqliteMgr::select("select * from User");
    while(query.next())
    {
        MUser user;
        user.Userid = query.value(0).toInt();
        user.Name = query.value(1).toString();
        user.Password = query.value(2).toString();
        user.DisplayName = query.value(3).toString();
        user.UserType = query.value(4).toInt();
        if (!Exist(user.Userid))
            m_display_list<<user;
        qDebug()<<"load user:"<<user.Name<<","<<user.Password<<","<<user.UserType;
    }
}

int UserModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_display_list.count();
}

QVariant UserModel::data(const QModelIndex &index, int role) const
{
    qDebug()<<"UserModel,data:"<<index.row()<<",role:"<<role;
    if (index.row()<0 || index.row()>=m_display_list.count())
        return QVariant();

    const MUser &user = m_display_list[index.row()];

    if (role == RolesUserid)
        return user.Userid;
    else if(role == RolesName)
        return user.Name;
    else if(role == RolesPassword)
        return user.Password;
    else if(role == RolesDisplayName)
        return user.DisplayName;
    else if(role == RoleUserType)
        return user.UserType;

    return QVariant();
}

QHash<int, QByteArray> UserModel::roleNames() const
{
    return roles;
}

bool UserModel::addUser(QString name, QString password, QString displayName, int type){
    //QString sql = "INSERT INTO User (Name, Password, UserType) VALUES ('"+name+"', '"+ password+"', 1)";
    QString sql = QString("INSERT INTO User (Name, Password, DisplayName, UserType) VALUES ('%1', '%2', '%3', %4)").arg(name).arg(password).arg(displayName).arg(type);
    qDebug()<<"addUser:"<<sql;
    QSqlQuery query = SqliteMgr::select("SELECT * FROM User WHERE Name = '"+name+"' COLLATE NOCASE");
    if (query.next())
        return false;
    bool result = SqliteMgr::execute(sql);

    if (result){
        beginInsertRows(QModelIndex(),rowCount(),rowCount());
        LoadUser();
        endInsertRows();
    }
    return result;
}

bool UserModel::updateUser(QString name, QString password, QString displayName, int type){
    QString sql = QString("update User set displayName='%1', Password='%2', UserType=%3 where Name = '%4'").arg(displayName).arg(password).arg(type).arg(name);
    bool result = SqliteMgr::execute(sql);
    if (result){
        beginResetModel();
        for (int i = 0; i < m_display_list.count();i++)
        {
            if (m_display_list[i].Name == name)
            {
                m_display_list[i].Password = password;
                m_display_list[i].DisplayName = displayName;
                m_display_list[i].UserType = type;
            }
        }
        endResetModel();
    }
    return result;
}

bool UserModel::deleteUser(int row){
    if (row < 0 || row >= m_display_list.count())
        return false;
    bool result = SqliteMgr::execute("delete from User where Userid = "+QString::number(m_display_list[row].Userid));
    if (result){
        beginRemoveRows(QModelIndex(),row,row);
        m_display_list.removeAt(row);
        endRemoveRows();
    }
    return result;
}

int UserModel::login(QString name, QString password){
    QString sql = "SELECT * FROM User WHERE Name = '"+name+"' COLLATE NOCASE AND Password = '"+password+"'";
    QSqlQuery query = SqliteMgr::select(sql);
    qDebug()<<"login:"<<sql;

    ExGlobal::User = name;
    while(query.next())
    {        
        ExGlobal::UserType = query.value(4).toInt();
        ExGlobal::DisplayUser = query.value(3).toString();
        Log::Logdb(LOGTYPE_LOGIN,0,name);
        ExGlobal::getPtr()->userChanged();
        ExGlobal::setLogin();
        return 0;
    }

    if (name.toLower() == "admin" && password == ExGlobal::AdminPassword)
    {
        ExGlobal::UserType = 3;
        ExGlobal::DisplayUser = "Admin";
        Log::Logdb(LOGTYPE_LOGIN,0,name);
        ExGlobal::getPtr()->userChanged();
        ExGlobal::setLogin();
        return 0;
    }

    if (name.toLower() == "flashdx" && password == "654321")
    {
        ExGlobal::UserType = 9;
        ExGlobal::DisplayUser = "Flashdx";
        Log::Logdb(LOGTYPE_LOGIN,0,name);
        ExGlobal::getPtr()->userChanged();
        ExGlobal::setLogin();
        return 0;
    }

    if (ExGlobal::projectMode() == 0)
    {
        if (name.length() == 0)
            ExGlobal::User = "user";
        ExGlobal::UserType = 1;
        ExGlobal::DisplayUser = "User";
        ExGlobal::getPtr()->userChanged();
        ExGlobal::setLogin();
        return 0;
    }

    if (name.toLower() == "admin")
        return 1;

    query = SqliteMgr::select("SELECT * FROM User WHERE Name = '"+name+"' COLLATE NOCASE");
    Log::Logdb(LOGTYPE_LOGIN,1,name,password);
    if (query.next())
        return 1;
    return 2;
}

bool UserModel::updatePassword(QString oldpassword, QString newpassword){
    if (ExGlobal::User == "admin")
    {
        if (oldpassword == ExGlobal::AdminPassword)
        {
            ExGlobal::updateTextParam("AdminPassword",newpassword);
            return true;
        }
    }
    else{
        QString sql = "SELECT * FROM User WHERE Name = '"+ExGlobal::User+"' AND Password = '"+oldpassword+"'";
        QSqlQuery query = SqliteMgr::select(sql);

        while(query.next())
        {
            if (SqliteMgr::execute("update User set Password = '"+newpassword+"' where Name = '"+ExGlobal::User+"'"))
                return true;
            else
                return false;
        }
    }
    return false;
}
