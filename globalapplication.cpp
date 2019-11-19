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
bool GlobalApplication::notify(QObject *obj, QEvent *e){
    const QMetaObject* objMeta = obj->metaObject();
    QString clName = objMeta->className();

    if (clName == "QQuickApplicationWindow_QML_30" && ExGlobal::user() == "NotLoggedIn")
    if(e->type() == QEvent::MouseButtonPress){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);
        qDebug()<<"clName:"<<clName<<"mouse:"<<mouseEvent->x();
        if (timerid != -1)
            this->killTimer(timerid);
        int timerout = ExGlobal::getCaliParam("LockScreenTime");
        if (timerout != 0)
            timerid = this->startTimer(timerout*1000);
    }

    return QApplication::notify(obj,e);
}

void GlobalApplication::timerTimeout(){
    qDebug()<<"timerTimeout";
    global->GlobalMessage(1);
}

void GlobalApplication::timerEvent(QTimerEvent *event){
    qDebug()<<"timerEvent:"<<event->timerId();
    global->GlobalMessage(1);
}
