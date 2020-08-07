#include "testresultmodel.h"
#include "../sqlitemgr.h"
#include "../datahandler.h"

#include<QDebug>

TestResultModel::TestResultModel(QObject *parent):QAbstractListModel (parent)
{    
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

    if(role == RolesPosIndex)
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

void TestResultModel::setCurrItem(int id){
    currItemid = id;
    m_display_list.clear();

    QHash<int,int> posItem = DataHandler::getPosItemid();
    foreach(int posIndex,posItem.keys()){
        if (posItem[posIndex] == id){
            for (size_t i = 0; i < dataPos[posIndex].size(); i++){
                TestResult result;
                result.PosIndex = posIndex;
                result.Itemid = id;
                result.cycle = i+1;
                m_display_list<<result;
            }
        }
    }
}

QVariant TestResultModel::getField(int row,QString field) const{
    if (row>=m_display_list.count())
        return QVariant();

    const TestResult &result = m_display_list[row];

    if(field == "PosIndex")
        return result.PosIndex;
    else if(field == "Itemid")
        return result.Itemid;
    else if(field == "cycle")
        return result.cycle;
    else if(field == "TestValue")
    {
        //qDebug()<<"row:"<<row<<",value:"<<dataPos[result.PosIndex][result.cycle-1].y<<",PosIndex:"<<result.PosIndex<<",cycle:"<<result.cycle;
        return dataPos[result.PosIndex][result.cycle-1].y;        
    }
    return QVariant();
}

void TestResultModel::setTestid(int id){
    Testid = id;
    DataHandler::FillTestData(Testid,dataPos);
    //DataHandler::RefPosFit(dataPos);
    DataHandler::BaseLineFit(dataPos);
}
