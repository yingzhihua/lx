#ifndef TESTMODEL_H
#define TESTMODEL_H

#include<QAbstractListModel>

class Test{
public:
    Test(){}
    Test(int Testid_, QString PanelCode_, QString SerialNo_, QString TestTime_, QString SampleInfo_, QString User_, int ResultType_){
        Testid = Testid_;
        PanelCode = PanelCode_;
        SerialNo = SerialNo_;
        TestTime = TestTime_;
        SampleInfo = SampleInfo_;
        User = User_;
        ResultType = ResultType_;
    }
    int Testid;
    QString PanelCode;
    QString SerialNo;
    QString TestTime;
    QString SampleInfo;
    QString User;
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
        RoleUser,
        RoleResultType
    };
public:
    explicit TestModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    void AddTest(const Test &test);
    Q_INVOKABLE void setCurrItem(int ItemIndex);

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<Test> m_display_list;
    QHash<int, QByteArray> roles;
    int currTestid;
};

#endif // TESTMODEL_H
