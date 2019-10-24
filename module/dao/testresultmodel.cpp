#include "testresultmodel.h"

TestResultModel::TestResultModel(QObject *parent):QAbstractListModel (parent)
{
    roles[RolesResultid] = "Resultid";
    roles[RolesPosIndex] = "PosIndex";
    roles[RolesItemid] = "Itemid";
    roles[Rolecycle] = "cycle";
    roles[RoleTestValue] = "TestValue";
}

int TestResultModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_display_list.count();
}

QVariant TestResultModel::data(const QModelIndex &index, int role) const
{
    if (index.row()<0 || index.row()>=m_display_list.count())
        return QVariant();

    const TestResult &result = m_display_list[index.row()];

    if (role == RolesResultid)
        return result.Resultid;
    else if(role == RolesPosIndex)
        return result.PosIndex;
    else if(role == RolesItemid)
        return result.Itemid;
    else if(role == Rolecycle)
        return result.cycle;
    else if(role == RoleTestValue)
        return result.TestValue;
    return QVariant();
}

QHash<int, QByteArray> TestResultModel::roleNames() const
{
    return roles;
}

void TestResultModel::AddTest(const TestResult &result){
    beginInsertRows(QModelIndex(),rowCount(),rowCount());
    m_display_list<<result;
    endInsertRows();
}
