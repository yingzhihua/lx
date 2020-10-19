#include "globalapplication.h"
#include <QKeyEvent>
#include <QDebug>
GlobalApplication::GlobalApplication(int &argc, char **argv):QApplication (argc,argv)
{
    timerid = -1;
}

GlobalApplication::~GlobalApplication(){

}
void GlobalApplication::setWindowInstance(QWidget *wnd){
    widget = wnd;
}
//static bool keyvalid = true;
static QString sampleCode = "";
static QString simpleKey = "";
static time_t timel = 0;
bool GlobalApplication::notify(QObject *obj, QEvent *e){
    const QMetaObject* objMeta = obj->metaObject();
    QString clName = objMeta->className();
    //qDebug()<<"e.type:"<<e->type();
    if(e->type() == QEvent::MouseButtonRelease){
        //qDebug()<<"clName:"<<clName.mid(0,28);
        if (clName.mid(0,28) == "QQuickApplicationWindow_QML_"){
            //QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);
            //qDebug()<<"clName:"<<clName<<"mouse:"<<mouseEvent->x();
            if (timerid != -1)
                this->killTimer(timerid);
            int timerout = ExGlobal::lockscreen_time;
            if (timerout != 0)
                timerid = this->startTimer(timerout*1000);
        }
    }
    else if(e->type() == QEvent::KeyPress){
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
        //qDebug()<<keyEvent->key()<<keyEvent->text()<<keyEvent->count();
        if (keyEvent->text() != "")
            simpleKey = keyEvent->text();
    }
    else if(e->type() == QEvent::KeyRelease){
        if (simpleKey != "")
        {            
            if (timel != 0)
            {
                time_t tmpt = timel;
                timel = clock();
                if (timel-tmpt > 30000)
                    sampleCode = "";
            }
            else
                timel = clock();
            sampleCode += simpleKey;
            simpleKey = "";
        }
        else if (sampleCode != "")
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
            if (keyEvent->key() == Qt::Key_Return)
            {
                if (sampleCode.length() > 3)
                    global->setSampleCode(sampleCode);
                qDebug()<<sampleCode;
                sampleCode = "";
            }
        }

    }

    return QApplication::notify(obj,e);
}

void GlobalApplication::timerEvent(QTimerEvent *event){
    qDebug()<<"timerEvent:"<<event->timerId();
    this->killTimer(event->timerId());
    if (event->timerId() == timerid)
    {
        timerid = -1;
        global->GlobalMessage(1);
    }
    else if(event->timerId() == timerkey)
    {
        timerkey = -1;
        sampleCode = "";
    }
}
