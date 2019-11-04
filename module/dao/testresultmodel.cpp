#include "testresultmodel.h"
#include "../sqlitemgr.h"
#include<QDebug>

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

void TestResultModel::setCurrItem(int id){
    currItemid = id;
    m_display_list.clear();
    QString sql = "select * from TestResult where Testid="+QString::number(Testid)+" and Itemid="+QString::number(id);
    qDebug()<<sql;
    QSqlQuery query = sqlitemgrinstance->select(sql);    

    dataPos.clear();
    while(query.next()){
        TestResult result;
        result.Resultid = query.value(0).toInt();
        result.PosIndex = query.value(2).toInt();
        result.Itemid = query.value(3).toInt();
        result.cycle = query.value(4).toInt();
        result.TestValue = query.value(5).toInt();
        m_display_list<<result;        
        //qDebug()<<"setTestid,resultid="<<result.Resultid<<",PosIndex="<<result.PosIndex<<",Itemid="<<result.Itemid<<",cycle="<<result.cycle<<",TestValue="<<result.TestValue;
        dataPos[result.PosIndex].push_back(Point(result.cycle,result.TestValue));
    }

    foreach(int dataKey, dataPos.keys()){
        qDebug()<<"key="<<dataKey<<",length="<<dataPos[dataKey].size();
        if (dataPos[dataKey].size()>30){
            vector<Point> points = dataPos[dataKey];
            vector<Point> tempPoint;
            int linebaseStart = 3;
            int linebaseEnd = points.size()-1;
            Vec4f line_para;
            while (linebaseEnd > 20) {
                tempPoint.assign(points.begin()+linebaseStart,points.begin()+linebaseEnd+1);
                fitLine(tempPoint,line_para,cv::DIST_L2,0,1e-2,1e-2);
                double dv = line_para[1]/line_para[0]*(points[linebaseEnd].x-line_para[2])+line_para[3];
                if (fabs(points[linebaseEnd].y-dv)>1)
                    linebaseEnd--;
                else
                    break;
            }
            while(linebaseStart<10){
                tempPoint.assign(points.begin()+linebaseStart,points.begin()+linebaseEnd+1);
                fitLine(tempPoint,line_para,cv::DIST_L2,0,1e-2,1e-2);
                double dv = line_para[1]/line_para[0]*(points[linebaseStart].x-line_para[2])+line_para[3];
                if (fabs(points[linebaseStart].y-dv)>1)
                    linebaseStart++;
                else
                    break;
            }
            double k = line_para[1]/line_para[0];
            double intercept = k*(0-line_para[2]) + line_para[3];
            for (int i = 0; i < dataPos[dataKey].size(); i++){
                //qDebug()<<"\tvalue="<<a.y<<","<<(a.y - (a.x*k + intercept));
                dataPos[dataKey][i].y = dataPos[dataKey][i].y - (dataPos[dataKey][i].x*k + intercept);
            }
        }
    }
}

QVariant TestResultModel::getField(int row,QString field) const{
    if (row>=m_display_list.count())
        return QVariant();

    const TestResult &result = m_display_list[row];

    if (field == "Resultid")
        return result.Resultid;
    else if(field == "PosIndex")
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

QList<int> TestResultModel::getItemValue(int posIndex){
    QList<int> result;

    return result;
}
