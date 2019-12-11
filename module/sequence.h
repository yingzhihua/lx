#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <QObject>
#include <QQmlEngine>
#include <QtCore>
#include <QtXml>
#include "serialmgr.h"
#include "imagecapture.h"
#include "imageprovider.h"
#include "imageanalysis.h"
#include "entity.h"
#include "qrcoder.h"
#include "dao/testmgr.h"

class Sequence : public QObject
{
    Q_OBJECT
public:
    explicit Sequence(QObject *parent = nullptr);
    enum class SequenceId{
        Sequence_Idle,
        Sequence_SelfCheck,
        Sequence_OpenBox,
        Sequence_CloseBox,
        Sequence_CannelTest,
        Sequence_Test,
        Sequence_SimpleAction,
        Sequence_QrDecode,
        Sequence_camView
    };
    Q_ENUM(SequenceId)

    enum class SequenceResult{
        Result_ok,
        Result_fail,
        Result_Simple_ok,
        Result_SelfCheck_ok,
        Result_SelfCheck_err1,
        Result_SelfCheck_err2,
        Result_OpenBox_ok,
        Result_OpenBox_err,
        Result_CloseBox_ok,
        Result_CloseBox_err,
        Result_CannelTest_ok,
        Result_Test_DataErr,
        Result_Test_ProcessErr
    };
    Q_ENUM(SequenceResult)

    enum class TimeState{
        idle,
        wait,
        running
    };
    Q_ENUM(TimeState)

    Q_PROPERTY(bool door READ readDoorState NOTIFY doorStateChanged)
    Q_PROPERTY(bool box READ readBoxState NOTIFY boxStateChanged)

    Q_INVOKABLE bool sequenceDo(SequenceId id);
    Q_INVOKABLE QString getCurrTestTime();
    Q_INVOKABLE bool actionDo(QString device, int value, int param1, int param2, int param3);
    Q_INVOKABLE void sequenceCancel();
    Q_INVOKABLE QString sequenceMessage();
    Q_INVOKABLE void sequenceSetPanel(QString PanelName);
    Q_PROPERTY(QStringList TestList READ getTestList NOTIFY testListChanged)

    Q_INVOKABLE bool sequenceInit();
    Q_INVOKABLE void openErr(QString err){emit errOccur(err);}
    QStringList getTestList();
    bool readBoxState(){return bBoxState;}
    bool readDoorState(){return bDoorState;}

    Q_INVOKABLE int getAbs();
    Q_INVOKABLE bool setAbs(int value);
    Q_INVOKABLE int getGain();
    Q_INVOKABLE bool setGain(int value);    
    Q_INVOKABLE int getWhiteBalance();
    Q_INVOKABLE bool setWhiteBalance(int value);

    Q_INVOKABLE bool startView(int id);
    Q_INVOKABLE bool stopView();
    Q_INVOKABLE bool saveView();
    Q_INVOKABLE void lxDebug();
    Q_INVOKABLE void showAnaImg(int type,int light);
    Q_INVOKABLE void qrSet(bool bopenlight, bool scale, bool handlimage, int bin,int pox);

    ImageProvider *imageProvider;

signals:
    void sequenceFinish(SequenceResult result);
    void processFinish(int total,int finish);
    void panelNameChanged();
    void testListChanged();
    void doorStateChanged();
    void boxStateChanged();
    void errOccur(QString error);
    void callQmlRefeshView();
    void callQmlRefeshQrImg();
    void callQmlRefeshAnaMainImg();
    void callQmlRefeshData(int index, QVector<int> item,QVector<int> value);
    void qrDecode(QString info);

public slots:
    void SequenceTimeout();
    void WaitSequenceTimeout();
    void ActionFinish(QByteArray data);
    void CameraView(QImage img);    
    void errReceive(int code);

private:
    bool ReadTestProcess(QString panel);
    bool WriteTestProcess(QString panel);
    bool ReadMask(QString mask);
    bool DoAction(QDomElement action,bool isChild);
    bool FindAction(bool bFinishAction);
    int CalSteps(QDomElement element);
    bool FormatAction();
    void FinishSequence();    
    void setSenorState(char char1, char char2);

    QDomElement sequenceAction;
    QTimer *timer;
    QTimer *waitNextSequence;
    SequenceId currSequenceId;
    SequenceId nextSequenceId;
    int waitCount;
    QString message;
    SerialMgr *serialMgr;
    ImageCapture *imageCapture;
    TestMgr *testMgr;
    bool bFinishAction;
    TimeState durationState;
    int nDuration;
    bool firstCapture;    
    bool bCannelSequence;
    bool bDoorState;
    bool bBoxState;
    ImageAnalysis *imageAna;

    QRcoder *qr;
    QList<action> actList;
    bool listNextAction(bool first);
    void qrDect();
    bool bQrOpenLight = true;
};

static Sequence *sequence;
static QObject *sequence_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    //Sequence *sequence = new Sequence();
    return sequence;
}
#endif // SEQUENCE_H
