#include "datahandler.h"
#include "sqlitemgr.h"
#include "exglobal.h"

#include <QDebug>
#include <QSqlRecord>
#include <QFile>

static int baseDataIndexStart = 4;
static int baseDataCount = 10;
static int ReferenceValue = 1;

static QHash<int,int> PosId;
static QHash<int,Vec2f> baseLine;
static QHash<int,vector<int>> posBG;

static QString saveDir;
void DataHandler::setSaveDir(QString dir){
    saveDir = dir;
}

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

            {
                tempPoint.assign(points.begin()+7,points.begin()+27);
                fitLine(tempPoint,line_para,cv::DIST_L2,0,1e-2,1e-2);
                double dvalue1 = 0;
                double dvalue2 = 0;
                double dvalue3 = 0;
                int value1 = 0;
                int value2 = 0;
                int value3 = 0;
                for (int i = 0; i < tempPoint.size(); i++){
                    double dv = line_para[1]/line_para[0]*(tempPoint[i].x-line_para[2])+line_para[3];
                    double tempdv;
                    dv = fabs(tempPoint[i].y-dv);
                    if (dv > dvalue1)
                    {
                        tempdv = dvalue1;
                        dvalue1 = dv;
                        dv = tempdv;
                        value1 = tempPoint[i].y;
                    }
                    if (dv > dvalue2)
                    {
                        tempdv = dvalue2;
                        dvalue2 = dv;
                        dv = tempdv;
                        value2 = tempPoint[i].y;
                    }
                    if (dv > dvalue3)
                    {
                        tempdv = dvalue3;
                        dvalue3 = dv;
                        dv = tempdv;
                        value3 = tempPoint[i].y;
                    }
                }
                if (posIndex == 82)
                qDebug()<<"value1="<<value1<<"value2="<<value2<<"value3="<<value3;
                vector<Point>::iterator ite = tempPoint.begin();
                while(ite != tempPoint.end()){
                    if (value1 != 0 && ite->y == value1)
                        ite = tempPoint.erase(ite);
                    else if (value2 != 0 && ite->y == value2)
                        ite = tempPoint.erase(ite);
                    else if (value3 != 0 && ite->y == value3)
                        ite = tempPoint.erase(ite);
                    else
                        ++ite;
                }
                fitLine(tempPoint,line_para,cv::DIST_L2,0,1e-2,1e-2);
                doLine = false;
            }

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

bool PloyCurveFit(vector<Point> &pos, int n, Mat &A){
    int N = pos.size();
    Mat X = Mat::zeros(n+1,n+1,CV_64FC1);
    for (int i = 0; i < n+1; i++)
    {
        for(int j = 0; j < n+1; j++)
        {
            for(int k = 0; k < N; k++)
            {
                X.at<double>(i,j) = X.at<double>(i,j)+pow(pos[k].x,i+j);
            }
        }
    }

    Mat Y = Mat::zeros(n+1,1,CV_64FC1);
    for (int i = 0; i < n+1; i++){
        for (int k = 0; k < N; k++){
            Y.at<double>(i,0) = Y.at<double>(i,0) + pow(pos[k].x, i)*pos[k].y;
        }
    }

    A = Mat::zeros(n+1,1,CV_64FC1);
    solve(X,Y,A,DECOMP_LU);
    return true;
}

void DataHandler::CurveFit(QHash<int, vector<Point> > &posArr){
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

void DataHandler::outputPos(QString filename, QHash<int, vector<Point> > posArr){
    QFile file(saveDir+"/"+filename+".csv");
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

void DataHandler::outputPos(QString filename, QHash<int, vector<Vec8i> > posArr){
    QFile file(saveDir+"/"+filename+".csv");
    if (file.open(QIODevice::WriteOnly|QIODevice::Text)){
        QTextStream textStream(&file);
        QString saveStr;
        int cyclecount = 0;
        saveStr = "cycle";
        foreach(int posIndex, PosId.keys()){
            saveStr += QString(",%1(%2-R%3C%4),,,,").arg(ExGlobal::AssayItem[PosId[posIndex]]).arg(posIndex).arg(posIndex/11+1).arg(posIndex%11+1);
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

void DataHandler::outputPos(QString filename, QHash<int, vector<Point> > posArr, QHash<int, Vec2f> baseLineParam){
    QFile file(saveDir+"/"+filename+".csv");
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
