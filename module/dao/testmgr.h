#ifndef TESTMGR_H
#define TESTMGR_H

#include <QObject>

class TestMgr : public QObject
{
    Q_OBJECT
public:
    explicit TestMgr(QObject *parent = nullptr);
    int TestCreate();

signals:

public slots:
};

#endif // TESTMGR_H
