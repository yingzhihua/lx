#ifndef GLOBALAPPLICATION_H
#define GLOBALAPPLICATION_H
#include <QApplication>
#include "module/exglobal.h"
#include <QTimer>
class GlobalApplication : public QApplication
{
public:
    GlobalApplication(int &argc,char **argv);
    ~GlobalApplication();
    bool notify(QObject*,QEvent *);
    void setWindowInstance(QWidget *wnd);
    ExGlobal *global;
private:
    QWidget *widget;
    int timerid;

public slots:
    void timerTimeout();
protected:
    void timerEvent(QTimerEvent *event);
};

#endif // GLOBALAPPLICATION_H