#include "sequence.h"
#include <QDebug>

#include "exglobal.h"
#include "actionparser.h"
#include "log.h"
#include "imageanalysis.h"
#include "sqlitemgr.h"

static QDomDocument doc;
static char currOrder = 0;
static int currCameraCaptureType = 0;
static int currCameraCycle = 0;
static QDateTime testStartTime;
Sequence::Sequence(QObject *parent) : QObject(parent)
{    
    imageAna = new ImageAnalysis();
    timer = new QTimer(this);
    timer->setSingleShot(true);
    waitNextSequence = new QTimer(this);
    waitNextSequence->setSingleShot(true);
    connect(timer,SIGNAL(timeout()),this,SLOT(SequenceTimeout()));
    connect(waitNextSequence,SIGNAL(timeout()),this,SLOT(WaitSequenceTimeout()));

    serialMgr = new SerialMgr();
    connect(serialMgr,&SerialMgr::finishAction,this,&Sequence::ActionFinish);
    connect(serialMgr,&SerialMgr::errOccur,this,&Sequence::errReceive);

    imageCapture = new ImageCapture();
    imageProvider = new ImageProvider();
    connect(imageCapture,&ImageCapture::finishCapture,this,&Sequence::ActionFinish);
    connect(imageCapture,&ImageCapture::reView,this,&Sequence::CameraView);

    testMgr = new TestMgr();

    qr = new QRcoder();
    connect(qr,&QRcoder::finishQRcode,this,&Sequence::ActionFinish);

    currSequenceId = SequenceId::Sequence_Idle;    
    bCannelSequence = false;

    //actionDo("Sensor",0,0,0);
    serialMgr->serialWrite(ActionParser::ParamToByte("AutoData",1,0,0,0));
    //ReadMask(QCoreApplication::applicationDirPath()+"/pos");
    //imageAna->SetMask(ExGlobal::getReagentBox("201"),0);

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
    if (currSequenceId != SequenceId::Sequence_Idle)
    {
        this->nextSequenceId = id;
        waitCount = 0;
        waitNextSequence->start(1000);
        return false;
    }
    this->currSequenceId = id;
    QDomElement root = doc.documentElement();
    qDebug()<<"start sequence:"<<id;
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
                ExGlobal::setPanelName(sequenceAction.attribute("PanelName"));

                ExGlobal::setReagentBox("202");
                imageAna->SetMask(ExGlobal::getReagentBox("202"),0);
                testMgr->TestCreate("12345",ExGlobal::reagentBox());
                break;
            }
        }
    }
    else if(id == SequenceId::Sequence_OpenBox)
    {
        sequenceAction = root.firstChildElement("OpenDoor");
    }
    else if(id == SequenceId::Sequence_CloseBox){
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
    if (currSequenceId != SequenceId::Sequence_Test)
        return;
    imageCapture->stop_capturing();

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
    if (waitCount < 5)
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
        if (currSequenceId != SequenceId::Sequence_Test){
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
            if (data[7] == '\x62')
                ExGlobal::settempversion(data.mid(13,5).data());
        }
        else if (data[1] == '\x01')
        {
            if (data[7] == '\x62')
                ExGlobal::setctrlversion(data.mid(13,5).data());
            else if (data[7] == '\x32')
                setSenorState(data[12],data[13]);
        }
        else if(data[1] == '\x03')//CameraCapture
        {
            qDebug()<<"CameraCapture ActionFinish,type="<<currCameraCaptureType;
            if (data[7] == '\xa0' && data[10] == '\x00' && currCameraCaptureType == 5){
                if (currCameraCycle < 2)                
                    imageAna->FirstImage(imageCapture->getyData(),0);                
                else                
                    imageAna->AddImage(imageCapture->getyData(),0);

                imageProvider->anaMainImg = imageAna->getMainImg(0,1);
                emit callQmlRefeshAnaMainImg();
                QVector<int> item = imageAna->getItem();
                QVector<int> value = imageAna->getValue();
                QVector<int> posIndex = imageAna->getIndex();
                emit callQmlRefeshData(currCameraCycle,item,value);

                sqlitemgrinstance->StartTransations();
                for(int i = 0; i < item.size(); i++){
                    testMgr->InsertData(posIndex[i],item[i],currCameraCycle,value[i]);
                }
                sqlitemgrinstance->EndTransations();

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
                errReceive(0x300 + data[10]);
                return;
            }

            currCameraCaptureType = 0;
        }        
    }
    else if (data[1] == '\x04'){
        if (data[2] == '\x01')
            emit callQmlRefeshQrImg();
        data.remove(0,10);
        emit qrDecode(data.data());
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
    else if (currSequenceId == SequenceId::Sequence_QrDecode){
        if (!listNextAction(false))
        {
            currSequenceId = SequenceId::Sequence_Idle;
        }
    }
    else{
        bFinishAction = true;
        //qDebug()<<"Sequence ActionFinish,durationState:"<<durationState;
        if (durationState == TimeState::idle)
            FindAction(true);
        else if(durationState == TimeState::wait)
        {
            durationState = TimeState::running;
            timer->start(nDuration);
        }
    }
}

void Sequence::FinishSequence()
{
    qDebug()<<"FinishSequence:"<<currSequenceId;
    if (currSequenceId == SequenceId::Sequence_SelfCheck)
    {
        emit sequenceFinish(SequenceResult::Result_SelfCheck_ok);
    }
    else if(currSequenceId == SequenceId::Sequence_CloseBox)
    {
        bDoorState = false;
        emit sequenceFinish(SequenceResult::Result_CloseBox_ok);
    }
    else if(currSequenceId == SequenceId::Sequence_OpenBox)
    {
        bDoorState = true;
        emit sequenceFinish(SequenceResult::Result_OpenBox_ok);
    }
    else if(currSequenceId == SequenceId::Sequence_CannelTest)
        emit sequenceFinish(SequenceResult::Result_CannelTest_ok);
    else if(currSequenceId == SequenceId::Sequence_Test)
    {
        imageCapture->stop_capturing();
        if (imageAna->getItem().size() > 45)
            testMgr->TestClose(2);
        else
            testMgr->TestClose(1);
        ExGlobal::addTest();
        Log::setDir(QCoreApplication::applicationDirPath());
    }
    currSequenceId = SequenceId::Sequence_Idle;
}

bool Sequence::ReadTestProcess(QString panel)
{
    QFile xmlfile(panel+".xml");
    qDebug("ReadTestProcess");
    if (!xmlfile.open(QFile::ReadOnly))
    {
        errReceive(0x401);
        qDebug()<<"file err:"<<xmlfile.error()<<",string:"<<xmlfile.errorString()<<",file:"<<xmlfile.fileName()<<",dir:"<<QDir::currentPath()<<",apppath:"<<QCoreApplication::applicationDirPath();
        return false;
    }

    if (!doc.setContent(&xmlfile))
    {
        xmlfile.close();
        qDebug()<<"xml setContent err return";
        errReceive(0x402);
        return  false;
    }
    xmlfile.close();

    QDomElement root = doc.documentElement();    
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
        errReceive(0x403);
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

    bFinishAction = false;    

    if (action.attribute("Device")=="CameraCapture")
    {
        currCameraCaptureType = action.attribute("ActionValue").toInt();
        currCameraCycle = 0;
        if(isChild){
            QDomElement p = action.parentNode().toElement();
            int cycle = p.attribute("currcycle").toInt();
            currCameraCycle = cycle;
            if (cycle < 10)
            {
                if (action.attribute("ActionValue") == "4")
                    imageCapture->setFileName("cycleb0"+QString::number(cycle));
                else
                    imageCapture->setFileName("cycle0"+QString::number(cycle));
            }
            else
            {
                if (action.attribute("ActionValue") == "4")
                    imageCapture->setFileName("cycleb"+QString::number(cycle));
                else
                    imageCapture->setFileName("cycle"+QString::number(cycle));
            }
        }
        else if (action.attribute("ActionValue") == "1")
        {
            imageCapture->setFileName("Dry");
        }
        else if (action.attribute("ActionValue") == "2")
        {
            imageCapture->setFileName("Black");
        }
        else if (action.attribute("ActionValue") == "3")
            imageCapture->setFileName("Fill");

        currOrder = '\xA0';
        imageCapture->setImageCount(action.attribute("ActionParam1").toInt());
        if (!imageCapture->Running())
            imageCapture->start();
    }
    else if(action.attribute("Device")=="Door" || action.attribute("Device")=="Light" || action.attribute("Device")=="Temperature"
            ||action.attribute("Device")=="V1" || action.attribute("Device")=="V2" || action.attribute("Device")=="V3"
            ||action.attribute("Device")=="VP" || action.attribute("Device")=="Pump" || action.attribute("Device")=="Query"
            ||action.attribute("Device")=="SetPID")
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

    emit processFinish(sequenceAction.attribute("Duration").toInt(),nPreTime);

    durationState = TimeState::running;
    if (nDuration == 0)
        SequenceTimeout();
    else if(action.attribute("DurationType")=="1")
        timer->start(nDuration);
    else
        durationState = TimeState::wait;

    return true;
}

QString Sequence::sequenceMessage(){    
    return message;
}
bool Sequence::FindAction(bool bFinishAction)
{    
    int totalStep = sequenceAction.attribute("steps").toInt();
    int currStep = sequenceAction.attribute("currstep").toInt();
    int nTotalTime = sequenceAction.attribute("Duration").toInt();

    QDomElement e = sequenceAction.firstChildElement();
    while (!e.isNull()) {
        if (e.hasAttribute("No") && e.attribute("No").toInt() == currStep){
            if (e.tagName() == "Action"){
                if (!bFinishAction)
                    return DoAction(e,false);
                else if(currStep == totalStep)
                {
                    emit processFinish(nTotalTime,nTotalTime);
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
                                        emit processFinish(nTotalTime,nTotalTime);
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

void Sequence::setSenorState(char char1, char char2)
{
    bool sensor;
    //qDebug()<<"setSenorState,char1:"<<char1<<",char2:"<<char2;
    if (char1&0x08)
        sensor = true;
    else
        sensor = false;
    if (sensor != bBoxState)
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
    //qDebug()<<"bDoorState:"<<bDoorState<<"bBoxState:"<<bBoxState;
}

void Sequence::CameraView(QImage img)
{
    imageProvider->img = img;
    emit callQmlRefeshView();
}

bool Sequence::startView(int id){
    if (id == 0){        
        if (!imageCapture->Running())
        {
            imageCapture->start_capturing(ImageCapture::CaptureMode::View);
            imageCapture->setImageCount(10);
            imageCapture->start();
        }
    }
    return true;
}

bool Sequence::stopView(){
    imageCapture->stop_capturing();
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

void Sequence::errReceive(int code){
    QString errStr;
    if (code == 0x101)
        errStr = "温控板通讯出错！";
    else if(code == 0x201)
        errStr = "驱动板通讯出错！";
    else if(code >= 0x301 && code <= 0x3FF)
        errStr = "摄像头通讯出错！";
    else if(code == 0x401)
        errStr = "配置文件读取出错！";
    else if(code == 0x402)
        errStr = "配置文件格式出错！";
    else if(code == 0x403)
        errStr = "模板文件读取出错！";
    emit errOccur(errStr+" 错误代码："+QString::number(code,16));
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
    //ImageAnalysis::QRDecode(QCoreApplication::applicationDirPath()+"/codebar/Cycle00.tif");

    //QRcoder::QRDecode();
    //emit callQmlRefeshData(data);
    //qr->saveFrame();
    //ExGlobal::addTest();
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
            while (qr->isRunning());
            qr->start();
        }else {
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
