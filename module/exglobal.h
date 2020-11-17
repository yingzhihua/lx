#ifndef EXGLOBAL_H
#define EXGLOBAL_H

#include <QObject>
#include <QQmlEngine>
#include <QQmlApplicationEngine>
#include <QTranslator>
#include <QApplication>

#include "dao/testmodel.h"
#include "dao/testresultmodel.h"
#include "dao/usermodel.h"
#include "dao/wifimodel.h"
#include "dao/usbmodel.h"

class ExGlobal : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString user READ user WRITE setUser NOTIFY userChanged)
    Q_PROPERTY(QString displayUser READ displayUser WRITE setDisplayUser NOTIFY userChanged)
    Q_PROPERTY(QString panelName READ panelName NOTIFY panelNameChanged)
    Q_PROPERTY(QString panelCode READ panelCode NOTIFY panelCodeChanged)
    Q_PROPERTY(QString sampleCode READ sampleCode WRITE setSampleCode NOTIFY sampleCodeChanged)
    Q_PROPERTY(QString sampleInfo READ sampleInfo WRITE setSampleInfo NOTIFY sampleInfoChanged)
    Q_PROPERTY(QString sampleRemark READ sampleRemark WRITE setSampleRemark NOTIFY sampleRemarkChanged)
    Q_PROPERTY(QStringList serialPort READ serialPort NOTIFY serialPortChanged)
    Q_PROPERTY(bool debug READ isDebug WRITE setDebug NOTIFY debugChanged)
    Q_PROPERTY(QString sysName READ sysName WRITE setSysName NOTIFY sysNameChanged)
    Q_PROPERTY(QString hospitalName READ hospitalName WRITE setHospitalName NOTIFY hospitalNameChanged)
    Q_PROPERTY(int printType READ printType WRITE setPrintType NOTIFY printTypeChanged)
    Q_PROPERTY(QString sysLanguageName READ sysLanguageName NOTIFY sysLanguageNameChanged)
    Q_PROPERTY(int sysLanguageCode READ sysLanguageCode WRITE setSysLanguageCode NOTIFY sysLanguageCodeChanged)
    Q_PROPERTY(QString version READ version NOTIFY versionChanged)
    Q_PROPERTY(QString buildversion READ buildversion NOTIFY versionChanged)
    Q_PROPERTY(QString tempversion READ tempversion NOTIFY versionChanged)
    Q_PROPERTY(QString ctrlversion READ ctrlversion NOTIFY versionChanged)    
    Q_PROPERTY(int lockTime READ lockTime WRITE setLockTime NOTIFY lockTimeChanged)
    Q_PROPERTY(bool lockscreenOpen READ lockscreenOpen WRITE setLockscreenOpen NOTIFY lockscreenOpenChanged)
    Q_PROPERTY(QString adminPassword READ adminPassword WRITE setAdminPassword NOTIFY adminPasswordChanged)
    Q_PROPERTY(bool childImage READ childImage WRITE setChildImage NOTIFY childImageChanged)
    Q_PROPERTY(int autoFocus READ autoFocus WRITE setAutoFocus NOTIFY autoFocusChanged)
    Q_PROPERTY(int qrCode READ qrCode WRITE setQrCode NOTIFY qrCodeChanged)
    Q_PROPERTY(QList<int> uiParam READ getUIParam NOTIFY uiParamChanged)

public:
    enum UI_PARAM:int16_t{
        UI_LOGIN_SUBMIT_FONT,
        UI_PARAM_LENGTH
    };
    Q_ENUM(UI_PARAM)

    explicit ExGlobal(QObject *parent = nullptr);
    static QObject *exglobal_provider(QQmlEngine *engine, QJSEngine *scriptEngine);
    static ExGlobal *getPtr();
    static void exInit();

    QString user() {return User;}
    QString displayUser() {return DisplayUser;}
    void setUser(const QString &user){User = user;emit userChanged();}
    void setDisplayUser(const QString &user){DisplayUser = user;emit userChanged();}
    static QString panelName(){return t_panelName;}
    static int SysCommand(QString command);
    static void AsynCommand(QString command);
    static void setPanelName(const QString &panelName){t_panelName = panelName;}
    static QString panelCode(){return t_panelCode;}
    static QString panelCodeEx(){return "Lot# "+t_panelCode;}
    static void setPanelCode(const QString &panelCode){t_panelCode = panelCode;}
    static QString sampleCode(){return t_sampleCode;}
    void setSampleCode(const QString &sampleCode);
    static QString sampleInfo(){return t_sampleInfo;}
    void setSampleInfo(const QString &sampleInfo){t_sampleInfo = sampleInfo;emit sampleInfoChanged();}
    static QString sampleRemark(){return t_sampleRemark;}
    void setSampleRemark(const QString &sampleRemark){t_sampleRemark = sampleRemark;emit sampleRemarkChanged();}
    static QString reagentBox();
    static void setReagentBox(const QString &reagentBoxType);
    static QStringList serialPort();
    static bool isDebug(){return test;}
    static void setDebug(bool debug){test = debug;}
    QString sysName(){return SysName;}
    void setSysName(const QString &name){updateTextParam("SysName",name);emit sysNameChanged();}
    QString hospitalName(){return HospitalName;}
    void setHospitalName(const QString &name){updateTextParam("HospitalName",name);emit hospitalNameChanged();}
    int printType(){return PrintType;}
    void setPrintType(int printType){updateCaliParam("PrintType",printType);emit printTypeChanged();}
    QString adminPassword(){return AdminPassword;}
    void setAdminPassword(const QString &password){updateTextParam("AdminPassword",password);emit adminPasswordChanged();}
    static QString sysLanguageName();
    int sysLanguageCode(){return LanguageCode;}
    void setSysLanguageCode(int code);
    static QString version(){return t_version;}
    static QString buildversion(){return build_version;}
    static QString tempversion(){return temp_version;}
    static QString ctrlversion(){return ctrl_version;}
    static void settempversion(const QString &version){temp_version = version;}
    static void setctrlversion(const QString &version){ctrl_version = version;}
    int lockTime(){return getCaliParam("LockScreenTime");}
    void setLockTime(int time){updateCaliParam("LockScreenTime",time);updateLockTime();emit lockTimeChanged();}
    bool lockscreenOpen(){return LockScreenOpen == 1;}
    void setLockscreenOpen(bool bopen){updateCaliParam("LockScreenOpen",bopen?1:0);updateLockTime();emit lockscreenOpenChanged();}
    bool childImage(){return bChildImage;}
    void setChildImage(bool set){bChildImage = set;}
    bool autoFocus(){return AutoFocus;}
    void setAutoFocus(int set){updateCaliParam("AutoFocus",set);emit autoFocusChanged();}
    int qrCode(){return QrCode;}
    void setQrCode(int set){updateCaliParam("QrCode",set);emit qrCodeChanged();}

    static uchar* getReagentBox(QString boxCode);
    Q_INVOKABLE static QString getPosName(int pos);
    Q_INVOKABLE static void updateCaliParam(const QString &caliName, int caliValue);
    Q_INVOKABLE static int getCaliParam(const QString &caliName);
    Q_INVOKABLE static void updateTextParam(const QString &caliName, QString caliValue);
    Q_INVOKABLE static QString getTextParam(const QString &caliName);
    Q_INVOKABLE static QString getIP();
    Q_INVOKABLE static QString getWIFIIP();
    Q_INVOKABLE static QString getMac();
    Q_INVOKABLE static QString getSerial();
    Q_INVOKABLE static QStringList getNetWork();
    Q_INVOKABLE static void setNetWork(const QString &ip,const QString &mask,const QString &gate);
    Q_INVOKABLE static bool setTime(QString time);
    Q_INVOKABLE static void exClose();
    Q_INVOKABLE static QStringList getPosNameArray();
    Q_INVOKABLE static int getItemCT(int Itemid);    
    Q_INVOKABLE static QList<int> getBoxItemList();
    Q_INVOKABLE static QStringList getSampleTypeArr();
    static QString getSampleTypeName(int type);
    Q_INVOKABLE static void setSampleType(int type){SampleType = type;}
    Q_INVOKABLE static int getSampleType(){return SampleType;}
    Q_INVOKABLE static QString getCurrSampleName();

    Q_INVOKABLE void qmlGlobalMessage(int code){GlobalMessage(code);}

    Q_INVOKABLE static int projectMode(){return ProjectMode;} //0:临时版本；1：正式版本; 2:注册版本
    Q_INVOKABLE void panelBoxIndexAddOne();
    Q_INVOKABLE int dataEntry(){return DataEntry;}
    Q_INVOKABLE void setDataEntry(int entry){DataEntry = entry;}
    Q_INVOKABLE static QString boxSerial(){return t_BoxSerial;}
    static void setBoxSerial(QString serial){t_BoxSerial = serial;}
    Q_INVOKABLE static int diskCheck();

    static void setLogin(){bLogin = true;}
    Q_INVOKABLE static bool login(){return bLogin;}

    static int V1WorkX;
    static int V2WorkX;
    static int V3WorkX;
    static int VPWorkX;    
    static int V1WorkOffset;
    static int V2WorkOffset;
    static int V3WorkOffset;
    static int VPWorkOffset;    
    static int V1SoftHomeX;
    static int V2SoftHomeX;
    static int V3SoftHomeX;
    static int VPSoftHomeX;
    static int PumpSoftHomeX;
    static int V1SoftHomeOffset;
    static int V2SoftHomeOffset;
    static int V3SoftHomeOffset;
    static int VPSoftHomeOffset;
    static int PumpSoftHomeOffset;
    static int V1ToolHomeX;
    static int V2ToolHomeX;
    static int V3ToolHomeX;
    static int VPToolHomeX;
    static int PumpToolHomeX;
    static int VDWorkX;
    static int VDSoftHomeX;

    static int LockScreenTime;
    static int lockscreen_time;
    static int LockScreenOpen;
    static int LanguageCode;
    static int PanelBoxIndex;

    static int CamAbs;
    static int CamGain;
    static int CamFocus;
    static int CamWhiteBlance;

    static int QrX1;
    static int QrY1;
    static int QrX2;
    static int QrY2;
    static int QrX3;
    static int QrY3;
    static int QrX4;
    static int QrY4;

    static TestModel *pTestModel;
    static TestResultModel *pTestResultModel;
    static UserModel *pUserModel;
    static WifiModel *pWifiModel;
    static UsbModel *pUsbModel;

    static QMap<int, QString> AssayItem;
    static QHash<QString, QString> ItemName;
    static QHash<int, int> ItemCT;

    static QString AdminPassword;
    static QString User;
    static QString DisplayUser;
    static int UserType;
    static bool bChildImage;
    static int AutoFocus;
    static int QrCode;
    static int ProjectMode;
    static int MachineMode;
    static int ClearMode;
    static int LiquidsHeight;
    static int DryWet;
    static int ClearWidth;
    static int FocusX;
    static int FocusY;
    static int FocusWidth;
    static int FocusHeight;
    static int LightCX;
    static int LightCY;
    static int LightR;
    static int DoorOut;
    static int DataEntry;
    static unsigned char *bufrgb;
    static unsigned char *hbufrgb;

    static QString SysName;
    static QString HospitalName;
    static int PrintType;
    static QTranslator tor;
    static QApplication *app;
    static QQmlApplicationEngine *qml;
    static void Translator(int language);
    static void setUIParam(int language);
    static QString screenCapture();

    static QString DemoPanelCode;
    static QString OnePointPanelCode;
    static int SampleType;
    static QDateTime validDateTime;
    static QList<int> UIParam;
    QList<int> getUIParam(){return UIParam;}
signals:
    void userChanged();
    void panelNameChanged();
    void panelCodeChanged();
    void sampleCodeChanged();
    void sampleInfoChanged();
    void sampleRemarkChanged();
    void serialPortChanged();
    void debugChanged();
    void projectModeChanged();
    void sysNameChanged();
    void hospitalNameChanged();
    void printTypeChanged();
    void adminPasswordChanged();
    void sysLanguageNameChanged();
    void sysLanguageCodeChanged();
    void childImageChanged();
    void autoFocusChanged();
    void qrCodeChanged();
    void versionChanged();
    void lockTimeChanged();
    void lockscreenOpenChanged();
    void exglobalMessage(int code);
    void uiParamChanged();

private:
    static QString t_panelCode;
    static QString t_panelName;
    static QString t_sampleCode;
    static QString t_sampleInfo;    
    static QString t_sampleRemark;
    static QString BoxCode;
    static QString t_BoxSerial;

    static bool test;

    static QString t_version;
    static QString build_version;
    static QString temp_version;
    static QString ctrl_version;
    static bool bLogin;

    static void CaliParamInit();
    static void SetCaliParam(const QString &caliName, int caliValue);
    static void SetTextParam(const QString &textName, QString textValue);
    static void LoadReagentBox();
    static void updateLockTime();
    static void UISetDefaultParam();

public:
    void GlobalMessage(int code);
};

#endif // EXGLOBAL_H
