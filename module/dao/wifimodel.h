#ifndef WIFIMODEL_H
#define WIFIMODEL_H

#include<QAbstractListModel>

class WifiItem{
public:
    WifiItem(){}
    WifiItem(QString SSID_, int Signal_, int Mode_, bool isConnected_){
        SSID = SSID_;
        Signal = Signal_;
        Mode = Mode_;
        isConnected = isConnected_;
    }
    QString SSID;
    int Signal;
    int Mode;
    bool isConnected;
};

class WifiModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum LineInfo_Roles{
        RolesSSID = Qt::UserRole + 1,
        RolesSIGNAL,
        RoleMode,
        RoleConnected
    };
public:
    explicit WifiModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    void LoadData();
    Q_INVOKABLE void refresh();
    Q_INVOKABLE bool connect(QString ssid,QString password);

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<WifiItem> m_display_list;
    QHash<int, QByteArray> roles;
    bool Exist(QString ssid);
};

#endif // WIFIMODEL_H
