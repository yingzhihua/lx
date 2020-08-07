#include "datahandler.h"
#include "sqlitemgr.h"
#include <QDebug>
#include <QSqlRecord>

static int baseDataIndexStart = 4;
static int baseDataCount = 10;
static int ReferenceValue = 2;

static QHash<int,int> PosId;
static QHash<int,vector<int>> posBG;

DataHandler::DataHandler()
{
}

bool DataHandler::HandleData(int testId, QHash<int, vector<Point> > &posArr){
    FillTestData(testId,posArr);
    RefPosFit(posArr);
    BaseLineFit(posArr);
    return true;
}

//参考点处理
void DataHandler::RefPosFit(QHash<int, vector<Point> > &posArr){
    QHash<int,int> posBaseValue;        //各点基准数据

    //计算各点基准数据，cycle4~cycle13中，去掉两个最大值、两个最小值，剩余六个数的均值。
    foreach(int posIndex,posArr.keys()){
        int MaxValue1 = 0,MaxValue2 = 0,MinValue1 = 0,MinValue2 = 0,sumValue = 0;
        for (int i = baseDataIndexStart; i < baseDataIndexStart+baseDataCount; i++)
        {
            int currValue = posArr[posIndex][i].y;
            int tempValue;
            bool pass = false;

            if (MaxValue1 == 0)
            {
                MaxValue1 = currValue;
                pass = true;
            }
            else if(MaxValue2 == 0)
            {
                MaxValue2 = currValue;
                pass = true;
            }
            else if(currValue > MaxValue1)
            {
                tempValue = MaxValue1;
                MaxValue1 = currValue;
                currValue = tempValue;
            }
            else if(currValue > MaxValue2)
            {
                tempValue = MaxValue2;
                MaxValue2 = currValue;
                currValue = tempValue;
            }

            if (!pass)
            {
                if(MinValue1 == 0)
                {
                    MinValue1 = currValue;
                    pass = true;
                }
                else if(MinValue2 == 0)
                {
                    MinValue2 = currValue;
                    pass = true;
                }
                else if(currValue < MinValue1)
                {
                    tempValue = MinValue1;
                    MinValue1 = currValue;
                    currValue = tempValue;
                }
                else if(currValue < MinValue2)
                {
                    tempValue = MinValue2;
                    MinValue2 = currValue;
                    currValue = tempValue;
                }
            }

            if (!pass)
                sumValue += currValue;
        }
        posBaseValue[posIndex] = sumValue/(baseDataCount-4);
    }

    //计算参考点信号变化率
    QHash<int,int>::iterator ref = PosId.begin();
    QMap<int,float> refDataMean;
    int refSize = 0;
    while(ref != PosId.end()){
        if (ref.value() == ReferenceValue){
            for(int i = 0; i < posArr[ref.key()].size(); i++){
                refDataMean[i] += (float)posArr[ref.key()][i].y/posBaseValue[ref.key()];
            }
            refSize++;
        }
        ref++;
    }
    //计算参考点信号变化率平均值
    foreach(int posIndex,refDataMean.keys()){
        refDataMean[posIndex] /= refSize;
    }

    qDebug()<<"posBaseValue"<<posBaseValue;
    qDebug()<<"refDataMean"<<refDataMean;

    //计算检测点衰减信号
    QHash<int, vector<Point>> realValue;
    QHash<int,int>::iterator pos = PosId.begin();
    while(pos != PosId.end()){
        if (pos.value() != ReferenceValue){
            for (int i = 0; i < posArr[pos.key()].size(); i++){
                int atten = (posBaseValue[pos.key()]*refDataMean[i]+0.5);
                posArr[pos.key()][i].y -= atten;
            }
        }
        pos++;
    }
}

//基线拟合
void DataHandler::BaseLineFit(QHash<int, vector<Point>> &posArr){
    size_t cycle = 0;
    foreach(int posIndex, posArr.keys()){
        cycle = posArr[posIndex].size();
        if (cycle > 30){
            vector<Point> points = posArr[posIndex];
            vector<Point> tempPoint;
            int linebaseStart = 3;
            int linebaseEnd = points.size() - 1;
            Vec4f line_para;
            bool doLine = true;
            while(doLine){
                doLine = false;
                tempPoint.assign(points.begin()+linebaseStart,points.begin()+linebaseEnd+1);
                fitLine(tempPoint,line_para,cv::DIST_L2,0,1e-2,1e-2);
                if (linebaseEnd>20){
                    double dv = line_para[1]/line_para[0]*(points[linebaseEnd].x-line_para[2])+line_para[3];
                    if (fabs(points[linebaseEnd].y-dv)>1){
                        linebaseEnd--;
                        doLine = true;
                    }
                }
                if (doLine == false && linebaseStart<10){
                    double dv = line_para[1]/line_para[0]*(points[linebaseStart].x-line_para[2])+line_para[3];
                    if (fabs(points[linebaseStart].y-dv)>1){
                        linebaseStart++;
                        doLine = true;
                    }
                }
            }

            double k = line_para[1]/line_para[0];
            double intercept = k*(0-line_para[2]) + line_para[3];
            for (int i = 0; i < posArr[posIndex].size();i++){
                posArr[posIndex][i].y = posArr[posIndex][i].y - (posArr[posIndex][i].x*k + intercept);
            }
        }
    }
}

void DataHandler::drawPosArr(cv::String winName, QHash<int, vector<Point> > posArr){
    Mat image = Mat::zeros(480,640,CV_8UC3);    
    foreach (int posIndex, posArr.keys()){
        for (int i = 1; i < posArr[posIndex].size(); i++)
            line(image,posArr[posIndex][i-1],posArr[posIndex][i],Scalar(0,posIndex*2,0),2,8,0);
    }
    flip(image,image,0);
    imshow(winName,image);
}

void DataHandler::FillTestData(int Testid, QHash<int, vector<Point> > &posArr){
    QSqlQuery query = SqliteMgr::sqlitemgrinstance->select(QString("select * from TestResult where Testid=%1").arg(Testid));
    int PosIndexNu = query.record().indexOf("PosIndex");
    int ItemidNu = query.record().indexOf("Itemid");
    int cycleNu = query.record().indexOf("cycle");
    int PosValueNu = query.record().indexOf("PosValue");
    int PosNumNu = query.record().indexOf("PosNum");
    int BgValueNu = query.record().indexOf("BgValue");
    int BgNumNu = query.record().indexOf("BgNum");

    posArr.clear();
    while(query.next()){
        int PosValue = query.value(PosValueNu).toInt();
        int PosNum = query.value(PosNumNu).toInt();
        int BgValue = query.value(BgValueNu).toInt();
        int BgNum = query.value(BgNumNu).toInt();
        int64 value = BgValue;
        value = value*PosNum/BgNum;

        posArr[query.value(PosIndexNu).toInt()].push_back(Point(query.value(cycleNu).toInt()*10,(PosValue - static_cast<int>(value))*10/PosNum));
        PosId[query.value(PosIndexNu).toInt()] = query.value(ItemidNu).toInt();
        posBG[query.value(PosIndexNu).toInt()].push_back(BgValue);
    }
}

QHash<int,int> DataHandler::getPosItemid(){
    return PosId;
}
