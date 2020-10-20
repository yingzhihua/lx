#ifndef ENTITY_H
#define ENTITY_H
#include <QString>

typedef struct Action
{
    QString device;
    int value;
    int param1;
    int param2;
    int param3;
    Action(){
        device = "";
        value = 0;
        param1 = 0;
        param2 = 0;
        param3 = 0;
    }
} action;
#endif // ENTITY_H
