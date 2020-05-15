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
#include "printmgr.h"

#include "log.h"
#include "dao/testmgr.h"
#include "cvcapture.h"
#include "cameraplayer.h"

class Sequence : public QObject
{
    Q_OBJECT
public:
    explicit Sequence(QObject *parent = nullptr);
    enum SequenceId{
        Sequence_Idle,
        Sequence_SelfCheck,
        Sequence_OpenBox,
        Sequence_CloseBox,
        Sequence_CannelTest,
        Sequence_Test,
        Sequence_LoopTest,
        Sequence_SimpleAction,
        Sequence_QrDecode,
        Sequence_Pierce,
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
        Result_Test_finish,
        Result_Test_unfinish,
        Result_LoopTest_finish,
        Result_CannelLoopTest_ok,
        Result_Print_finish,
        Result_Test_DataErr,
        Result_Test_ProcessErr,
        Result_Box_Valid,
        Result_Box_Invalid,
        Result_NULL
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
    Q_PROPERTY(QStringList LoopTestList READ getLoopTestList NOTIFY testListChanged)
    Q_INVOKABLE bool sequenceInit();
    Q_INVOKABLE void openErr(QString err){emit errOccur(err);}
    QStringList getTestList();
    QStringList getLoopTestList();
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

    Q_INVOKABLE double getDefinition(){return imageCapture->getDefinition();}
    Q_INVOKABLE double getDefinition2(){return imageCapture->getDefinition2();}
    Q_INVOKABLE bool validBox(){return bValidBox;}

    Q_PROPERTY(int fan1Speed READ fan1Speed NOTIFY fan1SpeedChanged)
    Q_PROPERTY(int fan2Speed READ fan2Speed NOTIFY fan2SpeedChanged)
    Q_PROPERTY(int fan3Speed READ fan3Speed NOTIFY fan3SpeedChanged)
    Q_INVOKABLE void autoFocus();

    Q_INVOKABLE bool printTest();
    ImageProvider *imageProvider;

    Q_INVOKABLE bool isTesting(){return currSequenceId == SequenceId::Sequence_Test;}
    Q_INVOKABLE void changeTitle(QString title);

    Q_INVOKABLE bool loopTest(QString testName, int count);
    Q_INVOKABLE bool isLoopTesting(){return currSequenceId == SequenceId::Sequence_LoopTest;}

    Q_INVOKABLE int boxParam(){return boxparam;}

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
    void fan1SpeedChanged();
    void fan2SpeedChanged();
    void fan3SpeedChanged();
    void autoFocusNotify(int status, int value);
    void titleNotify(int titleparam, QString title);

public slots:
    void SequenceTimeout();
    void WaitSequenceTimeout();
    void ActionFinish(QByteArray data);
    void PrintFinish(QByteArray data);
    void errFinish(QByteArray data);
    void CameraView(QImage img);    
    void errReceive(ERROR_CODE code);

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
    cvCapture *cvcap;
    TestMgr *testMgr;
    printmgr *printer;
    CameraPlayer *camera;

    bool bFinishAction;
    TimeState durationState;
    int nDuration;
    bool firstCapture;    
    bool bCannelSequence;
    bool bDoorState;
    bool bBoxState;
    bool bValidBox;

    ImageAnalysis *imageAna;

    QRcoder *qr;
    QList<action> actList;
    bool listNextAction(bool first);
    void qrDect();
    void PierceDect();
    void SwitchDoor();
    bool bQrOpenLight = true;
    bool bAutoFocus = false;

    int t_fan1Speed;
    int t_fan2Speed;
    int t_fan3Speed;
    int fan1Speed(){return t_fan1Speed;}
    int fan2Speed(){return t_fan2Speed;}
    int fan3Speed(){return t_fan3Speed;}
    void fan1SetSpeed(int speed);
    void fan2SetSpeed(int speed){t_fan2Speed = speed;emit fan2SpeedChanged();}
    void fan3SetSpeed(int speed){t_fan3Speed = speed;emit fan3SpeedChanged();}

    QString title;
    int titleparam;
    int boxparam;

    int loopTestCount;
    int loopTestCurrCount;
    QString loopTestName;
    bool continueLoopTest();
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
