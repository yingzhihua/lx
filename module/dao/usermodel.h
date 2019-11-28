#ifndef USERMODEL_H
#define USERMODEL_H

#include<QAbstractListModel>
#include"../sqlitemgr.h"

class MUser{
public:
    MUser(){}
    MUser(int Userid_, QString Name_, QString Password_, int UserType_){
        Userid = Userid_;
        Name = Name_;
        Password = Password_;
        UserType = UserType_;
    }
    int Userid;
    QString Name;
    QString Password;
    int UserType;
};

class UserModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum LineInfo_Roles{
        RolesUserid = Qt::UserRole + 1,
        RolesName,
        RolesPassword,
        RoleUserType
    };
public:
    explicit UserModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;    
    void LoadUser();
    Q_INVOKABLE bool addUser(QString name,QString password);
    Q_INVOKABLE bool deleteUser(int row);
    Q_INVOKABLE bool login(QString name,QString password);

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<MUser> m_display_list;
    QHash<int, QByteArray> roles;
    bool Exist(int id);
};

#endif // USERMODEL_H