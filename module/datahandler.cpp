#include "datahandler.h"
#include "sqlitemgr.h"
#include "exglobal.h"
#include "log.h"
#include <QDir>
#include <QDebug>
#include <QSqlRecord>
#include <QFile>

const static int BaseLineAccuracy = 10;
const static int BaseLineLeft = 8;
const static int BaseLineRight = 13;

static int baseDataIndexStart = 4;
static int baseDataCount = 10;
static int ReferenceValue = 1;

static QHash<int,int> PosId;
static QHash<int,float> PosIntercept;

static QHash<int,Vec2f> baseLine;
static QHash<int,vector<int>> posBG;
static QHash<int,int> bgMean;
static int BGMax;
static QHash<int,vector<Vec8i>> rawData;

static QMap<int,QString> AssayItem;

void DataHandler::setAssayItem(QMap<int, QString> assayItem){
    AssayItem = assayItem;
}

#define FirstPos 2
static void RemoveNoise(QHash<int, vector<Point> > &posArr){
    foreach(int posIndex,posArr.keys())
    {
        QMap<int,int> Eval;
        int EvalMax = FirstPos;
        int EvalMean = 0;
        for(int i = FirstPos; i < posArr[posIndex].size()-1; i++)
        {
            Eval[i] = abs(2*posArr[posIndex][i].y - posArr[posIndex][i+1].y - posArr[posIndex][i-1].y);
            if (Eval[i] > Eval[EvalMax])
                EvalMax = i;
            EvalMean += Eval[i];
        }
        EvalMean /= Eval.size();
        //qDebug()<<"RemoveNoise"<<posIndex<<"Max="<<EvalMax<<"Mean="<<EvalMean<<Eval;

        while((EvalMax<15&&Eval[EvalMax]>EvalMean)||Eval[EvalMax] > 2*EvalMean){
            posArr[posIndex][EvalMax].y = (posArr[posIndex][EvalMax-1].y+posArr[posIndex][EvalMax+1].y)/2;
            if (EvalMax == FirstPos)
                posArr[posIndex][FirstPos-1].y = posArr[posIndex][FirstPos].y*2-posArr[posIndex][FirstPos+1].y;
            if (EvalMax == posArr[posIndex].size()-2)
                posArr[posIndex][EvalMax+1].y = posArr[posIndex][EvalMax].y*2-posArr[posIndex][EvalMax-1].y;

            EvalMax=FirstPos;
            for(int i = FirstPos; i < posArr[posIndex].size()-1; i++)
            {
                Eval[i] = abs(2*posArr[posIndex][i].y - posArr[posIndex][i+1].y - posArr[posIndex][i-1].y);
                if (Eval[i] > Eval[EvalMax])
                    EvalMax = i;
            }
            //qDebug()<<"RemoveNoise"<<posIndex<<"Max="<<EvalMax<<"Mean="<<EvalMean<<Eval;
        }
    }
}

//参考点处理
static void RefPosFit(QHash<int, vector<Point> > &posArr){
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
    QHash<int,int>::iterator pos = PosId.begin();
    while(pos != PosId.end()){
        //if (pos.value() != ReferenceValue)
        {
            for (int i = 0; i < posArr[pos.key()].size(); i++){
                int atten = (posBaseValue[pos.key()]*refDataMean[i]+0.5);
                posArr[pos.key()][i].y -= atten;                
            }
        }
        pos++;
    }
}

static void NormalizeFit(QHash<int, vector<Point> > &posArr){
    foreach(int posIndex, posArr.keys()){
        for (int i = 0; i < posArr[posIndex].size(); i++){
            posArr[posIndex][i].y = posArr[posIndex][i].y*BGMax/bgMean[posIndex];
        }
    }
}

//基线拟合
static void BaseLineFit(QHash<int, vector<Point>> &posArr){
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
#if 1
            {
                tempPoint.assign(points.begin()+5,points.begin()+15);
                fitLine(tempPoint,line_para,cv::DIST_L2,0,1e-2,1e-2);                
                doLine = false;
            }
#endif
            while(doLine){
                doLine = false;
                tempPoint.assign(points.begin()+linebaseStart,points.begin()+linebaseEnd+1);
                fitLine(tempPoint,line_para,cv::DIST_L2,0,1e-2,1e-2);
                if (linebaseEnd>BaseLineRight){
                    double dv = line_para[1]/line_para[0]*(points[linebaseEnd].x-line_para[2])+line_para[3];
                    if (fabs(points[linebaseEnd].y-dv)>BaseLineAccuracy){
                        linebaseEnd--;
                        doLine = true;
                    }
                }
                if (doLine == false && linebaseStart<BaseLineLeft){
                    double dv = line_para[1]/line_para[0]*(points[linebaseStart].x-line_para[2])+line_para[3];
                    if (fabs(points[linebaseStart].y-dv)>BaseLineAccuracy){
                        linebaseStart++;
                        doLine = true;
                    }
                }
            }

            float k = line_para[1]/line_para[0];
            float intercept = k*(0-line_para[2]) + line_para[3];
            baseLine[posIndex][0] = k;
            baseLine[posIndex][1] = intercept;

            for (int i = 0; i < posArr[posIndex].size();i++){
                posArr[posIndex][i].y = posArr[posIndex][i].y - (posArr[posIndex][i].x*k + intercept);
            }
        }
    }
}

static void BaseLineFitEx(QHash<int, vector<Point>> &posArr){
    size_t cycle = 0;
    foreach(int posIndex, posArr.keys()){
        cycle = posArr[posIndex].size();
        if (cycle > 30){
            vector<Point> points = posArr[posIndex];
            vector<Point> tempPoint;
            Vec4f line_para;            
            vector<double> dyArr;
            QString posStr;
            for (int i = 0; i < points.size(); i++)
                posStr.append(","+QString::number(points[i].y));
            qDebug()<<"posArr"<<posStr;
            for (int i = 0; i < cycle-baseDataCount; i++){
                tempPoint.assign(points.begin()+i,points.begin()+i+baseDataCount);
                fitLine(tempPoint,line_para,cv::DIST_L2,0,1e-2,1e-2);
                double dy = 0;
                for (int j = 0; j < tempPoint.size(); j++)
                {
                    double dv = line_para[1]/line_para[0]*(tempPoint[j].x-line_para[2])+line_para[3];
                    dy += fabs(tempPoint[j].y-dv);
                }
                dyArr.push_back(dy);
            }
            int minIndex = distance(dyArr.begin(),min_element(dyArr.begin(),dyArr.end()));
            qDebug()<<"dyArr"<<dyArr<<minIndex;
            double minValue = dyArr.at(minIndex);
            tempPoint.assign(points.begin()+minIndex,points.begin()+minIndex+baseDataCount);
            fitLine(tempPoint,line_para,cv::DIST_L2,0,1e-2,1e-2);
            dyArr.clear();
            for (int j = 0; j < points.size(); j++)
            {
                double dv = line_para[1]/line_para[0]*(points[j].x-line_para[2])+line_para[3];
                dyArr.push_back(fabs(points[j].y-dv));
            }
            qDebug()<<"points="<<dyArr;
            int startIndex,endIndex;
            for (startIndex = minIndex; startIndex > 0; startIndex--)
            {
                if(dyArr.at(startIndex) > minValue)
                    break;
            }
            for (endIndex = minIndex+baseDataCount; endIndex < points.size(); endIndex++)
            {
                if(dyArr.at(endIndex) > minValue)
                    break;
            }
            qDebug()<<"startIndex="<<startIndex<<"endIndex="<<endIndex;
            tempPoint.assign(points.begin()+startIndex,points.begin()+endIndex);
            fitLine(tempPoint,line_para,cv::DIST_L2,0,1e-2,1e-2);

            float k = line_para[1]/line_para[0];
            float intercept = k*(0-line_para[2]) + line_para[3];
            qDebug()<<"k="<<k<<"intercept="<<intercept;
            for (int i = 0; i < posArr[posIndex].size();i++){
                posArr[posIndex][i].y = posArr[posIndex][i].y - (posArr[posIndex][i].x*k + intercept);
            }
            PosIntercept[posIndex] = intercept;
        }
    }
}

static bool PloyCurveFit(vector<Point> &pos, int n, Mat &A){
    int N = pos.size();
    Mat X = Mat::zeros(n+1,n+1,CV_64FC1);
    for (int i = 0; i < n+1; i++)
    {
        for(int j = 0; j < n+1; j++)
        {
            for(int k = 5; k < N; k++)
            {
                X.at<double>(i,j) = X.at<double>(i,j)+pow(pos[k].x,i+j);
            }
        }
    }

    Mat Y = Mat::zeros(n+1,1,CV_64FC1);
    for (int i = 0; i < n+1; i++){
        for (int k = 5; k < N; k++){
            Y.at<double>(i,0) = Y.at<double>(i,0) + pow(pos[k].x, i)*pos[k].y;
        }
    }

    A = Mat::zeros(n+1,1,CV_64FC1);
    solve(X,Y,A,DECOMP_LU);
    return true;
}

static void CurveFit(QHash<int, vector<Point> > &posArr){
    foreach(int posIndex, posArr.keys()){
        Mat A;
        PloyCurveFit(posArr[posIndex],3,A);
        for (int i = 0; i < posArr[posIndex].size(); i++)
        {
            int x = posArr[posIndex][i].x;
            posArr[posIndex][i].y = A.at<double>(0,0)+A.at<double>(1,0)*x+
                    A.at<double>(2,0)*pow(x,2)+A.at<double>(3,0)*pow(x,3)+0.5;
        }

    }
}

static void drawPosArr(cv::String winName, QHash<int, vector<Point> > posArr){
    Mat image = Mat::zeros(480,640,CV_8UC3);    
    foreach (int posIndex, posArr.keys()){
        for (int i = 1; i < posArr[posIndex].size(); i++)
            line(image,posArr[posIndex][i-1],posArr[posIndex][i],Scalar(0,posIndex*2,0),2,8,0);
    }
    flip(image,image,0);
    imshow(winName,image);
}

static void FillTestData(int Testid, QHash<int, vector<Point> > &posArr){
    QSqlQuery query = SqliteMgr::select(QString("select * from TestResult where Testid=%1").arg(Testid));
    int PosIndexNu = query.record().indexOf("PosIndex");
    int ItemidNu = query.record().indexOf("Itemid");
    int cycleNu = query.record().indexOf("cycle");
    int PosValueNu = query.record().indexOf("PosValue");
    int PosNumNu = query.record().indexOf("PosNum");
    int BgValueNu = query.record().indexOf("BgValue");
    int BgNumNu = query.record().indexOf("BgNum");

    posArr.clear();
    PosId.clear();
    PosIntercept.clear();

    rawData.clear();
    posBG.clear();
    BGMax = 0;
    while(query.next()){
        int PosValue = query.value(PosValueNu).toInt();
        int PosNum = query.value(PosNumNu).toInt();
        int BgValue = query.value(BgValueNu).toInt();
        int BgNum = query.value(BgNumNu).toInt();
        int64 value = BgValue;
        value = value*PosNum/BgNum;

        posArr[query.value(PosIndexNu).toInt()].push_back(Point(query.value(cycleNu).toInt()*10,(PosValue - static_cast<int>(value))*10/PosNum));
        PosId[query.value(PosIndexNu).toInt()] = query.value(ItemidNu).toInt();
        posBG[query.value(PosIndexNu).toInt()].push_back(BgValue*10/BgNum);
        rawData[query.value(PosIndexNu).toInt()].push_back(Vec8i(PosValue,PosNum,BgValue,BgNum,(PosValue-static_cast<int>(value))*10/PosNum));
    }

    foreach(int posIndex, posBG.keys()){
        int sum = 0;
        for(int i = 0; i < posBG[posIndex].size(); i++){
            sum += posBG[posIndex][i];
            if (posBG[posIndex][i] > BGMax)
                BGMax = posBG[posIndex][i];
        }
        bgMean[posIndex] = sum/posBG[posIndex].size();
    }
}

static void FillDirectData(int Testid, QHash<int, vector<Point> > &posArr){
    QSqlQuery query = SqliteMgr::select(QString("select * from TestResult where Testid=%1").arg(Testid));
    int PosIndexNu = query.record().indexOf("PosIndex");
    int ItemidNu = query.record().indexOf("Itemid");
    int cycleNu = query.record().indexOf("cycle");
    int TestValueNu = query.record().indexOf("TestValue");

    posArr.clear();
    PosId.clear();
    PosIntercept.clear();

    while(query.next()){
        int TestValue = query.value(TestValueNu).toInt();
        posArr[query.value(PosIndexNu).toInt()].push_back(Point(query.value(cycleNu).toInt()*10,TestValue));
        PosId[query.value(PosIndexNu).toInt()] = query.value(ItemidNu).toInt();
    }    
}

static bool LoadTestData(QString filename, QHash<int, vector<Point> > &posArr){
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)){
        QTextStream in(&file);
        QString line = in.readLine();
        QStringList lineArr = line.split(',');
        QHash<int,int> indexArr;

        posArr.clear();
        PosId.clear();
        rawData.clear();
        posBG.clear();
        BGMax = 0;

        for (int i = 1; i < lineArr.size(); i+=5){
            QStringList recordArr = lineArr[i].split('/');
            PosId[recordArr[2].toInt()] = recordArr[1].toInt();
            indexArr[i] = recordArr[2].toInt();
        }
        qDebug()<<line;
        line = in.readLine();
        int cycleNu = 1;
        while(!line.isEmpty()){
            lineArr = line.split(',');
            qDebug()<<line;
            for (int i = 1; i < lineArr.size(); i+=5){
                int PosValue = lineArr[i].toInt();
                int PosNum = lineArr[i+1].toInt();
                int BgValue = lineArr[i+2].toInt();
                int BgNum = lineArr[i+3].toInt();
                int64 value = BgValue;
                value = value*PosNum/BgNum;

                posArr[indexArr[i]].push_back(Point(cycleNu*10,(PosValue-static_cast<int>(value))*10/PosNum));
                posBG[indexArr[i]].push_back(BgValue*10/BgNum);
                rawData[indexArr[i]].push_back(Vec8i(PosValue,PosNum,BgValue,BgNum,(PosValue-static_cast<int>(value))*10/PosNum));
            }
            line = in.readLine();
            cycleNu++;
        }

        foreach(int posIndex, posBG.keys()){
            int sum = 0;
            for(size_t i = 0; i < posBG[posIndex].size(); i++){
                sum += posBG[posIndex][i];
                if (posBG[posIndex][i] > BGMax)
                    BGMax = posBG[posIndex][i];
            }
            bgMean[posIndex] = sum/posBG[posIndex].size();
        }
        return true;
    }
    return false;
}

static void outputPos(QString filename, QHash<int, vector<Point> > posArr){
    //QFile file(saveDir+"/"+filename+".csv");
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly|QIODevice::Text)){
        QTextStream textStream(&file);
        QString saveStr;
        int cyclecount = 0;
        saveStr = "cycle";
        foreach(int posIndex, PosId.keys()){
            saveStr += ",";
            //saveStr += AssayItem[PosId[posIndex]]+"(R"+QString::number(posIndex/11+1);
            //saveStr += "C"+QString::number(posIndex%11+1)+")";
            saveStr += QString("%1(%2)").arg(ExGlobal::AssayItem[PosId[posIndex]]).arg(posIndex);
            cyclecount = posArr[posIndex].size();
        }
        textStream<<saveStr<<endl;

        for(int i = 0; i < cyclecount; i++){
            saveStr = QString::number(i+1);            
            foreach(int posIndex, PosId.keys()){
                saveStr += ",";
                saveStr += QString::number(posArr[posIndex][i].y);
            }
            textStream<<saveStr<<endl;
        }

        textStream.flush();
        file.close();
    }
}

static void outputPos(QString filename, QHash<int, vector<Vec8i> > posArr){
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly|QIODevice::Text)){
        QTextStream textStream(&file);
        QString saveStr;
        int cyclecount = 0;
        saveStr = "cycle";
        foreach(int posIndex, PosId.keys()){
            //saveStr += QString(",%1(%2-R%3C%4),,,,").arg(ExGlobal::AssayItem[PosId[posIndex]]).arg(posIndex).arg(posIndex/11+1).arg(posIndex%11+1);
            if (PosId[posIndex] == 1)
                saveStr += QString(",%1/%2/%3,,,,").arg("pos").arg(PosId[posIndex]).arg(posIndex);
            else
                saveStr += QString(",%1/%2/%3,,,,").arg(ExGlobal::AssayItem[PosId[posIndex]]).arg(PosId[posIndex]).arg(posIndex);
            cyclecount = posArr[posIndex].size();
        }
        textStream<<saveStr<<endl;

        for(int i = 0; i < cyclecount; i++){
            saveStr = QString::number(i+1);
            foreach(int posIndex, PosId.keys()){
                saveStr += QString(",%1,%2,%3,%4,%5").arg(posArr[posIndex][i][0]).arg(posArr[posIndex][i][1]).arg(posArr[posIndex][i][2]).arg(posArr[posIndex][i][3]).arg(posArr[posIndex][i][4]);
            }
            textStream<<saveStr<<endl;
        }

        textStream.flush();
        file.close();
    }
}

static void outputPos(QString filename, QHash<int, vector<Point> > posArr, QHash<int, Vec2f> baseLineParam){
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly|QIODevice::Text)){
        QTextStream textStream(&file);
        QString saveStr,saveStr1;
        int cyclecount = 0;
        saveStr = "cycle";
        foreach(int posIndex, PosId.keys()){
            saveStr += ",";
            //saveStr += AssayItem[PosId[posIndex]]+"(R"+QString::number(posIndex/11+1);
            //saveStr += "C"+QString::number(posIndex%11+1)+")";
            saveStr += QString("%1(%2)").arg(ExGlobal::AssayItem[PosId[posIndex]]).arg(posIndex);
            cyclecount = posArr[posIndex].size();
        }
        textStream<<saveStr<<endl;

        saveStr = "";
        saveStr1 = "";
        foreach(int posIndex, PosId.keys()){
            saveStr += ",";
            saveStr += QString::number(baseLineParam[posIndex][0]);
            saveStr1 += ",";
            saveStr1 += QString::number(baseLineParam[posIndex][1]);
        }
        textStream<<saveStr<<endl;
        textStream<<saveStr1<<endl;

        for(int i = 0; i < cyclecount; i++){
            saveStr = QString::number(i+1);
            foreach(int posIndex, PosId.keys()){
                saveStr += ",";
                saveStr += QString::number(posArr[posIndex][i].y);
            }
            textStream<<saveStr<<endl;
        }

        textStream.flush();
        file.close();
    }
}

DataHandler::DataHandler()
{
}

QHash<int,int> DataHandler::getPosItemid(){
    return PosId;
}

QHash<int,float> DataHandler::getPosIntercept(){
    return PosIntercept;
}

bool DataHandler::HandleData(int testId, QHash<int, vector<Point>> &posArr){
    FillTestData(testId,posArr);
    RefPosFit(posArr);
    NormalizeFit(posArr);
    RemoveNoise(posArr);
    //CurveFit(posArr);
    BaseLineFit(posArr);
    return true;
}

bool DataHandler::HandleOnePointData(int testId, QHash<int, vector<Point> > &posArr){
    FillDirectData(testId,posArr);
    return true;
}

bool DataHandler::HandleOnePointDataEx(int testId, QHash<int, vector<Point> > &posArr){
    FillDirectData(testId,posArr);
    RemoveNoise(posArr);
    BaseLineFitEx(posArr);
    return true;
}

bool DataHandler::LoadData(QString filename, QHash<int, vector<Point> > &posArr){
    if (LoadTestData(filename,posArr))
    {
        RefPosFit(posArr);
        NormalizeFit(posArr);
        RemoveNoise(posArr);
        BaseLineFit(posArr);
        return true;
    }
    return false;
}

bool DataHandler::SaveData(int testId){
    QHash<int, vector<Point> > posArr;
    QDir dir(Log::getDir()+"/LinuxData");
    if (!dir.exists())
        dir.mkpath(Log::getDir()+"/LinuxData");
    FillTestData(testId,posArr);
    outputPos(Log::getDir()+"/LinuxData/RawData.csv",rawData);
    outputPos(Log::getDir()+"/LinuxData/Raw.csv",posArr);
    RefPosFit(posArr);
    NormalizeFit(posArr);
    RemoveNoise(posArr);
    outputPos(Log::getDir()+"/LinuxData/Normalize.csv",posArr);
    BaseLineFit(posArr);
    outputPos(Log::getDir()+"/LinuxData/Baseline.csv",posArr,baseLine);
    return true;
}

bool DataHandler::SaveOnePointData(int testId){
    QHash<int, vector<Point> > posArr;
    FillDirectData(testId,posArr);
    outputPos(Log::getDir()+"/Raw.csv",posArr);
    return true;
}
