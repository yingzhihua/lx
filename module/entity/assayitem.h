#ifndef ASSAYITEM_H
#define ASSAYITEM_H
#include<QString>

class AssayItem{
public:
    AssayItem(){}
    AssayItem(int Itemid_, QString ItemName_){
        Itemid = Itemid_;
        ItemName = ItemName_;
    }
    int Itemid;
    QString ItemName;
};

#endif // ASSAYITEM_H
