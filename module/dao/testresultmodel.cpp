#include "testresultmodel.h"
#include "../sqlitemgr.h"
#include "../datahandler.h"
#include "../exglobal.h"

#include<QDebug>
static QHash<int,int> PosId;

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

void TestResultModel::setTestid(int id,QString panelCode){
    Testid = id;
    if (panelCode == ExGlobal::DemoPanelCode)
        DataHandler::LoadData(QCoreApplication::applicationDirPath()+"/RawData.csv",dataPos);
    else
        DataHandler::HandleData(Testid,dataPos);
    PosId = DataHandler::getPosItemid();
}

int TestResultModel::getItemResult(int testid, int itemid){
    if (testid == Testid)
    {
        foreach(int dataKey, PosId.keys()){
            if (PosId[dataKey] == itemid && dataPos[dataKey].size()>20){
                for(int i = 10; i < dataPos[dataKey].size(); i++)
                    if (dataPos[dataKey][i].y > ExGlobal::getItemCT(itemid))
                        return i;
            }
        }
    }
    return 0;
}

QList<int> TestResultModel::getCurrItemResult(){
    QList<int> result;
    foreach(int dataKey, PosId.keys()){
        if (PosId[dataKey] == currItemid && dataPos[dataKey].size()>20){
            int t_result = 0;
            int ct = ExGlobal::getItemCT(currItemid);
            for(int i = 10; i < dataPos[dataKey].size(); i++)
                if (dataPos[dataKey][i].y > ct)
                {
                    if (i == 10)
                        t_result = 110;
                    else if(dataPos[dataKey][i].y == dataPos[dataKey][i-1].y)
                        t_result = (i+1)*10;
                    else
                        t_result = i*10+((ct-dataPos[dataKey][i-1].y)*10/(dataPos[dataKey][i].y-dataPos[dataKey][i-1].y));
                    break;
                }
            t_result += dataKey*1000;
            result<<t_result;
        }
    }
    std::sort(result.begin(),result.end());
    return result;
}
