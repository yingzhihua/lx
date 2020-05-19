#include "testmodel.h"
#include "../exglobal.h"
#include "../sqlitemgr.h"
#include<QDebug>
TestModel::TestModel(QObject *parent):QAbstractListModel (parent)
{
    roles[RolesTestid] = "Testid";
    roles[RoleUser] = "User";
    roles[RoleChecker] = "Checker";
    roles[RoleTestTime] = "TestTime";
    roles[RolesSerialNo] = "SerialNo";
    roles[RoleResultType] = "ResultType";
    roles[RoleSampleInfo] = "SampleInfo";
    roles[RoleSampleId] = "SampleId";
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
    else if(role == RoleChecker)
        return test.Checker;
    else if(role == RoleTestTime)
        return test.TestTime;
    else if(role == RolesSerialNo)
        return test.SerialNo;
    else if(role == RoleResultType)
        return test.ResultType;
    else if(role == RoleSampleInfo)
        return test.SampleInfo;
    else if(role == RoleSampleId)
        return test.SampleId;
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

bool TestModel::ExistTest(int Testid){
    foreach(Test test, m_display_list){
        if (test.Testid == Testid)
            return true;
    }
    return false;
}

void TestModel::setCurrTest(int TestIndex){
    currTestIndex = TestIndex;
    currTestid = m_display_list[TestIndex].Testid;
    ExGlobal::setBoxSerial(m_display_list[TestIndex].SerialNo);
    ExGlobal::pTestResultModel->setTestid(currTestid);
    qDebug()<<"setCurrTest:"<<currTestid;
}

bool TestModel::mayCheck(){
    qDebug()<<"mayCheck,"<<ExGlobal::UserType<<"Checker:"<<m_display_list[currTestIndex].Checker;
    if ((ExGlobal::UserType & 0x2) && m_display_list[currTestIndex].Checker.length() == 0)
        return true;
    return false;
}

void TestModel::checkTest(){
    QSqlQuery query = SqliteMgr::sqlitemgrinstance->select(QString("select * from PanelTest where Testid=%1").arg(currTestid));
    if (query.next())
    {
        QString sql = QString("update PanelTest set Checker='%1' where Testid=%2").arg(ExGlobal::User).arg(currTestid);
        SqliteMgr::sqlitemgrinstance->execute(sql);
        m_display_list[currTestIndex].Checker = ExGlobal::User;
    }
    emit dataChanged(createIndex(currTestIndex,0),createIndex(currTestIndex,0));
}
