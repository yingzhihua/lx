#include "testmodel.h"
#include "../exglobal.h"
#include "../sqlitemgr.h"
#include "../sequence.h"

#include<QDebug>
TestModel::TestModel(QObject *parent):QAbstractListModel (parent)
{
    roles[RolesTestid] = "Testid";
    roles[RoleUser] = "User";
    roles[RoleChecker] = "Checker";
    roles[RoleTestTime] = "TestTime";
    roles[RoleValidTime] = "ValidTime";
    roles[RolesSerialNo] = "SerialNo";
    roles[RolesBoxCode] = "BoxCode";
    roles[RoleResultType] = "ResultType";
    roles[RoleSampleInfo] = "SampleInfo";
    roles[RoleSampleId] = "SampleId";
    roles[RolesPanelCode] = "PanelCode";
    roles[RolesPanelName] = "PanelName";
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
    else if(role == RoleValidTime)
        return test.ValidTime;
    else if(role == RolesSerialNo)
        return test.SerialNo;
    else if(role == RolesBoxCode)
        return test.BoxCode;
    else if(role == RoleResultType)
        return test.ResultType;
    else if(role == RoleSampleInfo)
        return test.SampleInfo;
    else if(role == RoleSampleId)
        return test.SampleId;
    else if(role == RolesPanelCode)
        return test.PanelCode;
    else if(role == RolesPanelName)
        return test.PanelName;

    return QVariant();
}

QHash<int, QByteArray> TestModel::roleNames() const
{    
    return roles;
}

void TestModel::InitTest(){
    m_display_list.clear();
    //QString sql = "select * from PanelTest where ResultType = 2 order by Testid DESC";
    QString sql = "select * from PanelTest order by Testid DESC";
    QSqlQuery query = SqliteMgr::select(sql);
    while(query.next()){
        Test test;
        test.Testid = query.value(0).toInt();
        test.PanelCode = query.value(1).toString();
        test.SerialNo = query.value(2).toString();
        test.BoxCode = query.value(3).toString();
        test.TestTime = query.value(4).toString();
        test.ValidTime = query.value(10).toString();
        test.SampleInfo = query.value(5).toString();
        test.SampleId = query.value(6).toString();
        test.User = query.value(7).toString();
        test.Checker = query.value(8).toString();
        test.ResultType = query.value(9).toInt();
        test.PanelName = Sequence::getPanelName(test.PanelCode);
        m_display_list<<test;
    }
}
void TestModel::AddTest(int testid){
    if (!ExistTest(testid)){
        QString sql = QString("select * from PanelTest where ResultType = 2 and Testid = %1 order by Testid DESC").arg(testid);
        QSqlQuery query = SqliteMgr::select(sql);
        beginInsertRows(QModelIndex(),rowCount(),rowCount());
        if (query.next()){
            Test test;
            test.Testid = query.value(0).toInt();
            test.PanelCode = query.value(1).toString();
            test.SerialNo = query.value(2).toString();
            test.BoxCode = query.value(3).toString();
            test.TestTime = query.value(4).toString();
            test.SampleInfo = query.value(5).toString();
            test.SampleId = query.value(6).toString();
            test.User = query.value(7).toString();
            test.Checker = query.value(8).toString();
            test.ResultType = query.value(9).toInt();
            test.ValidTime = query.value(10).toString();
            test.PanelName = Sequence::getPanelName(test.PanelCode);
            m_display_list<<test;
        }
        currTestIndex = m_display_list.count() - 1;
        currTestid = m_display_list[currTestIndex].Testid;
        endInsertRows();
    }
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
    ExGlobal::pTestResultModel->setTestid(currTestid,m_display_list[TestIndex].PanelCode);
    qDebug()<<"setCurrTest:"<<currTestid;
}

bool TestModel::mayCheck(){
    qDebug()<<"mayCheck,"<<ExGlobal::UserType<<"Checker:"<<m_display_list[currTestIndex].Checker;
    if ((ExGlobal::UserType & 0x2)/* && m_display_list[currTestIndex].Checker.length() == 0*/)
        return true;
    return false;
}

bool TestModel::haveCheck(){
    return (m_display_list[currTestIndex].Checker.length() != 0);
}

void TestModel::checkTest(){    
    QSqlQuery query = SqliteMgr::select(QString("select * from PanelTest where Testid=%1").arg(currTestid));
    if (query.next())
    {
        QString sql = QString("update PanelTest set Checker='%1' where Testid=%2").arg(ExGlobal::User).arg(currTestid);
        SqliteMgr::execute(sql);
        m_display_list[currTestIndex].Checker = ExGlobal::User;
    }
    emit dataChanged(createIndex(currTestIndex,0),createIndex(currTestIndex,0));
}

void TestModel::uncheckTest(){
    QSqlQuery query = SqliteMgr::select(QString("select * from PanelTest where Testid=%1").arg(currTestid));
    if (query.next())
    {
        QString sql = QString("update PanelTest set Checker='' where Testid=%1").arg(currTestid);
        SqliteMgr::execute(sql);
        m_display_list[currTestIndex].Checker = "";
    }
    emit dataChanged(createIndex(currTestIndex,0),createIndex(currTestIndex,0));
}
