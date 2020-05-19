#ifndef TESTMODEL_H
#define TESTMODEL_H

#include<QAbstractListModel>

class Test{
public:
    Test(){}
    Test(int Testid_, QString PanelCode_, QString SerialNo_, QString TestTime_, QString SampleInfo_, QString User_, QString Checker_, int ResultType_){
        Testid = Testid_;
        PanelCode = PanelCode_;
        SerialNo = SerialNo_;
        TestTime = TestTime_;
        SampleInfo = SampleInfo_;
        User = User_;
        Checker = Checker_;
        ResultType = ResultType_;
    }
    int Testid;
    QString PanelCode;
    QString SerialNo;
    QString TestTime;
    QString SampleInfo;
    QString SampleId;
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
        RolesSerialNo,
        RoleTestTime,
        RoleSampleInfo,
        RoleSampleId,
        RoleUser,
        RoleChecker,
        RoleResultType
    };
public:
    explicit TestModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    void AddTest(const Test &test);
    bool ExistTest(int Testid);
    QString getCurrTestDateTime(){return m_display_list[currTestIndex].TestTime;}
    QString getCurrTestCode(){return m_display_list[currTestIndex].SampleId;}
    QString getCurrTestSerial(){return m_display_list[currTestIndex].SerialNo;}
    QString getCurrTestInfo(){return m_display_list[currTestIndex].SampleInfo;}
    QString getCurrTestUser(){return m_display_list[currTestIndex].User;}
    QString getCurrTestChecker(){return m_display_list[currTestIndex].Checker;}
    Q_INVOKABLE void setCurrTest(int TestIndex);
    Q_INVOKABLE bool mayCheck();
    Q_INVOKABLE void checkTest();

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<Test> m_display_list;
    QHash<int, QByteArray> roles;
    int currTestid;
    int currTestIndex;
};

#endif // TESTMODEL_H
