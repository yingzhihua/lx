#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QNetworkConfigurationManager>
#include <QThread>

#include "wifimodel.h"

static void disWifi(){
    if (system("nmcli c > /opt/wific.ini") == 0){
        QFile file("/opt/wific.ini");
        if (!file.open(QIODevice::ReadOnly)){
            qDebug()<<"open file error!";
            return;
        }
        if (file.size()>0){
            QTextStream in(&file);
            QString nameFlag = "NAME";
            QString uuidFlag = "UUID";
            QString typeFlag = "TYPE";
            QString deviceFlag = "DEVICE";
            QString line = in.readLine();
            if (!line.isEmpty()){
                int nameIndex = line.indexOf(nameFlag);
                int uuidIndex = line.indexOf(uuidFlag);
                int typeIndex = line.indexOf(typeFlag);
                int deviceIndex = line.indexOf(deviceFlag);

                line = in.readLine();
                while(!line.isEmpty()){                    
                    QString name = line.mid(nameIndex,(uuidIndex-nameIndex)).simplified();
                    QString uuid = line.mid(uuidIndex,typeIndex-uuidIndex).simplified();
                    QString type = line.mid(typeIndex,deviceIndex-typeIndex).simplified();
                    QString device = line.mid(deviceIndex).simplified();
                    qDebug()<<name<<uuid<<type<<device;
                    if (type.contains("wireless"))
                        system(QString("nmcli c del %1").arg(uuid).toLatin1());
                    line = in.readLine();
                }
            }
        }
        file.close();
    }
}

static bool connectWifi(QString ssid, QString password){
    QString connectNetWorkStr = QString("nmcli dev wifi connect \"%1\" password \"%2\"").arg(ssid).arg(password);
    qDebug()<<connectNetWorkStr;
    QTime time;
    time.start();
    disWifi();
    if (-1 != system(connectNetWorkStr.toLatin1().data())){
        QNetworkConfigurationManager mgr;
        qDebug()<<"wifi connect-0 elapsed="<<time.elapsed();
        if (mgr.isOnline())
            return true;
    }
    qDebug()<<"wifi connect-1 elapsed="<<time.elapsed();
    return false;
}
WifiConnect::WifiConnect(QObject *parent) : QObject(parent)
{
    qDebug()<<"new WifiConnect";
    mode = 0;
}

void WifiConnect::start(){
    if (mode == 0){
        if (connectWifi(ssid,password))
            emit Progress(1);
        else
            emit Progress(0);
    }
    else if (mode == 1){
        disWifi();
        emit Progress(2);
    }
    emit Finish();
}

#define WIFINUM 15
static bool openWifiFunc = true;

WifiModel::WifiModel(QObject *parent):QAbstractListModel (parent)
{
    roles[RolesSSID] = "SSID";
    roles[RolesSIGNAL] = "Signal";
    roles[RoleMode] = "Mode";
    roles[RoleConnected] = "Connected";
    roles[RoleSecurity] = "Security";

    connector = nullptr;
}

void WifiModel::LoadData(){
    refresh();
}

void WifiModel::refresh(){
    if (!openWifiFunc) return;

    m_display_list.clear();

    int result = system("nmcli dev wifi > /opt/wifi.ini");

    if (result == 0){
        QFile file("/opt/wifi.ini");
        if (!file.open(QIODevice::ReadOnly)){
            qDebug()<<"open file error!";
            return;
        }
        if (file.size()>0){
            QTextStream in(&file);
            QString signalFlag = "SIGNAL";
            QString wifiFlag = "SSID";
            QString modeFlag = "MODE";
            QString securityFlag = "SECURITY";
            QString line = in.readLine();
            if (!line.isEmpty()){
                int curLine = 0;
                int signalIndex = line.indexOf(signalFlag);
                int wifiIndex = line.indexOf(wifiFlag);
                int modeIndex = line.indexOf(modeFlag);
                int securityIndex = line.indexOf(securityFlag);
                beginResetModel();
                while(curLine < WIFINUM){
                    line = in.readLine();
                    if (line.isEmpty())
                        break;
                    QString tmpStr = line;
                    QString curConnection = line.left(1);
                    QString wifiName = line.mid(wifiIndex,(modeIndex-wifiIndex)).simplified();
                    int signalValue = tmpStr.mid(signalIndex,3).simplified().toInt();
                    QString security = tmpStr.mid(securityIndex).simplified();
                    qDebug()<<line;

                    int nExist = Exist(wifiName);

                    if (nExist < 0)
                        m_display_list<<WifiItem(wifiName,signalValue,1,(curConnection=="*"),security);
                    else if(curConnection=="*")
                        m_display_list[nExist].isConnected = true;

                    curLine++;
                }
                endResetModel();
            }
        }
    }
}

int WifiModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_display_list.count();
}

QVariant WifiModel::data(const QModelIndex &index, int role) const
{
    if (index.row()<0 || index.row()>=m_display_list.count())
        return QVariant();

    const WifiItem &item = m_display_list[index.row()];

    if (role == RolesSSID)
        return item.SSID;
    else if(role == RolesSIGNAL)
        return item.Signal;
    else if(role == RoleMode)
        return item.Mode;
    else if(role == RoleConnected)
        return item.isConnected;
    else if(role == RoleSecurity)
        return item.Security;

    return QVariant();
}

QHash<int, QByteArray> WifiModel::roleNames() const
{
    return roles;
}

int WifiModel::Exist(QString ssid){
    for (int i = 0; i < m_display_list.size(); i++)
    {
        if (m_display_list[i].SSID == ssid)
            return i;
    }
    return -1;
}

bool WifiModel::isConnected(){
    for (int i = 0; i < m_display_list.size(); i++)
    {
        if (m_display_list[i].isConnected)
            return true;
    }
    return false;
}

void WifiModel::connect(QString ssid, QString password){

    if (connector != nullptr)
        return;
    connector = new WifiConnect;
    QObject::connect(connector,SIGNAL(Progress(int)),this,SLOT(ConnectFinish(int)));
    QThread *thread = new QThread;
    connector->moveToThread(thread);
    QObject::connect(thread,SIGNAL(started()),connector,SLOT(start()));
    QObject::connect(connector,SIGNAL(Finish()),connector,SLOT(deleteLater()));
    QObject::connect(connector,SIGNAL(destroyed()),thread,SLOT(quit()));
    QObject::connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));
    connector->setParam(ssid,password);
    connector->setMode(0);
    thread->start();
}

void WifiModel::disconnect(){
    if (connector != nullptr)
        return;
    connector = new WifiConnect;
    QObject::connect(connector,SIGNAL(Progress(int)),this,SLOT(ConnectFinish(int)));
    QThread *thread = new QThread;
    connector->moveToThread(thread);
    QObject::connect(thread,SIGNAL(started()),connector,SLOT(start()));
    QObject::connect(connector,SIGNAL(Finish()),connector,SLOT(deleteLater()));
    QObject::connect(connector,SIGNAL(destroyed()),thread,SLOT(quit()));
    QObject::connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));
    connector->setMode(1);
    thread->start();
}

void WifiModel::ConnectFinish(int result){
    connector = nullptr;
    refresh();
    if (result == 1 && !isConnected())
        result = 0;
    emit finish(result);
}
