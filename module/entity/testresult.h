#ifndef TESTRESULT_H
#define TESTRESULT_H
#include<QString>
class TestResult{
public:
    TestResult(){}
    TestResult(int Resultid_, int PosIndex_, int Itemid_, int cycle_, int TestValue_){
        Resultid = Resultid_;
        PosIndex = PosIndex_;
        Itemid = Itemid_;
        cycle = cycle_;
        TestValue = TestValue_;
    }
    int Resultid;
    int PosIndex;
    int Itemid;
    int cycle;
    int TestValue;
};
#endif // TESTRESULT_H
