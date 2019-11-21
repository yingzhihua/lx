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

    if(e->type() == QEvent::MouseButtonRelease){
        if (clName == "QQuickApplicationWindow_QML_30"){
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);
            qDebug()<<"clName:"<<clName<<"mouse:"<<mouseEvent->x();
            if (timerid != -1)
                this->killTimer(timerid);
            int timerout = ExGlobal::getCaliParam("LockScreenTime");
            if (timerout != 0)
                timerid = this->startTimer(timerout*1000);
        }
    }

    return QApplication::notify(obj,e);
}

void GlobalApplication::timerEvent(QTimerEvent *event){
    qDebug()<<"timerEvent:"<<event->timerId();
    this->killTimer(event->timerId());
    timerid = -1;
    global->GlobalMessage(1);    
}
