#ifndef USBMODEL_H
#define USBMODEL_H

#include<QAbstractListModel>
#include<QThread>

class UsbNode{
public:
    UsbNode(){}
    UsbNode(bool Dir_, QString Name_){
        bDir = Dir_;
        Name = Name_;
    }
    QString Name;
    bool bDir;
    bool bSelect;
};

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);
    void stopWork(){stop = true;}
    void setList(QList<UsbNode> list){copylist = list;}

signals:
    void Progress(int val);
    void Finish();

public slots:
    void start();

private:
    bool stop;
    QList<UsbNode> copylist;
    bool CopyDir(const QString &fromDir, const QString &toDir, bool cover);
};

class UsbModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum LineInfo_Roles{
        RolesIsDir = Qt::UserRole + 1,
        RolesName,
        RolesSelect
    };

public:
    explicit UsbModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void deleteDir();
    Q_INVOKABLE void copyDir();
    Q_INVOKABLE void stopCopy();
    Q_INVOKABLE void updateSoft();
    Q_INVOKABLE void restart();
    Q_INVOKABLE void switchSelect(int index);

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<UsbNode> m_display_list;
    QHash<int, QByteArray> roles;
    Worker *worker;
    void startCpy();

public slots:
    void Progress(int val);
    void Finish();

signals:
    void copyFinish(int result);

};

#endif // USBMODEL_H
