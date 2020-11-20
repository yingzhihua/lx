#include "sqlitemgr.h"
#include "exglobal.h"
#include "sequence.h"
#include <QDebug>
#include <QCoreApplication>
#include "serialmgr.h"
#include <QNetworkInterface>
#include <QApplication>
#include <QScreen>
#include <QGuiApplication>

#include "systemcmd.h"

#define BOXCODE_DEFAULT "204"

QString ExGlobal::t_panelCode = "012315";
QString ExGlobal::t_panelName = "上呼吸道测试";
QString ExGlobal::t_sampleCode = "";//"SLX 01079";
QString ExGlobal::t_sampleInfo = "";//"华山11";
QString ExGlobal::t_sampleRemark = "";
QString ExGlobal::t_BoxSerial = "Lot# 000001";
QString ExGlobal::User = "";
QString ExGlobal::DisplayUser = "";
int ExGlobal::UserType = 0;
QString ExGlobal::BoxCode = "";
bool ExGlobal::test = false;

QString ExGlobal::SysName = "无名机";
QString ExGlobal::HospitalName = "";
int ExGlobal::PrintType = 0;

QString ExGlobal::AdminPassword = "123456";
int ExGlobal::LanguageCode = 0;
int ExGlobal::PanelBoxIndex = 1;

QString ExGlobal::t_version = "V1";
QString ExGlobal::build_version = "V1.0.19(build20201120)";
//QString ExGlobal::build_version = "V1.0.16(build20201012)";
QString ExGlobal::temp_version = "V0.00";
QString ExGlobal::ctrl_version = "V0.00";

bool ExGlobal::bLogin = false;

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
int ExGlobal::V2WorkOffset = 3276;
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

int ExGlobal::CamAbs = 0;
int ExGlobal::CamGain = 0;
int ExGlobal::CamFocus = 0;
int ExGlobal::CamWhiteBlance = 0;

int ExGlobal::PumpSoftHomeX = 0;
int ExGlobal::PumpToolHomeX = 0;
int ExGlobal::PumpSoftHomeOffset = 460;

int ExGlobal::LockScreenTime = 0;
int ExGlobal::lockscreen_time = 0;
int ExGlobal::LockScreenOpen = 0;
bool ExGlobal::bChildImage = false;
int ExGlobal::AutoFocus = 0;
int ExGlobal::QrCode = 0;
#if 0
int ExGlobal::QrX1 = 536;
int ExGlobal::QrY1 = 234;
int ExGlobal::QrX2 = 963;
int ExGlobal::QrY2 = 244;
int ExGlobal::QrX3 = 962;
int ExGlobal::QrY3 = 652;
int ExGlobal::QrX4 = 555;
int ExGlobal::QrY4 = 647;
#else
int ExGlobal::QrX1 = 424;
int ExGlobal::QrY1 = 204;
int ExGlobal::QrX2 = 952;
int ExGlobal::QrY2 = 217;
int ExGlobal::QrX3 = 975;
int ExGlobal::QrY3 = 731;
int ExGlobal::QrX4 = 456;
int ExGlobal::QrY4 = 715;
#endif

int ExGlobal::ProjectMode = 0;
int ExGlobal::MachineMode = 0;
int ExGlobal::ClearMode = 0;
int ExGlobal::LiquidsHeight = 70;
int ExGlobal::DryWet = 11;
int ExGlobal::ClearWidth = 600;
int ExGlobal::FocusX = 1000;
int ExGlobal::FocusY = 670;
int ExGlobal::FocusWidth = 600;
int ExGlobal::FocusHeight = 600;
int ExGlobal::LightCX = 1300;
int ExGlobal::LightCY = 1000;
int ExGlobal::LightR = 300;
int ExGlobal::DoorOut = 300;
int ExGlobal::DataEntry = 0;


static uchar ReagentBox[121];
TestModel * ExGlobal::pTestModel = nullptr;
TestResultModel * ExGlobal::pTestResultModel = nullptr;
UserModel * ExGlobal::pUserModel = nullptr;
WifiModel * ExGlobal::pWifiModel = nullptr;
UsbModel * ExGlobal::pUsbModel = nullptr;

QMap<int, QString> ExGlobal::AssayItem;
QHash<int, int> ExGlobal::ItemCT;
QHash<QString, QString> ExGlobal::ItemName;

unsigned char * ExGlobal::bufrgb = nullptr;
unsigned char * ExGlobal::hbufrgb = nullptr;

QTranslator ExGlobal::tor;
QApplication * ExGlobal::app;
QQmlApplicationEngine * ExGlobal::qml;

static ExGlobal *exGlobal = nullptr;

static SystemCmd cmd;

QString ExGlobal::DemoPanelCode = "20001";
QString ExGlobal::OnePointPanelCode = "31001";
int ExGlobal::SampleType = 0;

QDateTime ExGlobal::validDateTime = QDateTime::currentDateTime();
QList<int> ExGlobal::UIParam;

static QStringList SampleTypeArr;
QObject *ExGlobal::exglobal_provider(QQmlEngine *engine, QJSEngine *scriptEngine){
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    if (exGlobal == nullptr)
        exGlobal = new ExGlobal();
    return exGlobal;
}

ExGlobal *ExGlobal::getPtr()
{
    if (exGlobal == nullptr)
        exGlobal = new ExGlobal();
    return exGlobal;
}

ExGlobal::ExGlobal(QObject *parent) : QObject(parent)
{
    //qDebug()<<"ExGlobal";
    SampleTypeArr<<"鼻咽拭子"<<"咽拭子";
    UISetDefaultParam();
}

void ExGlobal::GlobalMessage(int code){
    qDebug()<<"GlobalMessage:"<<code;
    exglobalMessage(code);
}

int ExGlobal::SysCommand(QString command){
    qDebug()<<"SysCommand"<<command;
    return system(command.toLatin1().data());
}

void ExGlobal::AsynCommand(QString command){
    cmd.execute(command);
}

void ExGlobal::exClose(){
    //cpu name: wmic cpu get Name
    qDebug()<<"exClose";
    //QProcess p;
    //p.start("halt");
    Log::Logdb(LOGTYPE_POWERDOWN);
    system("shutdown -t 0");
}

QString ExGlobal::getIP(){
    foreach(QNetworkInterface i,QNetworkInterface::allInterfaces()){
        if (i.name().contains("eth"))
        foreach(QNetworkAddressEntry e,i.addressEntries()){
            if (e.ip() != QHostAddress::LocalHost && e.ip().toIPv4Address())
                return e.ip().toString();
        }
    }
    /*
    foreach(const QHostAddress& hostAddress,QNetworkInterface::allAddresses())
        if(hostAddress != QHostAddress::LocalHost && hostAddress.toIPv4Address())
            return hostAddress.toString();
            */
    return "";
}

QString ExGlobal::getWIFIIP(){
    foreach(QNetworkInterface i,QNetworkInterface::allInterfaces()){
        if (i.name().contains("wlan"))
        foreach(QNetworkAddressEntry e,i.addressEntries()){
            if (e.ip() != QHostAddress::LocalHost && e.ip().toIPv4Address())
                return e.ip().toString();
        }
    }
    return "";
}

QString ExGlobal::getMac(){
    foreach(QNetworkInterface i,QNetworkInterface::allInterfaces()){
        if (i.name().contains("eth"))
            return i.hardwareAddress();
    }
    return "";
}

QString ExGlobal::getSerial(){
    return "002001002";
}

QStringList ExGlobal::getNetWork(){
    QStringList netArr;
    netArr<<""<<""<<""<<""<<""<<""<<""<<""<<""<<""<<""<<"";
    QFile file("/etc/network/interfaces.d/eth0");
    if (file.open(QIODevice::ReadOnly)){
        if (file.size()>0){
            QTextStream in(&file);
            QString line = in.readLine();
            while(!line.isEmpty())
            {
                qDebug()<<line;
                int addressIndex;

                if ((addressIndex = line.indexOf("address")) >= 0)
                {
                    QStringList strArr = line.mid(addressIndex+7).trimmed().split('.');
                    netArr[0] = strArr[0];
                    netArr[1] = strArr[1];
                    netArr[2] = strArr[2];
                    netArr[3] = strArr[3];
                }
                else if((addressIndex = line.indexOf("gateway")) >= 0)
                {
                    QStringList strArr = line.mid(addressIndex+7).trimmed().split('.');
                    netArr[4] = strArr[0];
                    netArr[5] = strArr[1];
                    netArr[6] = strArr[2];
                    netArr[7] = strArr[3];
                }
                else if((addressIndex = line.indexOf("netmask")) >= 0)
                {
                    QStringList strArr = line.mid(addressIndex+7).trimmed().split('.');
                    netArr[8] = strArr[0];
                    netArr[9] = strArr[1];
                    netArr[10] = strArr[2];
                    netArr[11] = strArr[3];
                }

                line = in.readLine();
            }
        }
        file.close();
    }
    foreach(const QHostAddress& hostAddress,QNetworkInterface::allAddresses())
        if(hostAddress != QHostAddress::LocalHost && hostAddress.toIPv4Address())
        {
            QStringList strArr = hostAddress.toString().split('.');
            netArr[0] = strArr[0];
            netArr[1] = strArr[1];
            netArr[2] = strArr[2];
            netArr[3] = strArr[3];
            break;
        }
    return netArr;
}

void ExGlobal::setNetWork(const QString &ip, const QString &mask, const QString &gate){
    QFile file("/etc/network/interfaces.d/eth0");
    if (file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QTextStream text(&file);
        text<<"auto eth0"<<endl<<"iface eth0 inet static"<<endl;
        text<<"address "<<ip<<endl;
        text<<"gateway "<<gate<<endl;
        text<<"netmask "<<mask<<endl;
        text<<"dns-nameservers "<<gate<<endl;
        text.flush();
        file.close();

        if (getIP() != ip)
            system("/etc/init.d/networking restart");
    }
    qDebug()<<"setNetWork"<<ip<<mask<<gate;
}
bool ExGlobal::setTime(QString time){
    QString order = "date -s '"+time+"'";
    int result = system(order.toLatin1().data());
    qDebug()<<"setTime,result="<<result;
    system("hwclock --systohc");
    return true;
}

int ExGlobal::diskCheck(){
    int result = 0;
    if (system("df / > /opt/diskspacet") == 0){
        QFile file("/opt/diskspacet");
        if (!file.open(QIODevice::ReadOnly)){
            qDebug()<<"getDiskSpace open file error!";
            return 0;
        }
        if (file.size()>0){
            QTextStream in(&file);
            QString line = in.readLine();
            if (!line.isEmpty()){
                int availableIndex = line.indexOf("Available");
                int usepaIndex = line.indexOf("Use%");
                line = in.readLine();
                if (!line.isEmpty())
                {
                    QString available = line.mid(availableIndex,(usepaIndex-availableIndex)).simplified();
                    qDebug()<<available;
                    result = available.toInt();
                }
            }
        }
        file.close();
    }
    if (result < 6000000)
        return 1;
    else if (result < 1000000){
        return 2;
    }
    return 0;
}

void ExGlobal::exInit()
{
    CaliParamInit();
    SerialMgr::SerialInit();    

    //Sequence::sequenceInit();
}

void ExGlobal::CaliParamInit()
{
    QString sql = "SELECT * FROM CaliParam";
    QSqlQuery query = SqliteMgr::select(sql);

    while(query.next())
    {
        SetCaliParam(query.value(0).toString(),query.value(1).toInt());
    }

    sql = "select * from TextParam";
    query = SqliteMgr::select(sql);
    while(query.next()){
        SetTextParam(query.value(0).toString(),query.value(1).toString());
    }

    sql = "select * from AssayItem order by Itemid";
    query = SqliteMgr::select(sql);
    while(query.next()){
        //qDebug()<<"Itemid:"<<query.value(0).toInt()<<"ItemName:"<<query.value(1).toString()<<"ItemCT:"<<query.value(2).toInt();
        AssayItem[query.value(0).toInt()] = query.value(1).toString();
        ItemCT[query.value(0).toInt()] = query.value(2).toInt();
        ItemName[query.value(1).toString()] = query.value(3).toString();
    }

    pUserModel->LoadUser();
    pWifiModel->LoadData();

    updateLockTime();

    Log::Logdb(LOGTYPE_POWERON);
    qDebug()<<"CaliParamInit end";
}

uchar* ExGlobal::getReagentBox(QString boxCode){
    if (boxCode == "")
        boxCode = BOXCODE_DEFAULT;
    if (boxCode != BoxCode)
    {
        BoxCode = boxCode;
        LoadReagentBox();
    }
    return ReagentBox;
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
    else if (name == "CamWhiteBlance")
        CamWhiteBlance = caliValue;
    else if (name == "LockScreenTime")
        LockScreenTime = caliValue;
    else if (name == "LockScreenOpen")
        LockScreenOpen = caliValue;
    else if (name == "LanguageCode")
        LanguageCode = caliValue;
    else if (name == "PanelBoxIndex")
        PanelBoxIndex = caliValue;
    else if (name == "AutoFocus")
        AutoFocus = caliValue;
    else if (name == "QrCode")
        QrCode = caliValue;
    else if(name == "QrX1")
        QrX1 = caliValue;
    else if(name == "QrY1")
        QrY1 = caliValue;
    else if(name == "QrX2")
        QrX2 = caliValue;
    else if(name == "QrY2")
        QrY2 = caliValue;
    else if(name == "QrX3")
        QrX3 = caliValue;
    else if(name == "QrY3")
        QrY3 = caliValue;
    else if(name == "QrX4")
        QrX4 = caliValue;
    else if(name == "QrY4")
        QrY4 = caliValue;
    else if(name == "ProjectMode")
        ProjectMode = caliValue;
    else if(name == "MachineMode")
        MachineMode = caliValue;
    else if(name == "ClearMode")
        ClearMode = caliValue;
    else if(name == "ClearWidth")
        ClearWidth = caliValue;
    else if(name == "FocusX")
        FocusX = caliValue;
    else if(name == "FocusY")
        FocusY = caliValue;
    else if(name == "FocusWidth")
        FocusWidth = caliValue;
    else if(name == "FocusHeight")
        FocusHeight = caliValue;
    else if(name == "LightCX")
        LightCX = caliValue;
    else if(name == "LightCY")
        LightCY = caliValue;
    else if(name == "LightR")
        LightR = caliValue;
    else if(name == "DryWet")
        DryWet = caliValue;
    else if(name == "LiquidsHeight")
        LiquidsHeight = caliValue;
    else if(name == "PrintType")
        PrintType = caliValue;
    //qDebug()<<"setCaliParam,"<<name<<",result="<<caliValue;
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
    else if(caliName == "CamWhiteBlance")
        result = CamWhiteBlance;
    else if(caliName == "LockScreenTime")
        result = LockScreenTime;
    else if(caliName == "LockScreenOpen")
        result = LockScreenOpen;
    else if(caliName == "LanguageCode")
        result = LanguageCode;
    else if(caliName == "PanelBoxIndex")
        result = PanelBoxIndex;
    else if(caliName == "AutoFocus")
        result = AutoFocus;
    else if(caliName == "QrCode")
        result = QrCode;
    else if(caliName == "QrX1")
        result = QrX1;
    else if(caliName == "QrY1")
        result = QrY1;
    else if(caliName == "QrX2")
        result = QrX2;
    else if(caliName == "QrY2")
        result = QrY2;
    else if(caliName == "QrX3")
        result = QrX3;
    else if(caliName == "QrY3")
        result = QrY3;
    else if(caliName == "QrX4")
        result = QrX4;
    else if(caliName == "QrY4")
        result = QrY4;
    else if(caliName == "ProjectMode")
        result = ProjectMode;
    else if(caliName == "MachineMode")
        result = MachineMode;
    else if(caliName == "ClearMode")
        result = ClearMode;
    else if(caliName == "ClearWidth")
        result = ClearWidth;
    else if(caliName == "FocusX")
        result = FocusX;
    else if(caliName == "FocusY")
        result = FocusY;
    else if(caliName == "FocusWidth")
        result = FocusWidth;
    else if(caliName == "FocusHeight")
        result = FocusHeight;
    else if(caliName == "LightCX")
        result = LightCX;
    else if(caliName == "LightCY")
        result = LightCY;
    else if(caliName == "LightR")
        result = LightR;
    else if(caliName == "DryWet")
        result = DryWet;
    else if(caliName == "LiquidsHeight")
        result = LiquidsHeight;
    else if(caliName == "PrintType")
        result = PrintType;
    qDebug()<<"getCaliParam,"<<caliName<<",result="<<result;
    return result;
}

void ExGlobal::updateCaliParam(const QString &caliName, int caliValue)
{
    QString sql = "SELECT * FROM CaliParam WHERE ParamName = '"+caliName+"'";
    QSqlQuery query = SqliteMgr::select(sql);
    if (query.next())
        sql = "UPDATE CaliParam SET ParamValue = "+ QString::number(caliValue)+" WHERE ParamName = '"+ caliName +"'";
    else
        sql = "INSERT INTO CaliParam (ParamName, ParamValue) VALUES ('"+caliName+"', "+ QString::number(caliValue)+")";
    SqliteMgr::execute(sql);
    qDebug()<<"updateCaliParam,"<<caliName<<",result="<<caliValue;
    SetCaliParam(caliName,caliValue);
}

void ExGlobal::SetTextParam(const QString &textName, QString textValue){
    if (textName == "SysName")
        SysName = textValue;
    else if(textName == "AdminPassword")
        AdminPassword = textValue;
    else if(textName == "HospitalName")
        HospitalName = textValue;
}

QString ExGlobal::getTextParam(const QString &caliName){
    QString result;
    if (caliName == "SysName")
        result = SysName;
    else if(caliName == "AdminPassword")
        result = AdminPassword;
    else if(caliName == "HospitalName")
        result = HospitalName;
    return result;
}

void ExGlobal::updateTextParam(const QString &textName, QString textValue){
    QString sql = QString("replace into TextParam(ParamName,ParamValue) values('%1','%2')").arg(textName).arg(textValue);
    SqliteMgr::execute(sql);
    SetTextParam(textName,textValue);
}

QString ExGlobal::sysLanguageName()
{
    QString lang = "中文";
    if (LanguageCode == 1)
        lang = "English";

    return lang;
}
QStringList ExGlobal::serialPort(){
    QStringList port;
    port<<"ttyUSB0"<<"ttyS0"<<"ttyS4";
    return port;
}

QString ExGlobal::getPosName(int pos){
    return AssayItem[pos];
}

int ExGlobal::getItemCT(int Itemid){
    qDebug()<<"getItemCT,Itemid="<<Itemid<<",CT="<<ItemCT[Itemid];
    return 500;
    //return ItemCT[Itemid];
}

QList<int> ExGlobal::getBoxItemList(){
    QList<int> result;
    QString sql = "SELECT * FROM ReagentPos where BoxCode='"+pTestModel->getCurrTestBoxCode()+"'";
    QSqlQuery query = SqliteMgr::select(sql);
    while(query.next()){
        bool find = false;
        int value = query.value(2).toInt();
        if (value > 2){
            for (int j = 0; j < result.size(); j++)
                if (result[j] == value)
                {
                    find = true;
                    break;
                }
            if (find == false)
                result<<value;
        }
    }

    sort(result.begin(),result.end());
    return result;
}

QStringList ExGlobal::getPosNameArray(){
    QStringList nameArr;
    getReagentBox(reagentBox());
    for (int i = 0; i < 121; i++)
        if (ReagentBox[i] > 1){
            QString name = AssayItem[ReagentBox[i]];
            bool findit = false;
            for (int i = 0; i < nameArr.size(); i++){
                if(nameArr[i] == name){
                    findit = true;
                    break;
                }
            }
            if (findit == false)
                nameArr<<name;
        }

    return nameArr;
}

void ExGlobal::panelBoxIndexAddOne(){
    updateCaliParam("PanelBoxIndex",PanelBoxIndex + 1);
    t_BoxSerial = QString("Lot# %1").arg(PanelBoxIndex, 6, 10, QChar('0'));
    //t_sampleCode = QString("SLX %1").arg(PanelBoxIndex, 6, 10, QChar('0'));
}

QString ExGlobal::reagentBox(){
    if (BoxCode == "")
    {
        BoxCode = BOXCODE_DEFAULT;
        LoadReagentBox();
    }
    return BoxCode;
}

void ExGlobal::setReagentBox(const QString &reagentBoxType){
    if (reagentBoxType != BoxCode){
        BoxCode = reagentBoxType;
        LoadReagentBox();
    }
}

void ExGlobal::LoadReagentBox(){
    QString sql = "SELECT * FROM ReagentPos where BoxCode='"+BoxCode+"'";
    QSqlQuery query = SqliteMgr::select(sql);
    memset(ReagentBox,0,sizeof(ReagentBox));
    while(query.next()){
        if (query.value(1).toInt() < 121)
            ReagentBox[query.value(1).toInt()] = static_cast<uchar>(query.value(2).toInt());
    }
}

void ExGlobal::setSysLanguageCode(int code){
    if (code != LanguageCode)
    {
        //QApplication::installTranslator(&tor);
        //QApplication::removeTranslator(&tor);
        qDebug()<<"setLanguage"<<code;
        if (code == 0)
        {
            app->removeTranslator(&tor);
            setUIParam(0);
        }
        else
            Translator(code);

        qml->retranslate();
        updateCaliParam("LanguageCode",code);
        emit sysLanguageCodeChanged();
        emit uiParamChanged();
    }
}

void ExGlobal::Translator(int language){
    if (language == 1)
    {
        tor.load(QCoreApplication::applicationDirPath()+"/en_US.qm");
        app->installTranslator(&tor);
    }
    setUIParam(language);
}

void ExGlobal::updateLockTime(){
    if (LockScreenOpen == 0)
        lockscreen_time = 0;
    else if(LockScreenTime == 0)
        lockscreen_time = 60;
    else if(LockScreenTime == 1)
        lockscreen_time = 180;
    else if(LockScreenTime == 2)
        lockscreen_time = 300;
    else if(LockScreenTime == 3)
        lockscreen_time = 600;
    else if(LockScreenTime == 4)
        lockscreen_time = 900;
    else if(LockScreenTime == 5)
        lockscreen_time = 1800;
    else
        lockscreen_time = 0;
}

QString ExGlobal::screenCapture(){
    QString filename = QCoreApplication::applicationDirPath()+"/screenshot";
    QDir dir(filename);
    if (!dir.exists())
        dir.mkpath(filename);
    filename += "/"+QDateTime::currentDateTime().toString("yyyyMMdd_hh-mm-ss-zzz")+".jpg";
    QScreen *screen = QGuiApplication::primaryScreen();
    screen->grabWindow(0).save(filename,"jpg");
    return filename;
}

void ExGlobal::setSampleCode(const QString &sampleCode){
    if (sampleCode == "p" || sampleCode == "P")
    {
        screenCapture();
    }
    else if(sampleCode.length()>1 && sampleCode[0] == 'I'){
        setSampleInfo(sampleCode.mid(1));
    }
    else if(sampleCode.length()>1 && sampleCode[0] == 'R'){
        setSampleRemark(sampleCode.mid(1));
    }
    else{
        t_sampleCode = sampleCode;
        emit sampleCodeChanged();
    }

}

QStringList ExGlobal::getSampleTypeArr(){
    return SampleTypeArr;
}

QString ExGlobal::getSampleTypeName(int type){
    return SampleTypeArr[type];
}

QString ExGlobal::getCurrSampleName(){
    return SampleTypeArr[SampleType];
}

static QDomDocument UIXml;
void ExGlobal::UISetDefaultParam(){
    qDebug()<<"UISetDefaultParam";
    UIParam<<60;        //UI_MAIN_MENU_FONT
    UIParam<<40;        //UI_MAIN_MENU_HORIZONTALOFFSET
    UIParam<<40;        //UI_LOGIN_SUBMIT_FONT
    UIParam<<40;        //UI_IDLE_EXITLOGIN_FONT
    UIParam<<40;        //UI_IDLE_SHUTDOWN_FONT
    UIParam<<45;        //UI_BOXREADY_BUTTOM_PROMPT_FONT
    UIParam<<40;        //UI_BOXREADY_TEST_CANCEL_FONT
    UIParam<<40;        //UI_LOGIN_SUBMIT_FONT
    UIParam<<40;        //UI_LOGIN_SUBMIT_FONT
    UIParam<<40;        //UI_LOGIN_SUBMIT_FONT
    UIParam<<40;        //UI_LOGIN_SUBMIT_FONT
    UIParam<<40;        //UI_LOGIN_SUBMIT_FONT
    UIParam<<40;        //UI_LOGIN_SUBMIT_FONT
    UIParam<<40;        //UI_LOGIN_SUBMIT_FONT
    UIParam<<40;        //UI_LOGIN_SUBMIT_FONT


    QFile xmlfile(QCoreApplication::applicationDirPath()+"/UIParam.xml");
    if (!xmlfile.open(QFile::ReadOnly))
        return;
    UIXml.setContent(&xmlfile);
    xmlfile.close();
}

void ExGlobal::setUIParam(int language){
    if (UIXml.isNull())
    {
        qDebug()<<"UIXml is null";
        return;
    }
    QDomElement root = UIXml.documentElement();
    if (root.isNull())
        return;
    QDomElement param;
    if (language == 0)
        param = root.firstChildElement("cn");
    else if(language == 1)
        param = root.firstChildElement("en");
    if (param.isNull())
    {
        qDebug()<<"param is null";
        return;
    }
    QDomNode node = param.firstChild();
    while(!node.isNull()){
        QDomElement e = node.toElement();
        if (!e.isNull())
        {
            qDebug()<<e.tagName()<<e.text().toInt();
            if (e.tagName() == "UI_MAIN_MENU_FONT")
                UIParam[UI_MAIN_MENU_FONT] = e.text().toInt();
            else if(e.tagName() == "UI_MAIN_MENU_HORIZONTALOFFSET")
                UIParam[UI_MAIN_MENU_HORIZONTALOFFSET] = e.text().toInt();
            else if(e.tagName() == "UI_LOGIN_SUBMIT_FONT")
                UIParam[UI_LOGIN_SUBMIT_FONT] = e.text().toInt();
            else if(e.tagName() == "UI_IDLE_EXITLOGIN_FONT")
                UIParam[UI_IDLE_EXITLOGIN_FONT] = e.text().toInt();
            else if(e.tagName() == "UI_IDLE_SHUTDOWN_FONT")
                UIParam[UI_IDLE_SHUTDOWN_FONT] = e.text().toInt();
            else if(e.tagName() == "UI_BOXREADY_BUTTOM_PROMPT_FONT")
                UIParam[UI_BOXREADY_BUTTOM_PROMPT_FONT] = e.text().toInt();
            else if(e.tagName() == "UI_BOXREADY_TEST_CANCEL_FONT")
                UIParam[UI_BOXREADY_TEST_CANCEL_FONT] = e.text().toInt();
        }
        node = node.nextSibling();
    }
}
