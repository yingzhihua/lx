#ifndef TESTMODEL_H
#define TESTMODEL_H

#include<QAbstractListModel>

class Test{
public:
    Test(){}
    Test(int Testid_, QString PanelCode_, QString SerialNo_, QString BoxCode_, QString TestTime_, QString SampleInfo_, QString User_, QString Checker_, int ResultType_){
        Testid = Testid_;
        PanelCode = PanelCode_;
        SerialNo = SerialNo_;
        BoxCode = BoxCode_;
        TestTime = TestTime_;
        SampleInfo = SampleInfo_;
        User = User_;
        Checker = Checker_;
        ResultType = ResultType_;
    }
    int Testid;
    QString PanelCode;
    QString PanelName;
    QString SerialNo;
    QString BoxCode;
    QString TestTime;
    QString ValidTime;
    QString SampleInfo;
    QString SampleId;
    QString SampleRemark;
    int SampleType;
    QString User;
    QString Checker;
    int ResultType;
};

class TestModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum LineInfo_Roles{
        RolesTestid = Qt::UserRole + 1,
        RolesPanelCode,
        RolesPanelName,
        RolesSerialNo,
        RolesBoxCode,
        RoleTestTime,
        RoleValidTime,
        RoleSampleInfo,
        RoleSampleId,
        RoleSampleRemark,
        RoleSampleType,
        RoleUser,
        RoleChecker,
        RoleResultType
    };
public:
    explicit TestModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    void AddTest(int testid);
    bool ExistTest(int Testid);
    void InitTest();
    QString getCurrTestDateTime(){return m_display_list[currTestIndex].TestTime;}
    QString getCurrValidDateTime(){return m_display_list[currTestIndex].ValidTime;}
    Q_INVOKABLE QString getCurrTestCode(){return m_display_list[currTestIndex].SampleId;}
    Q_INVOKABLE QString getCurrTestSerial(){return m_display_list[currTestIndex].SerialNo;}
    Q_INVOKABLE QString getCurrTestInfo(){return m_display_list[currTestIndex].SampleInfo;}    
    QString getCurrTestChecker(){return m_display_list[currTestIndex].Checker;}
    QString getCurrTestBoxCode(){return m_display_list[currTestIndex].BoxCode;}
    QString getCurrTestRemark(){return m_display_list[currTestIndex].SampleRemark;}
    int getCurrTestType(){return m_display_list[currTestIndex].SampleType;}
    QString getCurrTestUser(){return m_display_list[currTestIndex].User;}
    Q_INVOKABLE QString getCurrTestPanelName(){return m_display_list[currTestIndex].PanelName;}
    Q_INVOKABLE QString getCurrTestPanelCode(){return m_display_list[currTestIndex].PanelCode;}
    Q_INVOKABLE void setCurrTest(int TestIndex);
    Q_INVOKABLE bool mayCheck();
    Q_INVOKABLE bool haveCheck();
    Q_INVOKABLE void checkTest();
    Q_INVOKABLE void uncheckTest();

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<Test> m_display_list;
    QHash<int, QByteArray> roles;
    int currTestid;
    int currTestIndex;
};

#endif // TESTMODEL_H
