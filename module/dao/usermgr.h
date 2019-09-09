#ifndef USERMRG_H
#define USERMRG_H

#include <QObject>
#include"../sqlitemgr.h"
#include"../entity/User.h"

class UserMgr : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE QList<QString> getAll();

    Q_INVOKABLE bool logon(QString name, QString password);

    Q_INVOKABLE bool update(QString name, QString password);

    Q_INVOKABLE bool remove(int userid);

    Q_INVOKABLE bool insert(User* user);

signals:

public slots:
};


static UserMgr * usermgrinstance = nullptr;

static QObject *usermgr_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    return usermgrinstance;
}

#endif // USERMRG_H
