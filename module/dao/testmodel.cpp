#include "testmodel.h"
#include "../exglobal.h"

TestModel::TestModel(QObject *parent):QAbstractListModel (parent)
{
    roles[RolesTestid] = "Testid";
    roles[RoleUser] = "User";
    roles[RoleTestTime] = "TestTime";
    roles[RolesSerialNo] = "SerialNo";
    roles[RoleResultType] = "ResultType";
    roles[RoleSampleInfo] = "SampleInfo";
    roles[RolesPanelCode] = "PanelCode";
}

int TestModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_display_list.count();
}

QVariant TestModel::data(const QModelIndex &index, int role) const
{
    if (index.row()<0 || index.row()>=m_display_list.count())
        return QVariant();

    const Test &test = m_display_list[index.row()];

    if (role == RolesTestid)
        return test.Testid;
    else if(role == RoleUser)
        return test.User;
    else if(role == RoleTestTime)
        return test.TestTime;
    else if(role == RolesSerialNo)
        return test.SerialNo;
    else if(role == RoleResultType)
        return test.ResultType;
    else if(role == RoleSampleInfo)
        return test.SampleInfo;
    else if(role == RolesPanelCode)
        return test.PanelCode;

    return QVariant();
}

QHash<int, QByteArray> TestModel::roleNames() const
{    
    return roles;
}

void TestModel::AddTest(const Test &test){
    beginInsertRows(QModelIndex(),rowCount(),rowCount());
    m_display_list<<test;
    endInsertRows();
    //emit dataChanged(createIndex(index,0),createIndex(index,0));
}

void TestModel::setCurrTest(int TestIndex){
    currTestid = m_display_list[TestIndex].Testid;
    ExGlobal::pTestResultModel->setTestid(currTestid);
}
