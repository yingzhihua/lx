#include "exglobal.h"
#include "sequence.h"
#include <QDebug>
#include "sqlitemgr.h"
#include <QCoreApplication>
#include "serialmgr.h"
#include <QNetworkInterface>

QString ExGlobal::t_panelCode = "012315";
QString ExGlobal::t_panelName = "上呼吸道测试";
QString ExGlobal::t_sampleCode = "SLX 01079";
QString ExGlobal::t_sampleInfo = "华山11";
QString ExGlobal::t_user = "";
bool ExGlobal::test = false;

QString ExGlobal::t_sysName = "样机02";
int ExGlobal::t_sysLanguageCode = 1;

QString ExGlobal::t_version = "V1.13";
QString ExGlobal::temp_version = "V0.00";
QString ExGlobal::ctrl_version = "V0.00";

int ExGlobal::V1WorkX = 9166;
int ExGlobal::V2WorkX = 8178;
int ExGlobal::V3WorkX = 5686;
int ExGlobal::VPWorkX = 9043;
int ExGlobal::V1SoftHomeX = 5890;
int ExGlobal::V2SoftHomeX = 4524;
int ExGlobal::V3SoftHomeX = 2410;
int ExGlobal::VPSoftHomeX = 5389;
int ExGlobal::VDWorkX = 2000;
int ExGlobal::VDSoftHomeX = 1000;

int ExGlobal::V1WorkOffset = 3276;
int ExGlobal::V2WorkOffset = 3654;
int ExGlobal::V3WorkOffset = 3276;
int ExGlobal::VPWorkOffset = 3654;
int ExGlobal::V1SoftHomeOffset = 2520;
int ExGlobal::V2SoftHomeOffset = 2520;
int ExGlobal::V3SoftHomeOffset = 2520;
int ExGlobal::VPSoftHomeOffset = 2520;
int ExGlobal::V1ToolHomeX = 9166;
int ExGlobal::V2ToolHomeX = 8178;
int ExGlobal::V3ToolHomeX = 5686;
int ExGlobal::VPToolHomeX = 9043;

int ExGlobal::CamAbs = 500;
int ExGlobal::CamGain = 0;
int ExGlobal::CamFocus = 0;

int ExGlobal::PumpSoftHomeX = 0;
int ExGlobal::PumpToolHomeX = 0;
int ExGlobal::PumpSoftHomeOffset = 0;

ExGlobal::ExGlobal(QObject *parent) : QObject(parent)
{
    //qDebug()<<"ExGlobal";
}

static QString getWMIC(const QString &cmd){
    //cpu name: wmic cpu get Name
    QProcess p;
    p.start(cmd);
    p.waitForFinished();
    QString result = QString::fromLocal8Bit(p.readAllStandardOutput());
    QStringList list = cmd.split(" ");
    result = result.remove(list.last(),Qt::CaseInsensitive);
    result = result.replace("\r","");
    result = result.replace("\n","");
    result = result.simplified();
    return result;
}

void ExGlobal::exClose(){
    //cpu name: wmic cpu get Name
    qDebug()<<"exClose";
    //QProcess p;
    //p.start("halt");
    system("shutdown -t 0");
}

QString ExGlobal::getIP(){
    foreach(const QHostAddress& hostAddress,QNetworkInterface::allAddresses())
        if(hostAddress != QHostAddress::LocalHost && hostAddress.toIPv4Address())
            return hostAddress.toString();
    return "127.0.0.1";
}

void ExGlobal::exInit()
{
    CaliParamInit();
    SerialMgr::SerialInit();    
    //Sequence::sequenceInit();

    if(sqlitemgrinstance == nullptr)
    {
        qDebug()<<"global sqlitemgrinstance is nullptr";
    }
    else
    {
        qDebug()<<"global sqlitemgrinstance is not nullptr";
    }
}

void ExGlobal::CaliParamInit()
{
    QString sql = "SELECT * FROM CaliParam";
    QSqlQuery query = sqlitemgrinstance->select(sql);

    while(query.next())
    {
        SetCaliParam(query.value(0).toString(),query.value(1).toInt());
    }
}

void ExGlobal::SetCaliParam(const QString &name, int caliValue)
{
    if (name == "V1WorkX")
        V1WorkX = caliValue;
    else if (name == "V2WorkX")
        V2WorkX = caliValue;
    else if (name == "V2WorkX")
        V2WorkX = caliValue;
    else if (name == "V3WorkX")
        V3WorkX = caliValue;
    else if (name == "VPWorkX")
        VPWorkX = caliValue;
    else if (name == "V1SoftHomeX")
        V1SoftHomeX = caliValue;
    else if (name == "V2SoftHomeX")
        V2SoftHomeX = caliValue;
    else if (name == "V3SoftHomeX")
        V3SoftHomeX = caliValue;
    else if (name == "VPSoftHomeX")
        VPSoftHomeX = caliValue;
    else if (name == "V1SoftHomeOffset")
        V1SoftHomeOffset = caliValue;
    else if (name == "V2SoftHomeOffset")
        V2SoftHomeOffset = caliValue;
    else if (name == "V3SoftHomeOffset")
        V3SoftHomeOffset = caliValue;
    else if (name == "VPSoftHomeOffset")
        VPSoftHomeOffset = caliValue;
    else if (name == "V1ToolHomeX")
        V1ToolHomeX = caliValue;
    else if (name == "V2ToolHomeX")
        V2ToolHomeX = caliValue;
    else if (name == "V3ToolHomeX")
        V3ToolHomeX = caliValue;
    else if (name == "VPToolHomeX")
        VPToolHomeX = caliValue;
    else if (name == "V1WorkOffset")
        V1WorkOffset = caliValue;
    else if (name == "V2WorkOffset")
        V2WorkOffset = caliValue;
    else if (name == "V3WorkOffset")
        V3WorkOffset = caliValue;
    else if (name == "VPWorkOffset")
        VPWorkOffset = caliValue;
    else if (name == "VDWorkX")
        VDWorkX = caliValue;
    else if (name == "VDSoftHomeX")
        VDSoftHomeX = caliValue;
    else if (name == "V2WorkX")
        V2WorkX = caliValue;
    else if (name == "PumpSoftHomeX")
        PumpSoftHomeX = caliValue;
    else if (name == "PumpToolHomeX")
        PumpToolHomeX = caliValue;
    else if (name == "PumpSoftHomeOffset")
        PumpSoftHomeOffset = caliValue;

    if (name == "CamAbs")
        CamAbs = caliValue;
    else if (name == "CamGain")
        CamGain = caliValue;
    else if (name == "CamFocus")
        CamFocus = caliValue;

}

int ExGlobal::getCaliParam(const QString &caliName)
{
    int result = 0;
    if (caliName == "V1WorkX")
        result = V1WorkX;
    else if(caliName == "V2WorkX")
        result = V2WorkX;
    else if(caliName == "V3WorkX")
        result = V3WorkX;
    else if(caliName == "VPWorkX")
        result = VPWorkX;
    else if(caliName == "V1ToolHomeX")
        result = V1ToolHomeX;
    else if(caliName == "V2ToolHomeX")
        result = V2ToolHomeX;
    else if(caliName == "V3ToolHomeX")
        result = V3ToolHomeX;
    else if(caliName == "VPToolHomeX")
        result = VPToolHomeX;
    else if(caliName == "V1SoftHomeX")
        result = V1SoftHomeX;
    else if(caliName == "V2SoftHomeX")
        result = V2SoftHomeX;
    else if(caliName == "V3SoftHomeX")
        result = V3SoftHomeX;
    else if(caliName == "VPSoftHomeX")
        result = VPSoftHomeX;
    else if(caliName == "V1SoftHomeOffset")
        result = V1SoftHomeOffset;
    else if(caliName == "V2SoftHomeOffset")
        result = V2SoftHomeOffset;
    else if(caliName == "V3SoftHomeOffset")
        result = V3SoftHomeOffset;
    else if(caliName == "VPSoftHomeOffset")
        result = VPSoftHomeOffset;
    else if(caliName == "V1WorkOffset")
        result = V1WorkOffset;
    else if(caliName == "V2WorkOffset")
        result = V2WorkOffset;
    else if(caliName == "V3WorkOffset")
        result = V3WorkOffset;
    else if(caliName == "VPWorkOffset")
        result = VPWorkOffset;
    else if(caliName == "VDWorkX")
        result = VDWorkX;
    else if(caliName == "VDSoftHomeX")
        result = VDSoftHomeX;
    else if(caliName == "PumpSoftHomeX")
        result = PumpSoftHomeX;
    else if(caliName == "PumpToolHomeX")
        result = PumpToolHomeX;
    else if(caliName == "PumpSoftHomeOffset")
        result = PumpSoftHomeOffset;

    if (caliName == "CamAbs")
        result = CamAbs;
    else if(caliName == "CamGain")
        result = CamGain;
    else if(caliName == "CamFocus")
        result = CamFocus;
    qDebug()<<"getCaliParam,"<<caliName<<",result="<<result;
    return result;
}

void ExGlobal::updateCaliParam(const QString &caliName, int caliValue)
{
    QString sql = "SELECT * FROM CaliParam WHERE ParamName = '"+caliName+"'";
    QSqlQuery query = sqlitemgrinstance->select(sql);
    if (query.next())
        sql = "UPDATE CaliParam SET ParamValue = "+ QString::number(caliValue)+" WHERE ParamName = '"+ caliName +"'";
    else
        sql = "INSERT INTO CaliParam (ParamName, ParamValue) VALUES ('"+caliName+"', "+ QString::number(caliValue)+")";
    sqlitemgrinstance->execute(sql);
    SetCaliParam(caliName,caliValue);
}

QString ExGlobal::sysLanguageName()
{
    QString lang = "中文";
    if (t_sysLanguageCode == 0)
        lang = "English";

    return lang;
}
QStringList ExGlobal::serialPort(){
    QStringList port;
    port<<"ttyUSB0"<<"ttyS0"<<"ttyS4";
    return port;
}


QString ExGlobal::getPosName(int pos){
    QString name = "Ortho";
    if (pos == 1)
        name = "IC";
    else if(pos == 2)
        name = "IFA";
    else if(pos == 3)
        name = "IFB";
    else if(pos == 4)
        name = "RSV-A";
    else if(pos == 5)
        name = "RSV-B";
    else if(pos == 6)
        name = "ADV-1";
    else if(pos == 7)
        name = "ADV-2";
    else if(pos == 8)
        name = "PIV-1";
    else if(pos == 9)
        name = "PIV-2";
    else if(pos == 10)
        name = "PIV-3";
    else if(pos == 11)
        name = "PIV-4";
    else if(pos == 12)
        name = "MP";
    else if(pos == 13)
        name = "BP";
    return name;
}

