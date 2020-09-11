#ifndef SYSTEMCMD_H
#define SYSTEMCMD_H

#include <QThread>

class SystemCmd : public QThread
{
    Q_OBJECT
public:    
    explicit SystemCmd(QObject *parent = nullptr);
    void execute(QString cmd);
protected:
    void run();
private:
    QString cmdStr;
};

#endif // SYSTEMCMD_H
