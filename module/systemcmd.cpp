#include "systemcmd.h"

SystemCmd::SystemCmd(QObject *parent) : QThread(parent){

}

void SystemCmd::execute(QString cmd){
    while(this->isRunning());
    cmdStr = cmd;
    this->start();
}

void SystemCmd::run(){
    system(cmdStr.toLatin1().data());
}
