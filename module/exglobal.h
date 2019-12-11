#ifndef EXGLOBAL_H
#define EXGLOBAL_H

#include <QObject>
#include <QQmlEngine>
#include "dao/testmodel.h"
#include "dao/testresultmodel.h"
#include "dao/usermodel.h"

class ExGlobal : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString user READ user WRITE setUser NOTIFY userChanged)
    Q_PROPERTY(QString panelName READ panelName NOTIFY panelNameChanged)
    Q_PROPERTY(QString panelCode READ panelCodeEx NOTIFY panelCodeChanged)
    Q_PROPERTY(QString sampleCode READ sampleCode NOTIFY sampleCodeChanged)
    Q_PROPERTY(QString sampleInfo READ sampleInfo NOTIFY sampleInfoChanged)
    Q_PROPERTY(QStringList serialPort READ serialPort NOTIFY serialPortChanged)
    Q_PROPERTY(bool debug READ isDebug WRITE setDebug NOTIFY debugChanged)
    Q_PROPERTY(QString sysName READ sysName WRITE setSysName NOTIFY sysNameChanged)
    Q_PROPERTY(QString sysLanguageName READ sysLanguageName NOTIFY sysLanguageNameChanged)
    Q_PROPERTY(int sysLanguageCode READ sysLanguageCode WRITE setSysLanguageCode NOTIFY sysLanguageCodeChanged)
    Q_PROPERTY(QString version READ version NOTIFY versionChanged)
    Q_PROPERTY(QString tempversion READ tempversion NOTIFY versionChanged)
    Q_PROPERTY(QString ctrlversion READ ctrlversion NOTIFY versionChanged)    
    Q_PROPERTY(int lockTime READ lockTime WRITE setLockTime NOTIFY lockTimeChanged)
    Q_PROPERTY(QString adminPassword READ adminPassword WRITE setAdminPassword NOTIFY adminPasswordChanged)
    Q_PROPERTY(bool childImage READ childImage WRITE setChildImage NOTIFY childImageChanged)
public:
    explicit ExGlobal(QObject *parent = nullptr);
    static void exInit();

    QString user() {return User;}
    void setUser(const QString &user){User = user;emit userChanged();}
    static QString panelName(){return t_panelName;}
    static void setPanelName(const QString &panelName){t_panelName = panelName;}
    static QString panelCode(){return t_panelCode;}
    static QString panelCodeEx(){return "Lot# "+t_panelCode;}
    static void setPanelCode(const QString &panelCode){t_panelCode = panelCode;}
    static QString sampleCode(){return t_sampleCode;}
    static void setSampleCode(const QString &sampleCode){t_sampleCode = sampleCode;}
    static QString sampleInfo(){return t_sampleInfo;}
    static void setSampleInfo(const QString &sampleInfo){t_sampleInfo = sampleInfo;}
    static QString reagentBox(){return t_ReagentBox;}
    static void setReagentBox(const QString &reagentBoxType){t_ReagentBox = reagentBoxType;}
    static QStringList serialPort();
    static bool isDebug(){return test;}
    static void setDebug(bool debug){test = debug;}
    QString sysName(){return SysName;}
    void setSysName(const QString &name){updateTextParam("SysName",name);emit sysNameChanged();}
    QString adminPassword(){return AdminPassword;}
    void setAdminPassword(const QString &password){updateTextParam("AdminPassword",password);emit adminPasswordChanged();}
    static QString sysLanguageName();
    int sysLanguageCode(){return LanguageCode;}
    void setSysLanguageCode(int code){updateCaliParam("LanguageCode",code);emit sysLanguageCodeChanged();}
    static QString version(){return t_version;}
    static QString tempversion(){return temp_version;}
    static QString ctrlversion(){return ctrl_version;}
    static void settempversion(const QString &version){temp_version = version;}
    static void setctrlversion(const QString &version){ctrl_version = version;}
    int lockTime(){return getCaliParam("LockScreenTime");}
    void setLockTime(int time){updateCaliParam("LockScreenTime",time);emit lockTimeChanged();}
    bool childImage(){return bChildImage;}
    void setChildImage(bool set){bChildImage = set;}

    static uchar* getReagentBox(QString BoxCode);
    static void addTest();
    Q_INVOKABLE static void updateCaliParam(const QString &caliName, int caliValue);
    Q_INVOKABLE static int getCaliParam(const QString &caliName);
    Q_INVOKABLE static void updateTextParam(const QString &caliName, QString caliValue);
    Q_INVOKABLE static QString getTextParam(const QString &caliName);
    Q_INVOKABLE static QString getIP();
    Q_INVOKABLE static bool setTime(QString time);
    Q_INVOKABLE static void exClose();
    Q_INVOKABLE static QString getPosName(int pos);
    Q_INVOKABLE static int getItemCT(int Itemid);
    Q_INVOKABLE static int getItemResult(int Testid, int Itemid);
    Q_INVOKABLE static QList<int> getBoxItemList(QString BoxCode);
    Q_INVOKABLE static QList<int> getCurrItemResult();

    Q_INVOKABLE void qmlGlobalMessage(int code){GlobalMessage(code);}

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
    static int LanguageCode;

    static int CamAbs;
    static int CamGain;
    static int CamFocus;
    static int CamWhiteBlance;

    static TestModel *pTestModel;
    static TestResultModel *pTestResultModel;
    static UserModel *pUserModel;

    static QHash<int, QByteArray> AssayItem;
    static QHash<int, int> ItemCT;

    static QString AdminPassword;
    static QString User;
    static bool bChildImage;
signals:
    void userChanged();
    void panelNameChanged();
    void panelCodeChanged();
    void sampleCodeChanged();
    void sampleInfoChanged();
    void serialPortChanged();
    void debugChanged();
    void sysNameChanged();
    void adminPasswordChanged();
    void sysLanguageNameChanged();
    void sysLanguageCodeChanged();
    void childImageChanged();
    void versionChanged();
    void lockTimeChanged();
    void exglobalMessage(int code);

private:
    static QString t_panelCode;
    static QString t_panelName;
    static QString t_sampleCode;
    static QString t_sampleInfo;    
    static QString t_ReagentBox;
    static bool test;

    static QString SysName;    
    static QString t_version;
    static QString temp_version;
    static QString ctrl_version;

    static void CaliParamInit();
    static void SetCaliParam(const QString &caliName, int caliValue);
    static void SetTextParam(const QString &textName, QString textValue);
public:
    void GlobalMessage(int code);
};

static ExGlobal *exGlobal;
static QObject *exglobal_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    //ExGlobal *exGlobal = new ExGlobal();
    return exGlobal;
}
#endif // EXGLOBAL_H
