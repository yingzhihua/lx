#include "usermodel.h"
#include "../exglobal.h"

#include<QDebug>
UserModel::UserModel(QObject *parent):QAbstractListModel (parent)
{
    roles[RolesUserid] = "Userid";
    roles[RolesName] = "Name";
    roles[RolesPassword] = "Password";
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
    QSqlQuery query = SqliteMgr::sqlitemgrinstance->select("select * from User");
    while(query.next())
    {
        MUser user;
        user.Userid = query.value(0).toInt();
        user.Name = query.value(1).toString();
        user.Password = query.value(2).toString();
        user.UserType = query.value(3).toInt();
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
    else if(role == RoleUserType)
        return user.UserType;

    return QVariant();
}

QHash<int, QByteArray> UserModel::roleNames() const
{
    return roles;
}

bool UserModel::addUser(QString name, QString password){
    QString sql = "INSERT INTO User (Name, Password, UserType) VALUES ('"+name+"', '"+ password+"', 1)";
    bool result = SqliteMgr::sqlitemgrinstance->execute(sql);

    if (result){
        beginInsertRows(QModelIndex(),rowCount(),rowCount());
        LoadUser();
        endInsertRows();
    }
    return result;
}

bool UserModel::deleteUser(int row){
    if (row < 0 || row >= m_display_list.count())
        return false;
    bool result = SqliteMgr::sqlitemgrinstance->execute("delete from User where Userid = "+QString::number(m_display_list[row].Userid));
    if (result){
        beginRemoveRows(QModelIndex(),row,row);
        m_display_list.removeAt(row);
        endRemoveRows();
    }
    return result;
}

bool UserModel::login(QString name, QString password){
    QString sql = "SELECT * FROM User WHERE Name = '"+name+"' COLLATE NOCASE AND Password = '"+password+"'";
    QSqlQuery query = SqliteMgr::sqlitemgrinstance->select(sql);
    qDebug()<<"login:"<<sql;
    while(query.next())
    {
        return true;
    }

    if (name.toLower() == "admin" && password == ExGlobal::AdminPassword)
        return true;

    if (ExGlobal::projectMode() == 0)
        return true;

    return false;
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
        QSqlQuery query = SqliteMgr::sqlitemgrinstance->select(sql);

        while(query.next())
        {
            if (SqliteMgr::sqlitemgrinstance->execute("update User set Password = '"+newpassword+"' where Name = '"+ExGlobal::User+"'"))
                return true;
            else
                return false;
        }
    }
    return false;
}
