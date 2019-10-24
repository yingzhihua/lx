#ifndef TESTRESULTMODEL_H
#define TESTRESULTMODEL_H

#include<QAbstractListModel>
#include "../entity/testresult.h"

class TestResultModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum LineInfo_Roles{
        RolesResultid = Qt::UserRole + 1,
        RolesPosIndex,
        RolesItemid,
        Rolecycle,
        RoleTestValue
    };
public:
    explicit TestResultModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    void AddTest(const TestResult &result);
    Q_INVOKABLE void setTestid(int id){Testid = id;}
    Q_INVOKABLE int getTestid(){return Testid;}
protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<TestResult> m_display_list;
    QHash<int, QByteArray> roles;
    int Testid;
};

#endif // TESTRESULTMODEL_H
