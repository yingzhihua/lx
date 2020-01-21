#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QNetworkConfigurationManager>
#include "wifimodel.h"

#define WIFINUM 15
static bool openWifiFunc = false;

WifiModel::WifiModel(QObject *parent):QAbstractListModel (parent)
{
    roles[RolesSSID] = "SSID";
    roles[RolesSIGNAL] = "Signal";
    roles[RoleMode] = "Mode";
    roles[RoleConnected] = "Connected";
}

void WifiModel::LoadData(){
    QNetworkConfigurationManager mgr;
    if (!openWifiFunc) return;
    qDebug()<<"Wifi Init,start time:"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz")<<"online:"<<mgr.isOnline();
    int result = system("nmcli dev wifi > /opt/wifi.ini");
    qDebug()<<"end time:"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz")<<",result = "<<result;
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

            QString line = in.readLine();
            if (!line.isEmpty()){
                int curLine = 0;
                int signalIndex = line.indexOf(signalFlag);
                int wifiIndex = line.indexOf(wifiFlag);
                int modeIndex = line.indexOf(modeFlag);
                while(curLine < WIFINUM){
                    line = in.readLine();
                    QString tmpStr = line;
                    QString curConnection = line.left(1);
                    QString wifiName = line.mid(wifiIndex,(modeIndex-wifiIndex)).simplified();
                    int signalValue = tmpStr.mid(signalIndex,3).simplified().toInt();
                    qDebug()<<line;
                    if (!wifiName.isEmpty()){
                        qDebug()<<"wifiName:"<<wifiName<<",curConnection:"<<curConnection<<",signalValue:"<<signalValue;
                        if (!Exist(wifiName)){
                            m_display_list<<WifiItem(wifiName,signalValue,1,(curConnection=="*"));
                        }
                    }

                    curLine++;
                }
            }
        }
        file.close();
    }
}

void WifiModel::refresh(){
    if (!openWifiFunc) return;

    beginResetModel();
    m_display_list.clear();
    endResetModel();

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

            QString line = in.readLine();
            if (!line.isEmpty()){
                int curLine = 0;
                int signalIndex = line.indexOf(signalFlag);
                int wifiIndex = line.indexOf(wifiFlag);
                int modeIndex = line.indexOf(modeFlag);
                while(curLine < WIFINUM){
                    line = in.readLine();
                    QString tmpStr = line;
                    QString curConnection = line.left(1);
                    QString wifiName = line.mid(wifiIndex,(modeIndex-wifiIndex)).simplified();
                    int signalValue = tmpStr.mid(signalIndex,3).simplified().toInt();
                    if (!wifiName.isEmpty()){
                        if (!Exist(wifiName)){
                            beginInsertRows(QModelIndex(),rowCount(),rowCount());
                            m_display_list<<WifiItem(wifiName,signalValue,1,(curConnection=="*"));
                            endInsertRows();
                        }
                    }

                    curLine++;
                }
            }
        }
        file.close();
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

    return QVariant();
}

QHash<int, QByteArray> WifiModel::roleNames() const
{
    return roles;
}

bool WifiModel::Exist(QString ssid){
    foreach(WifiItem item, m_display_list){
        if (item.SSID == ssid)
            return true;
    }
    return false;
}

bool WifiModel::connect(QString ssid, QString password){
    QString connectNetWorkStr = QString("nmcli dev wifi connect %1 password %2").arg(ssid).arg(password);
    qDebug()<<connectNetWorkStr;
    if (-1 != system(connectNetWorkStr.toLatin1().data())){
        QNetworkConfigurationManager mgr;
        if (mgr.isOnline())
            return true;
    }
    return false;
}
