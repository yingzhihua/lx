#ifndef WIFIMODEL_H
#define WIFIMODEL_H

#include<QAbstractListModel>

class WifiItem{
public:
    WifiItem(){}
    WifiItem(QString SSID_, int Signal_, int Mode_, bool isConnected_, QString Security_){
        SSID = SSID_;
        Signal = Signal_;
        Mode = Mode_;
        isConnected = isConnected_;
        Security = Security_;
    }
    QString SSID;
    int Signal;
    int Mode;
    bool isConnected;
    QString Security;
};

class WifiConnect : public QObject
{
    Q_OBJECT
public:
    explicit WifiConnect(QObject *parent = nullptr);
    void setParam(QString ssid_,QString password_){ssid = ssid_;password=password_;}
    void setMode(int mode_){mode = mode_;}

signals:
    void Progress(int val);
    void Finish();

public slots:
    void start();

private:
    QString ssid;
    QString password;
    int mode;
};

class WifiModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum LineInfo_Roles{
        RolesSSID = Qt::UserRole + 1,
        RolesSIGNAL,
        RoleMode,
        RoleConnected,
        RoleSecurity
    };
public:
    explicit WifiModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    void LoadData();
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE void connect(QString ssid,QString password);

protected:
    QHash<int, QByteArray> roleNames() const;

public slots:
    void ConnectFinish(int result);

signals:
    void finish(int val);

private:
    QList<WifiItem> m_display_list;
    QHash<int, QByteArray> roles;
    int Exist(QString ssid);
    bool isConnected();
    WifiConnect *connector;
};

#endif // WIFIMODEL_H
