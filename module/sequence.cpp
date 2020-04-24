#include "sequence.h"
#include <QDebug>

#include "exglobal.h"
#include "actionparser.h"
#include "imageanalysis.h"
#include "sqlitemgr.h"

#define AUTOFOCUS_MIN 2600
#define AUTOFOCUS_MAX 7000
#define AUTOFOCUS_STEP 100
#define AUTOFOCUS_MAXOFFSET 6

static int autoFocus_CurrPoint = 0;
static int autoFocus_ClarityPoint = 0;
static double autoFocus_ClarityValue = 0;
static int autoFocus_JumpStep = 0;
static bool autoFocus_dec = false;

static QDomDocument doc;
static char currOrder = 0;
static int currCameraCaptureType = 0;
static int currCameraCycle = 0;
static QDateTime testStartTime;
Sequence::Sequence(QObject *parent) : QObject(parent)
{    
    imageAna = new ImageAnalysis();
    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    timer->setSingleShot(true);
    waitNextSequence = new QTimer(this);
    waitNextSequence->setSingleShot(true);
    connect(timer,SIGNAL(timeout()),this,SLOT(SequenceTimeout()));
    connect(waitNextSequence,SIGNAL(timeout()),this,SLOT(WaitSequenceTimeout()));

    serialMgr = new SerialMgr();
    connect(serialMgr,&SerialMgr::finishAction,this,&Sequence::ActionFinish);
    connect(serialMgr,&SerialMgr::errAction,this,&Sequence::errFinish);
    connect(serialMgr,&SerialMgr::errOccur,this,&Sequence::errReceive);

    imageCapture = new ImageCapture();
    imageProvider = new ImageProvider();
    cvcap = new cvCapture();
    connect(imageCapture,&ImageCapture::finishCapture,this,&Sequence::ActionFinish);
    connect(imageCapture,&ImageCapture::reView,this,&Sequence::CameraView);

    camera = new CameraPlayer();

    testMgr = new TestMgr();

    qr = new QRcoder();
    connect(qr,&QRcoder::finishQRcode,this,&Sequence::ActionFinish);

    printer = new printmgr();
    connect(printer,&printmgr::finishPrint,this,&Sequence::PrintFinish);

    currSequenceId = SequenceId::Sequence_Idle;    
    bCannelSequence = false;

    bValidBox = false;

    //actionDo("Sensor",0,0,0);
    serialMgr->serialWrite(ActionParser::ParamToByte("AutoData",1,0,0,0));
    //ReadMask(QCoreApplication::applicationDirPath()+"/pos");
    //imageAna->SetMask(ExGlobal::getReagentBox("201"),0);

    t_fan1Speed = t_fan2Speed = t_fan3Speed = 0;
#if 0
    qDebug()<<"startTrans:"<<sqlitemgrinstance->StartTransations();
    testMgr->TestCreate("123456","201");
    for (int i = 0; i < 121; i++)
        testMgr->InsertData(i,5,1,120+i);
    qDebug()<<"endTrans:"<<sqlitemgrinstance->EndTransations();
#endif
}

bool Sequence::sequenceInit(){    
    if (!ReadTestProcess(QCoreApplication::applicationDirPath()+"/FLASHDXcn"))    
        return false;
    return true;
}


QStringList Sequence::getTestList(){
    QStringList test;
    QDomElement root = doc.documentElement();
    for (QDomElement e = root.firstChildElement("PanelTest"); !e.isNull(); e = e.nextSiblingElement("PanelTest"))
    {
        QString panelCode = e.attribute("PanelCode");
        if (panelCode.length() > 4 && panelCode.startsWith("90"))
            continue;
        test<<e.attribute("PanelName");
    }
    return test;
}

void Sequence::sequenceSetPanel(QString panelName)
{
    QDomElement root = doc.documentElement();

    for (QDomElement e = root.firstChildElement("PanelTest"); !e.isNull(); e = e.nextSiblingElement("PanelTest"))
    {
        if (e.attribute("PanelName")==panelName)
        {
            ExGlobal::setPanelName(e.attribute("PanelName"));
            ExGlobal::setPanelCode(e.attribute("PanelCode"));
            emit panelNameChanged();
            break;
        }
    }
}

QString Sequence::getCurrTestTime(){
    return QDateTime::fromMSecsSinceEpoch(QDateTime::currentDateTime().toMSecsSinceEpoch() - testStartTime.toMSecsSinceEpoch()).toUTC().toString("hh:mm:ss");
}

bool Sequence::sequenceDo(SequenceId id)
{
    qDebug()<<"sequenceDo,currSequenceId:"<<currSequenceId<<id;
    if (currSequenceId != SequenceId::Sequence_Idle)
    {
        this->nextSequenceId = id;
        waitCount = 0;
        waitNextSequence->start(1000);
        return false;
    }
    this->currSequenceId = id;
    QDomElement root = doc.documentElement();    
    if (id == SequenceId::Sequence_Test)
    {
        for (sequenceAction = root.firstChildElement("PanelTest"); !sequenceAction.isNull(); sequenceAction = sequenceAction.nextSiblingElement("PanelTest"))
        {
            if (sequenceAction.attribute("PanelCode")==ExGlobal::panelCode())
            {
                testStartTime = QDateTime::currentDateTime();
                QString current_time_str = testStartTime.toString("yyyyMMdd_hhmmss");
                Log::setDir(QCoreApplication::applicationDirPath()+"/"+current_time_str);
                imageCapture->start_capturing(ImageCapture::CaptureMode::Capture);
                camera->CameraStart(CameraPlayer::CaptureMode::Capture);
                ExGlobal::setPanelName(sequenceAction.attribute("PanelName"));

                //ExGlobal::setReagentBox("202");
                imageAna->SetMask(ExGlobal::getReagentBox(ExGlobal::reagentBox()),0);
                testMgr->TestCreate(ExGlobal::boxSerial());
                break;
            }
        }
    }
    else if(id == SequenceId::Sequence_OpenBox)
    {
        //sequenceAction = root.firstChildElement("OpenDoor");
        if (bDoorState == false)//close
            sequenceAction = root.firstChildElement("OpenDoor");
        else
            sequenceAction = root.firstChildElement("CloseDoor");
    }
    else if(id == SequenceId::Sequence_CloseBox){
        //sequenceAction = root.firstChildElement("CloseDoor");
        if (bDoorState == false)//close
            sequenceAction = root.firstChildElement("OpenDoor");
        else
            sequenceAction = root.firstChildElement("CloseDoor");
    }
    else if(id == SequenceId::Sequence_SelfCheck){
        sequenceAction = root.firstChildElement("SelfCheck");
    }
    else if(id == SequenceId::Sequence_CannelTest){
        sequenceAction = root.firstChildElement("StopTest");
    }
    else if(id == SequenceId::Sequence_QrDecode)
    {
        qrDect();
        return true;
    }
    else if(id == SequenceId::Sequence_Pierce)
    {
        PierceDect();
        return true;
    }

    if (sequenceAction.isNull())
        return false;

    if (!FormatAction())
        return false;

    bFinishAction = false;
    durationState = TimeState::idle;
    bCannelSequence = false;

    FindAction(false);
    return true;
}

bool Sequence::actionDo(QString device, int value, int param1, int param2, int param3)
{
    qDebug()<<"actionDo:"<<device<<",currSequenceId:"<<currSequenceId;
    if (currSequenceId != SequenceId::Sequence_Idle)
        return false;
    currSequenceId = SequenceId::Sequence_SimpleAction;
    QByteArray send = ActionParser::ParamToByte(device,value,param1,param2,param3);
    currOrder = send[7];
    serialMgr->serialWrite(send);
    return true;
}

void Sequence::sequenceCancel()
{
    qDebug()<<"sequenceCancel:"<<",currSequenceId:"<<currSequenceId<<",durationState:"<<durationState<<",bFinishAction:"<<bFinishAction;
    if (!isTesting()&&!isLoopTesting())
        return;
    imageCapture->stop_capturing();
    camera->CameraStop();

    if(durationState == TimeState::running)
        timer->stop();

    if (bFinishAction) {
        currSequenceId = SequenceId::Sequence_Idle;
        sequenceDo(SequenceId::Sequence_CannelTest);
    }
    else {
        bCannelSequence = true;
    }
}

void Sequence::SequenceTimeout()
{
    qDebug()<<"Action Duration finish";
    durationState = TimeState::idle;
    if (bFinishAction)
        FindAction(true);

}

void Sequence::WaitSequenceTimeout()
{
    qDebug()<<"WaitSequenceTimeout,count:"<<waitCount;
    waitCount++;
    if (waitCount < 10)
    {
        if (currSequenceId != SequenceId::Sequence_Idle)
            waitNextSequence->start(1000);
        else
            sequenceDo(nextSequenceId);
    }
}

void Sequence::ActionFinish(QByteArray data)
{
    qDebug()<<"Sequence ActionFinish:"<<data.toHex(' ');

    if (data[7] == '\x72'){
        //if (currSequenceId != SequenceId::Sequence_Test){
        if (currSequenceId == SequenceId::Sequence_Idle){
            if (bDoorState == false)//close
                sequenceDo(SequenceId::Sequence_OpenBox);
            else
                sequenceDo(SequenceId::Sequence_CloseBox);
        }
        return;
    }

    if (currOrder != data[7])
        return;

    if (data.length()>11 && data[0] == '\xaa' && data[data.length()-1]=='\x55')
    {        
        if (data[1] == '\x02')
        {
            if (data[7] == '\x62'){
                ExGlobal::settempversion(data.mid(13,5).data());
            }
            else if(data[7] == '\x20'){
            }
            else if(data[7] == '\x1F'){
                if (data[12] == '\x00')
                    fan1SetSpeed((data[13]<<8)+data[14]);
                else if (data[12] == '\x01')
                    fan1SetSpeed((data[13]<<8)+data[14]);
                else if (data[12] == '\x02')
                    fan1SetSpeed((data[13]<<8)+data[14]);
                int fan = data[12];
                int fanspeed = (data[13]<<8)+data[14];
                if (isLoopTesting())
                    Log::LogByFile("LoopTest.txt",QString("风扇:%1,转速：%2").arg(fan).arg(fanspeed));
            }
        }
        else if (data[1] == '\x01')
        {
            if (data[7] == '\x62')
                ExGlobal::setctrlversion(data.mid(13,5).data());
            else if (data[7] == '\x32')
                setSenorState(data[12],data[13]);
            else if(bAutoFocus == true && data[7] == '\x70' && data.length()>12 && (data[12] == '\x2a'||data[12] == '\x2b'))
                autoFocus_JumpStep = 1;
        }
        else if(data[1] == '\x03')//CameraCapture
        {
            qDebug()<<"CameraCapture ActionFinish,type="<<currCameraCaptureType;
            if (data[7] == '\xa0' && data[10] == '\x00' && currCameraCaptureType == 5){
                if (currCameraCycle < 2)                
                    imageAna->FirstImage(imageCapture->getyData(),imageCapture->imagetype);
                else                
                    imageAna->AddImage(imageCapture->getyData(),imageCapture->imagetype);

                imageProvider->anaMainImg = imageAna->getMainImg(0,1);
                emit callQmlRefeshAnaMainImg();
                QVector<int> item = imageAna->getItem();
                QVector<int> value = imageAna->getValue();
                QVector<int> posIndex = imageAna->getIndex();
                emit callQmlRefeshData(currCameraCycle,item,value);

                SqliteMgr::sqlitemgrinstance->StartTransations();
                for(int i = 0; i < item.size(); i++){
                    testMgr->InsertData(posIndex[i],item[i],currCameraCycle,value[i]);
                }
                SqliteMgr::sqlitemgrinstance->EndTransations();

                /*
                QString saveStr;
                if (currCameraCycle == 1){
                    saveStr = "cycle";
                    for (int i = 0; i < item.size(); i++){
                        saveStr += ",";
                        //saveStr += Log::getPosName(item[i]);
                        saveStr += ExGlobal::getPosName(item[i]);
                    }
                    Log::LogPos(saveStr);
                }
                saveStr = QString::number(currCameraCycle);
                for (int i = 0; i < item.size(); i++){
                    saveStr += ",";
                    saveStr += QString::number(value[i]);
                }
                Log::LogPos(saveStr);
                //*/
            }
            else if(data[7] == '\xa0' && data[10] != '\x00'){
                //errReceive(0x300 + data[10]);
                errReceive(ERROR_CODE_CAM_CONNECT);
                return;
            }

            currCameraCaptureType = 0;
        }        
    }
    else if (data[1] == '\x04'){
        if (data[7] == '\xB0'){     //二维码识别
            //if (data[2] == '\x01' || data[2] == '\x03')
                emit callQmlRefeshQrImg();
            data.remove(0,10);
            emit qrDecode(data.data());
        }
        else if(data[7] == '\xB1'){ //刺穿识别
            if (data[2] == '\x03')
                emit sequenceFinish(SequenceResult::Result_Pierce_Yes);
            else if (data[2]&0x04)
                emit sequenceFinish(SequenceResult::Result_Pierce_Damage);
            else if ((data[2]&0x02) == 0)
                emit sequenceFinish(SequenceResult::Result_Pierce_No);
            else
                emit sequenceFinish(SequenceResult::Result_Pierce_Yes_NoQr);
        }
    }

    if (bCannelSequence)
    {
        currSequenceId = SequenceId::Sequence_Idle;
        sequenceDo(SequenceId::Sequence_CannelTest);
        return;
    }

    if (currSequenceId == SequenceId::Sequence_SimpleAction)
    {
        currSequenceId = SequenceId::Sequence_Idle;
        emit sequenceFinish(SequenceResult::Result_Simple_ok);
    }
    else if (currSequenceId == SequenceId::Sequence_QrDecode || currSequenceId == SequenceId::Sequence_Pierce){
        if (!listNextAction(false))
        {
            currSequenceId = SequenceId::Sequence_Idle;
        }
    }
    else{
        bFinishAction = true;
        //qDebug()<<"Sequence ActionFinish,durationState:"<<durationState;


        Log::LogTime(QString("Action finish,duration state:%1").arg((int)durationState));

        if (durationState == TimeState::idle)
            FindAction(true);
        else if(durationState == TimeState::wait)
        {
            durationState = TimeState::running;
            Log::LogTime(QString("Start timer:%1").arg(nDuration));
            timer->start(nDuration);
        }
    }
}

void Sequence::errFinish(QByteArray data){
    qDebug()<<"Sequence errFinish:"<<data.toHex(' ');
    QString errStr;
    if (data.length()>11 && data[0] == '\xaa' && data[data.length()-1]=='\x55'){
        if (data[1] == '\x02'){            
            if (data[7] == '\x89'){
                int temp1 = (data[12]<<8)+data[13];
                int temp2 = (data[14]<<8)+data[15];
                int chan = data[11];
                errStr = QString("通道%1两传感器温差过大，一传感器温度：%2，一传感器温度：%3").arg(chan).arg(temp1).arg(temp2);
                emit errOccur(errStr+" \n\n错误码："+QString::number(ERROR_CODE_TEMP_DIFF,16));
            }
            else if(data[7] == '\x24')
            {
                errStr = QString("温度错误！");
                emit errOccur(errStr+" \n\n错误码："+QString::number(ERROR_CODE_TEMP_ERR,16));
            }

        }
        else {
            emit errOccur("未知错误："+data.toHex(' '));
        }
        sequenceCancel();
    }
}
//因为打印动作可能同步执行，所有独立接收函数
void Sequence::PrintFinish(QByteArray data){
    if (data[7] == '\xB1'){
        emit sequenceFinish(SequenceResult::Result_Print_finish);
    }
}
void Sequence::FinishSequence()
{
    SequenceResult out = SequenceResult::Result_NULL;
    qDebug()<<"FinishSequence:"<<currSequenceId;
    if (currSequenceId == SequenceId::Sequence_SelfCheck)
    {        
        out = SequenceResult::Result_SelfCheck_ok;
    }
    else if(currSequenceId == SequenceId::Sequence_CloseBox)
    {
        bDoorState = false;
        out = SequenceResult::Result_CloseBox_ok;
    }
    else if(currSequenceId == SequenceId::Sequence_OpenBox)
    {
        bDoorState = true;
        out = SequenceResult::Result_OpenBox_ok;
    }
    else if(currSequenceId == SequenceId::Sequence_CannelTest)
        out = SequenceResult::Result_CannelTest_ok;
    else if(currSequenceId == SequenceId::Sequence_Test)
    {
        imageCapture->stop_capturing();
        camera->CameraStop();
        currSequenceId = SequenceId::Sequence_Idle;
        if (imageAna->getItem().size() > 45)
        {
            int testid = testMgr->TestClose(2);
            ExGlobal::addTest();            
            ExGlobal::pTestResultModel->setTestid(testid);
            out = SequenceResult::Result_Test_finish;
        }
        else
        {
            testMgr->TestClose(1);
            out = SequenceResult::Result_Test_unfinish;
        }

        Log::setDir(QCoreApplication::applicationDirPath());

    }
    else if(currSequenceId == SequenceId::Sequence_LoopTest)
    {
        if (continueLoopTest())
            return;
        else
        {
            imageCapture->stop_capturing();
            camera->CameraStop();
            currSequenceId = SequenceId::Sequence_Idle;
            testMgr->TestClose(3);
            out = SequenceResult::Result_LoopTest_finish;
        }
    }
    currSequenceId = SequenceId::Sequence_Idle;
    if (out != SequenceResult::Result_NULL)
        emit sequenceFinish(out);
}

bool Sequence::ReadTestProcess(QString panel)
{
    QFile xmlfile(panel+".xml");
    qDebug("ReadTestProcess");
    if (!xmlfile.open(QFile::ReadOnly))
    {
        errReceive(ERROR_CODE_FILE_READ);
        qDebug()<<"file err:"<<xmlfile.error()<<",string:"<<xmlfile.errorString()<<",file:"<<xmlfile.fileName()<<",dir:"<<QDir::currentPath()<<",apppath:"<<QCoreApplication::applicationDirPath();
        return false;
    }

    if (!doc.setContent(&xmlfile))
    {
        xmlfile.close();
        qDebug()<<"xml setContent err return";
        errReceive(ERROR_CODE_FILE_FORMAT);
        return  false;
    }
    xmlfile.close();

    QDomElement root = doc.documentElement();    
    if (root.hasAttribute("ProjectMode")){
        //ExGlobal::ProjectMode = root.attribute("ProjectMode").toInt();
    }

    if (root.hasAttribute("DefaultPanelCode"))
    {
        for (QDomElement panel = root.firstChildElement("PanelTest"); !panel.isNull(); panel = panel.nextSiblingElement("PanelTest"))
        {
            if (panel.attribute("PanelCode")==root.attribute("DefaultPanelCode"))
            {
                qDebug()<<"DefaultPanelCode:"<<panel.attribute("PanelCode");
                ExGlobal::setPanelName(panel.attribute("PanelName"));
                ExGlobal::setPanelCode(panel.attribute("PanelCode"));
                break;
            }
        }
    }

    return true;
}

bool Sequence::ReadMask(QString mask)
{
    qDebug()<<"ReadMask:"<<mask;
    QFile maskfile(mask+".bin");
    if (!maskfile.open(QFile::ReadOnly))
    {
        errReceive(ERROR_CODE_FILE_MODEL);
        qDebug()<<"file err:"<<maskfile.error()<<",string:"<<maskfile.errorString()<<",file:"<<maskfile.fileName()<<",dir:"<<QDir::currentPath()<<",apppath:"<<QCoreApplication::applicationDirPath();
        return false;
    }
    QDataStream in(&maskfile);
    char r[121];
    int readlen = in.readRawData(r,121);
    qDebug()<<"ReadMask:readlen="<<readlen;
    imageAna->SetMask(r,0);
    maskfile.close();
    return true;
}

bool Sequence::WriteTestProcess(QString panel)
{
    QFile xmlfile(panel+".xml");

    if (!xmlfile.open(QFile::WriteOnly|QFile::Truncate))
        return false;
    QTextStream out(&xmlfile);
    doc.save(out,4);
    xmlfile.close();
    return true;
}

bool Sequence::DoAction(QDomElement action,bool isChild)
{    
    nDuration = action.attribute("Duration").toInt();
    int nPreTime = action.attribute("pretime").toInt();
    qDebug()<<"DoAction:"<<action.attribute("Device")<<"step:"<<action.attribute("No")<<"duration:"<<action.attribute("Duration")<<"Value:"<<action.attribute("ActionValue")<<"Param:"<<action.attribute("ActionParam1");

    message.sprintf("No: %d; Duration:%d; Device:",action.attribute("No").toInt(),action.attribute("Duration").toInt());
    message += action.attribute("Device")+","+action.attribute("ActionValue")+","+action.attribute("ActionParam1")+","+action.attribute("ActionParam2");
    Log::LogWithTime(message);

    if (isLoopTesting())
        //Log::LogByFile("LoopTest.txt",QString("第%1/%2大循环,%3").arg(loopTestCurrCount).arg(loopTestCount).arg(message));
        Log::LogByFile("LoopTest.txt",message);

    if (isChild){
        QDomElement p = action.parentNode().toElement();
        int cycle = p.attribute("currcycle").toInt();
        if (action.attribute("No").toInt() == 1){
            Log::LogTime(QString("\ncycle=%1").arg(cycle));
        }
    }
    Log::LogTime(message);
    bFinishAction = false;    

    if (action.attribute("Device")=="CameraCapture")
    {
        currCameraCaptureType = action.attribute("ActionValue").toInt();
        currCameraCycle = 0;
        QString captureFileName;
        if(isChild){
            QDomElement p = action.parentNode().toElement();
            int cycle = p.attribute("currcycle").toInt();
            currCameraCycle = cycle;
            if (action.attribute("ActionValue") == "4")
                captureFileName = "cycleb"+QString("%1").arg(cycle,2,10,QChar('0'));
            else
                captureFileName = "cycle"+QString("%1").arg(cycle,2,10,QChar('0'));
        }
        else if (action.attribute("ActionValue") == "1")
        {
            captureFileName = "Dry";
        }
        else if (action.attribute("ActionValue") == "2")
        {
            captureFileName = "Black";
        }
        else if (action.attribute("ActionValue") == "3")
            captureFileName = "Fill";
        imageCapture->setFileName(captureFileName);
        camera->setFileName(captureFileName);

        currOrder = '\xA0';
        imageCapture->setImageCount(action.attribute("ActionParam1").toInt());
        if (imageCapture->waitStop())
            imageCapture->start();
#if 1
        camera->setImageCount(action.attribute("ActionParam1").toInt());
        if (camera->waitStop())
            camera->start();
#endif
    }
    else if(action.attribute("Device")=="Door" || action.attribute("Device")=="Light" || action.attribute("Device")=="Temperature"
            ||action.attribute("Device")=="V1" || action.attribute("Device")=="V2" || action.attribute("Device")=="V3"
            ||action.attribute("Device")=="VP" || action.attribute("Device")=="Pump" || action.attribute("Device")=="Query"
            ||action.attribute("Device")=="SetPID" || action.attribute("Device")=="Fan")
    {
        QByteArray send = ActionParser::ActionToByte(action);
        currOrder = send[7];
        serialMgr->serialWrite(send);
    }

    if (isChild){
        QDomElement p = action.parentNode().toElement();
        nPreTime += p.attribute("Duration").toInt()*(p.attribute("currcycle").toInt()-1) + p.attribute("pretime").toInt();
        message.sprintf("总时间：%d秒;\t完成时间：%d秒;\t第 %d 步; \t耗时：%d秒\n\t第%d循环; \t第 %d 步; \t耗时：%d秒\t设备：",sequenceAction.attribute("Duration").toInt()/1000,nPreTime/1000,
                        p.attribute("No").toInt(),p.attribute("Duration").toInt()/1000,p.attribute("currcycle").toInt(),action.attribute("No").toInt(),action.attribute("Duration").toInt()/1000);
    }
    else
        message.sprintf("总时间：%d秒;\t完成时间：%d秒;\t第 %d 步; \t耗时：%d秒\t设备：",sequenceAction.attribute("Duration").toInt()/1000,nPreTime/1000,action.attribute("No").toInt(),action.attribute("Duration").toInt()/1000);
    message += action.attribute("Device")+","+action.attribute("ActionValue")+","+action.attribute("ActionParam1")+","+action.attribute("ActionParam2");

    if (isTesting()||isLoopTesting()){
        emit processFinish(sequenceAction.attribute("Duration").toInt(),nPreTime);
        int remain = (sequenceAction.attribute("Duration").toInt() - nPreTime)/1000;
        QString title = "预计剩余"+QString::number(remain)+"秒";
        if (isLoopTesting()){
            int oneloop = sequenceAction.attribute("Duration").toInt()/1000;
            remain += (loopTestCount-loopTestCurrCount)*oneloop;
            title = "耐力测试，正在测试第"+QString::number(loopTestCurrCount)+"/"+QString::number(loopTestCount)+"循环，预计剩余"+QString::number(remain)+"秒";
            nPreTime = nPreTime + (loopTestCurrCount-1)*oneloop*1000;
            emit titleNotify(nPreTime/(oneloop*loopTestCount)+100,title);
            Log::LogByFile("LoopTest.txt",QString("total time=%1,completion time=%2").arg(oneloop*loopTestCount).arg(nPreTime));
        }
        else
        {
            emit titleNotify(nPreTime/(sequenceAction.attribute("Duration").toInt()/1000)+100,title);
            Log::LogWithTime(QString("total time=%1,completion time=%2").arg(sequenceAction.attribute("Duration")).arg(nPreTime));
        }
    }

    Log::LogTime(QString("start timer,duration=%1,type=%2").arg(nDuration).arg(action.attribute("DurationType")));
    durationState = TimeState::running;
    if (nDuration == 0)
    {
        SequenceTimeout();
    }
    else if(action.attribute("DurationType")=="1")
    {
        Log::LogTime(QString("Timing before action"));
        timer->start(nDuration);
    }
    else
    {
        Log::LogTime(QString("Timing after action"));
        durationState = TimeState::wait;
    }

    return true;
}

QString Sequence::sequenceMessage(){    
    return message;
}
bool Sequence::FindAction(bool bFinishAction)
{    
    int totalStep = sequenceAction.attribute("steps").toInt();
    int currStep = sequenceAction.attribute("currstep").toInt();
    //int nTotalTime = sequenceAction.attribute("Duration").toInt();

    QDomElement e = sequenceAction.firstChildElement();
    while (!e.isNull()) {
        if (e.hasAttribute("No") && e.attribute("No").toInt() == currStep){
            if (e.tagName() == "Action"){
                if (!bFinishAction)
                    return DoAction(e,false);
                else if(currStep == totalStep)
                {
                    //emit processFinish(nTotalTime,nTotalTime);
                    FinishSequence();
                    return true;
                }
                else
                {
                    currStep++;
                    sequenceAction.setAttribute("currstep",currStep);
                    return FindAction(false);
                }
            }else if(e.tagName() == "Actions"){
                int nCycle = e.attribute("cycle").toInt();
                int nCurrCycle = e.attribute("currcycle").toInt();
                int nChildCurrStep = e.attribute("currstep").toInt();
                int nChildTotalStep = e.attribute("steps").toInt();
                QDomNodeList list = e.childNodes();
                for (int i = 0; i<list.count();i++)
                {
                    QDomNode n = list.at(i);
                    if (n.isElement()){
                        QDomElement ne = n.toElement();
                        if (ne.tagName() == "Action" && ne.attribute("No").toInt() == nChildCurrStep){
                            if (!bFinishAction)
                                return DoAction(ne,true);
                            else if(nChildCurrStep == nChildTotalStep){
                                if (nCurrCycle == nCycle){
                                    if (currStep == totalStep){
                                        //emit processFinish(nTotalTime,nTotalTime);
                                        FinishSequence();
                                        return true;
                                    }
                                    else{
                                        currStep++;
                                        sequenceAction.setAttribute("currstep",currStep);
                                    }
                                }
                                else {
                                    nChildCurrStep = 1;
                                    nCurrCycle++;
                                    e.setAttribute("currstep",nChildCurrStep);
                                    e.setAttribute("currcycle",nCurrCycle);
                                }
                            }
                            else
                            {
                                nChildCurrStep++;
                                e.setAttribute("currstep",nChildCurrStep);
                            }
                            return FindAction(false);
                        }
                    }
                }
            }
        }
        e = e.nextSiblingElement();
    }

    return true;
}

int Sequence::CalSteps(QDomElement element)
{
    int nStep = 0;
    QDomNode node = element.firstChild();
    while(!node.isNull())
    {
        QDomElement e = node.toElement();
        if (!e.isNull() && (e.tagName()=="Action"||e.tagName()=="Actions"))
            nStep++;
        node = node.nextSibling();
    }
    return nStep;
}

bool Sequence::FormatAction(){
    bool result = false;
    int totalStep = CalSteps(sequenceAction);
    int nFinishTime = 0;

    sequenceAction.setAttribute("currstep",1);
    sequenceAction.setAttribute("steps",totalStep);

    for (int i = 1; i <= totalStep; i++){
        QDomNode node = sequenceAction.firstChild();
        while (!node.isNull()) {
            QDomElement e = node.toElement();
            if (!e.isNull()&& e.hasAttribute("No") && e.attribute("No").toInt() == i){
                e.setAttribute("pretime",nFinishTime);
                if (e.tagName()=="Action")
                {
                    nFinishTime += e.attribute("Duration").toInt();
                }
                else if(e.tagName()=="Actions"){
                    int childTotalStep = CalSteps(e);
                    e.setAttribute("steps",childTotalStep);
                    int cycle = e.attribute("cycle").toInt();
                    int nChildFinshTime = 0;
                    for (int j = 1; j <= childTotalStep; j++)
                    {
                        QDomNode cnode = e.firstChild();
                        bool cresult = false;
                        e.setAttribute("currcycle",1);
                        e.setAttribute("currstep",1);
                        while(!cnode.isNull())
                        {
                            QDomElement ce = cnode.toElement();
                            if (!ce.isNull()&& ce.hasAttribute("No") && ce.attribute("No").toInt() == j){
                                ce.setAttribute("pretime",nChildFinshTime);
                                nChildFinshTime += ce.attribute("Duration").toInt();
                                cresult = true;
                                break;
                            }
                            cnode = cnode.nextSibling();
                        }
                        if (!cresult)
                            return false;
                    }
                    e.setAttribute("Duration",nChildFinshTime);
                    nFinishTime += cycle*nChildFinshTime;
                }
                result = true;
                break;
            }
            node = node.nextSibling();
        }
        if (!result)
            return false;
    }
    sequenceAction.setAttribute("Duration",nFinishTime);
    return true;
}

static bool FirstCheckSensor = true;
void Sequence::setSenorState(char char1, char char2)
{
    bool sensor;
    //qDebug()<<"setSenorState,char1:"<<char1<<",char2:"<<char2;
    qDebug()<<"setSensorState,bBoxState:"<<bBoxState<<",bDoorState:"<<bDoorState<<",char1:"<<QString::number(char1,16)<<",char2:"<<QString::number(char2,16);
    if (char1&0x08)
        sensor = true;
    else
        sensor = false;
    if (FirstCheckSensor == true || sensor != bBoxState)
    {
        bBoxState = sensor;
        emit boxStateChanged();
    }
    if (char2&0x80)
        sensor = true;
    else
        sensor = false;
    if (sensor != bDoorState)
    {
        bDoorState = sensor;
        emit doorStateChanged();
    }
    FirstCheckSensor = false;
    //qDebug()<<"bDoorState:"<<bDoorState<<"bBoxState:"<<bBoxState;
}

void Sequence::CameraView(QImage img)
{
    imageProvider->img = img;
    emit callQmlRefeshView();

    if (bAutoFocus){
        qDebug()<<"JumpStep:"<<autoFocus_JumpStep<<"CurrPoint:"<<autoFocus_CurrPoint<<"ClarityValue:"<<autoFocus_ClarityValue<<"ClarityPoint:"<<autoFocus_ClarityPoint;
        if (autoFocus_JumpStep == 2){
            if (autoFocus_CurrPoint > 0)
            {
                double value = getDefinition();
                qDebug()<<"currValue:"<<value;
                if (value > autoFocus_ClarityValue)
                {
                    autoFocus_ClarityValue = value;
                    autoFocus_ClarityPoint = autoFocus_CurrPoint;
                }

                if (autoFocus_CurrPoint - autoFocus_ClarityPoint > AUTOFOCUS_MAXOFFSET || autoFocus_CurrPoint > (AUTOFOCUS_MAX-AUTOFOCUS_MIN)/AUTOFOCUS_STEP){
                    if (autoFocus_dec == false && autoFocus_ClarityPoint == 1){
                        autoFocus_dec = true;
                        autoFocus_CurrPoint = 0;
                        autoFocus_ClarityValue = 0;
                    }
                    else{
                        if (autoFocus_dec == true){
                            actionDo("Focus",2,AUTOFOCUS_MAX-AUTOFOCUS_STEP*(autoFocus_ClarityPoint-1),0,0);
                            emit autoFocusNotify(0,AUTOFOCUS_MAX-AUTOFOCUS_STEP*(autoFocus_ClarityPoint-1)-AUTOFOCUS_MIN);
                        }
                        else {
                            actionDo("Focus",2,AUTOFOCUS_STEP*(autoFocus_ClarityPoint-1)+AUTOFOCUS_MIN,0,0);
                            emit autoFocusNotify(1,AUTOFOCUS_STEP*(autoFocus_ClarityPoint-1));
                        }
                        bAutoFocus = false;
                        return;
                    }
                }
            }

            autoFocus_CurrPoint++;
            autoFocus_JumpStep = 0;
            if (autoFocus_dec == true){
                actionDo("Focus",2,AUTOFOCUS_MAX-AUTOFOCUS_STEP*(autoFocus_CurrPoint-1),0,0);
                emit autoFocusNotify(0,AUTOFOCUS_MAX-AUTOFOCUS_STEP*(autoFocus_CurrPoint-1)-AUTOFOCUS_MIN);
            }
            else{
                actionDo("Focus",2,AUTOFOCUS_STEP*(autoFocus_CurrPoint-1)+AUTOFOCUS_MIN,0,0);
                emit autoFocusNotify(0,AUTOFOCUS_STEP*(autoFocus_CurrPoint-1));
            }
        }
        else if(autoFocus_JumpStep == 1)
            autoFocus_JumpStep = 2;
    }
}

bool Sequence::startView(int id){
    if (id == 0){        
        if (imageCapture->waitStop())
        {
            actionDo("Light",5,0,0,0);
            imageCapture->imagetype = 0;
            imageCapture->start_capturing(ImageCapture::CaptureMode::View);
            imageCapture->setImageCount(99999);
            imageCapture->start();
        }
    }
    return true;
}

bool Sequence::stopView(){
    imageCapture->stop_capturing();
    camera->CameraStop();
    return true;
}

bool Sequence::saveView(){
    QDir dir(QCoreApplication::applicationDirPath());
    QStringList filter;
    filter<<QString("tmp*.bmp");
    dir.setNameFilters(filter);
    QFileInfoList fileList = dir.entryInfoList();
    char filenames[100] = {0};
    sprintf(filenames,"%s/tmp%d.bmp",QCoreApplication::applicationDirPath().toLatin1().data(),fileList.count()+1);
    imageProvider->img.save(filenames);
    return true;
}

void Sequence::errReceive(ERROR_CODE code){
    QString errStr;
    if (code == ERROR_CODE_TEMP_CONNECT){
        errStr = "温控板通讯出错！";        
    }
    else if(code == ERROR_CODE_CTRL_CONNECT)
        errStr = "驱动板通讯出错！";
    else if(code >= ERROR_CODE_CAM_CONNECT && code < ERROR_CODE_FILE_READ)
        errStr = "摄像头通讯出错！";
    else if(code == ERROR_CODE_FILE_READ)
        errStr = "配置文件读取出错！";
    else if(code == ERROR_CODE_FILE_FORMAT)
        errStr = "配置文件格式出错！";
    else if(code == ERROR_CODE_FILE_MODEL)
        errStr = "模板文件读取出错！";
    emit errOccur(errStr+" \n\n错误码："+QString::number(code,16));
}

int Sequence::getAbs(){
    return imageCapture->getabsExpose();
}

bool Sequence::setAbs(int value){
    return imageCapture->setabsExpose(value);
}

int Sequence::getGain(){
    return imageCapture->getGain();
}

bool Sequence::setGain(int value){
    return imageCapture->setGain(value);
}

int Sequence::getWhiteBalance(){
    return imageCapture->getWhite();
}

bool Sequence::setWhiteBalance(int value){
    return imageCapture->setWhite(value);
}


void Sequence::lxDebug(){
    qDebug()<<"lxDebug";
    return;
    serialMgr->serialWrite(ActionParser::ParamToByte("Light",4,0,0,0));
    cvcap->setCurrCamera(0);
    cvcap->setImageCount(3);
    cvcap->start();
}

void Sequence::showAnaImg(int type, int light){
    imageProvider->anaMainImg = imageAna->getMainImg(type,light);
    emit callQmlRefeshAnaMainImg();
}

bool Sequence::listNextAction(bool first){
    qDebug()<<"listNextAction:count="<<actList.count();
    if (!first && actList.count()>0)
        actList.removeFirst();

    if (actList.count()>0){
        action act = actList.first();
        if (act.device == "Qrcode"){
            currOrder = '\xB0';
            qr->Qr();
        }
        else if(act.device == "Pierce"){
            currOrder = '\xB1';
            qr->Pierce();
        }
        else {
            QByteArray send = ActionParser::ParamToByte(act.device,act.value,act.param1,act.param2,act.param3);
            currOrder = send[7];
            serialMgr->serialWrite(send);
        }
        return true;
    }

    return false;
}

void Sequence::qrDect(){
    actList.clear();
    action act;

    if (bQrOpenLight)
    {
        act.device = "Led";
        act.value = 62;
        actList.append(act);
    }

    act.device = "Qrcode";
    actList.append(act);

    if (bQrOpenLight)
    {
        act.device = "Led";
        act.value = 63;
        actList.append(act);
    }

    listNextAction(true);
}

void Sequence::qrSet(bool bopenlight, bool scale, bool handlimage, int bin, int pox){
    bQrOpenLight = bopenlight;
    qr->binValue = bin;
    qr->poxValue = pox;
    qr->handleimage = handlimage;
    qr->scale = scale;
}

void Sequence::PierceDect(){
    actList.clear();
    action act;

    act.device = "Led";
    act.value = 62;
    actList.append(act);

    act.device = "Pierce";
    actList.append(act);

    act.device = "Led";
    act.value = 63;
    actList.append(act);

    listNextAction(true);
}

void Sequence::fan1SetSpeed(int speed)
{
    qDebug()<<"fan1SetSpeed:"<<speed<<"prespeed:"<<t_fan1Speed;
    t_fan1Speed = speed;
    emit fan1SpeedChanged();
}

void Sequence::autoFocus(){    
    if (imageCapture->isRunning() && bAutoFocus == false){
        actionDo("Focus",1,0,0,0);
        bAutoFocus = true;
        autoFocus_CurrPoint = 0;
        autoFocus_ClarityValue = 0;
        autoFocus_JumpStep = 0;
        autoFocus_dec = false;
    }
}

bool Sequence::printTest(){
    if (printer->isRunning())
        return false;
    printer->testTime = ExGlobal::pTestModel->getCurrTestDateTime();
    printer->sampCode = ExGlobal::pTestModel->getCurrTestCode();
    printer->sampInfo = ExGlobal::pTestModel->getCurrTestInfo();
    printer->user = ExGlobal::pTestModel->getCurrTestUser();
    printer->itemMap.clear();
    QList<int> item = ExGlobal::getBoxItemList();
    int testID = ExGlobal::pTestResultModel->getTestid();
    //qDebug()<<"printTest:"<<item.length()<<","<<testID;
    for(int i = 0; i < item.length(); i++){
        printer->itemMap[ExGlobal::getPosName(item[i])] = ExGlobal::getItemResult(testID,item[i]);
        //qDebug()<<ExGlobal::getPosName(item[i])<<","<<ExGlobal::getItemResult(testID,item[i]);
    }
    printer->start();
    return true;
}

void Sequence::changeTitle(QString title){
    if (!isTesting() && !isLoopTesting())
        emit titleNotify(0, title);
}

bool Sequence::loopTest(QString testName, int count){
    qDebug()<<"Loop Test"<<loopTestName<<count;
    if (currSequenceId != SequenceId::Sequence_Idle)
        return false;
    currSequenceId = SequenceId::Sequence_LoopTest;
    QDomElement root = doc.documentElement();
    loopTestCount = count;
    loopTestCurrCount = 1;
    loopTestName = testName;
    for (sequenceAction = root.firstChildElement("PanelTest"); !sequenceAction.isNull(); sequenceAction = sequenceAction.nextSiblingElement("PanelTest"))
    {
        if (sequenceAction.attribute("PanelName")==testName)
        {
            testStartTime = QDateTime::currentDateTime();
            QString current_time_str = testStartTime.toString("yyyyMMdd_hhmmss");
            Log::setDir(QCoreApplication::applicationDirPath()+"/"+current_time_str);
            imageCapture->start_capturing(ImageCapture::CaptureMode::Capture);
            camera->CameraStart(CameraPlayer::CaptureMode::Capture);
            ExGlobal::setPanelName(sequenceAction.attribute("PanelName"));
            //ExGlobal::setReagentBox("202");
            imageAna->SetMask(ExGlobal::getReagentBox(ExGlobal::reagentBox()),0);

            if (!FormatAction())
                return false;
            testMgr->LoopTestCreate(sequenceAction.attribute("PanelCode"),loopTestCount);
            bFinishAction = false;
            durationState = TimeState::idle;
            bCannelSequence = false;
            Log::LogByFile("LoopTest.txt",QString("开始 %1，循环数：%2").arg(sequenceAction.attribute("PanelName")).arg(count));
            FindAction(false);            
            return true;
        }
    }

    return false;
}

bool Sequence::continueLoopTest(){
    Log::LogByFile("LoopTest.txt",QString("完成循环数：%1").arg(loopTestCurrCount));
    testMgr->LoopTestFinishCycle(loopTestCurrCount);
    if (loopTestCurrCount >= loopTestCount)
        return false;
    FormatAction();
    bFinishAction = false;
    durationState = TimeState::idle;
    bCannelSequence = false;
    FindAction(false);
    loopTestCurrCount++;
    return true;
}

QStringList Sequence::getLoopTestList(){
    QStringList test;
    QDomElement root = doc.documentElement();
    for (QDomElement e = root.firstChildElement("PanelTest"); !e.isNull(); e = e.nextSiblingElement("PanelTest"))
    {
        QString panelCode = e.attribute("PanelCode");
        if (panelCode.length() > 4 && panelCode.startsWith("90"))
            test<<e.attribute("PanelName");
    }
    return test;
}
