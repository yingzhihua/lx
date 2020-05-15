#include "actionparser.h"
#include "exglobal.h"
#include <QDebug>
static quint8 nserial = 0;
ActionParser::ActionParser()
{

}
QByteArray ActionParser::DrvParamToByte(const QString &action, int value, int param1, int param2, int param3)
{
    QByteArray data;

    if (action == "Query"){
        data.resize(12);
        data[6] = 0x00;
        data[7] = 0x32;
        data[8] = 0x00;
        data[9] = 0x01;
        data[10] = 0x01;    //保留
        if (value == 1)
        {
            data[7] = 0x62;
            FormatBytes(0x02,data);
        }
        else if(value == 2)
        {
            data[7] = 0x62;
            FormatBytes(0x01,data);
        }
        else if(value == 3) //query sensor status
        {
            data[7] = 0x32;
            FormatBytes(0x01,data);
        }
    }
    else if(action == "SetPID"){
        data.resize(19);
        data[6] = 0x00;
        data[7] = 0x75;
        data[8] = 0x00;
        data[9] = 0x08;
        data[10] = 0x01;    //保留
        data[12] = ((param1*10)>>8)&0xFF;
        data[13] = (param1*10)&0xFF;
        data[14] = (param2>>8)&0xFF;
        data[15] = param2&0xFF;
        data[16] = (param3>>8)&0xFF;
        data[17] = param3&0xFF;
        if (value == 1)
        {
            data[11] = 0x01;
            FormatBytes(0x02,data);
        }
        else if(value == 2)
        {
            data[11] = 0x00;
            FormatBytes(0x02,data);
        }
        else if(value == 3) //query sensor status
        {
            data[11] = 0x03;
            FormatBytes(0x02,data);
        }
    }
    else if(action == "AutoData"){
        data.resize(13);
        data[6] = 0x00;
        data[7] = 0x28;
        data[8] = 0x00;
        data[9] = 0x02;
        data[10] = 0x01;    //保留
        if (value == 1)
            data[11] = 0x01;    //01:open 00:close
        else
            data[11] = 0x00;

        FormatBytes(0x02,data);
    }
    else if(action == "Focus"){
        if (value == 1)
        {
            data.resize(13);
            data[9] = 0x02;
            data[11] = 0x2a;
        }
        else if(value == 2)
        {
            data.resize(15);
            data[9] = 0x04;
            data[11] = 0x2b;
            data[12] = (param1>>8)&0xFF;    //focus step
            data[13] = param1&0xFF;
        }
        data[6] = 0x00;
        data[7] = 0x70;
        data[8] = 0x00;
        data[10] = 0x01;
        FormatBytes(0x01,data);
    }
    else if(action == "Led"){
        data.resize(13);
        data[6] = 0x00;
        data[7] = 0x70;
        data[8] = 0x00;
        data[9] = 0x02;
        data[10] = 0x01;    //保留
        data[11] = value&0xFF;
        FormatBytes(0x01,data);
    }
    else if(action == "Fan"){
        data[6] = 0x00;
        if (value == 1){    //query fan's speed
            data.resize(13);
            data[7] = 0x20;
            data[8] = 0x00;
            data[9] = 0x02;
            data[10] = 0x01;
            data[11] = param1-1;
        }
        else if(value == 2) //set fan's speed
        {
            data.resize(15);
            data[7] = 0x22;
            data[8] = 0x00;
            data[9] = 0x04;
            data[10] = 0x01;
            data[11] = param1-1;
            data[12] = (param2>>8)&0xFF;
            data[13] = param2&0xFF;
        }
        else if(value == 3) //query actual fan's speed
        {
            data.resize(13);
            data[7] = 0x1F;
            data[8] = 0x00;
            data[9] = 0x02;
            data[10] = 0x01;
            data[11] = param1-1;
        }

        FormatBytes(0x02,data);
    }

    return data;
}

QByteArray ActionParser::ParamToByte(const QString &action, int value, int param1, int param2, int param3)
{
    QByteArray data;

    if (action == "Door"){        
        if (value==1)       //1:出仓；2：进仓；3：正方向移动；4：负方向移动;5:复位
        {
            data.resize(15);
            data[6] = 0x00;
            data[7] = 0x70;
            data[8] = 0x00;
            data[9] = 0x04;
            data[10] = 0x01;    //保留
            data[11] = 0x02;    //时序编号 02：出仓；03：进仓；0x48：正方向移动；0x49：负方向移动（包含速度）
            data[12] = (param1>>8)&0xFF;
            data[13] = param1&0xFF;
        }
        else if(value == 2)
        {
            int pumpSoftHomeX = ExGlobal::PumpSoftHomeX;
            data.resize(17);
            data[6] = 0x00;
            data[7] = 0x70;
            data[8] = 0x00;
            data[9] = 0x04;
            data[10] = 0x01;    //保留
            data[11] = 0x06;
            data[12] = (ExGlobal::VDWorkX>>8)&0xFF;    //时序参数
            data[13] = ExGlobal::VDWorkX&0xFF;
            if (pumpSoftHomeX < 0)
            {
                pumpSoftHomeX = -pumpSoftHomeX;
                data[11] = 0x03;
            }
            data[14] = (pumpSoftHomeX>>8)&0xFF;
            data[15] = pumpSoftHomeX&0xFF;
        }
        else if(value == 3){
            data.resize(17);
            data[6] = 0x00;
            data[7] = 0x70;
            data[8] = 0x00;
            data[9] = 0x06;
            data[10] = 0x01;    //保留            
            data[11] = 0x48;
            data[12] = (param1>>8)&0xFF;    //时序参数
            data[13] = param1&0xFF;
            data[14] = (param2>>8)&0xFF;
            data[15] = (param2)&0xFF;
        }
        else if(value == 4){
            data.resize(17);
            data[6] = 0x00;
            data[7] = 0x70;
            data[8] = 0x00;
            data[9] = 0x04;
            data[10] = 0x01;    //保留
            data[9] = 0x06;
            data[11] = 0x49;
            data[12] = (param1>>8)&0xFF;    //时序参数
            data[13] = param1&0xFF;
            data[14] = (param2>>8)&0xFF;
            data[15] = (param2)&0xFF;
        }
        else if(value == 5){
            data.resize(13);
            data[6] = 0x00;
            data[7] = 0x70;
            data[8] = 0x00;
            data[9] = 0x02;
            data[10] = 0x01;    //保留
            data[11] = 0x28;
        }
        else if(value == 9){
            data.resize(15);
            data[6] = 0x00;
            data[7] = 0x70;
            data[8] = 0x00;
            data[9] = 0x04;
            data[10] = 0x01;    //保留
            data[11] = 0x03;
            data[12] = (param1>>8)&0xFF;    //时序参数
            data[13] = param1&0xFF;
        }

        FormatBytes(0x01,data);
    }
    else if(action == "Light")
    {
        data.resize(13);
        data[6] = 0x00;
        data[7] = 0x70;
        data[8] = 0x00;
        data[9] = 0x02;
        data[10] = 0x01;    //保留

        data[11] = 0x0B;    //0B-0E:1-4档光源,0A:close
        if (value <= 5 && value >= 1)
            data[11] = 0x0A + value - 1;

        FormatBytes(0x01,data);
    }
    else if(action=="Temperature")
    {
        data.resize(16);
        data[6] = 0x00;
        data[7] = 0x24;
        data[8] = 0x00;
        data[9] = 0x05;
        data[10] = 0x00;
        data[11] = 0x00;    //temp ctrl index, 0:behind,1:front
        if (value == 1)
            data[11] = 0x01;
        else if(value == 2)
            data[11] = 0x00;
        else if(value == 3)
            data[11] = 0x10;
        else if(value == 4)
            data[11] = 0x03;

        if (param1 == 0)
            data[12] = 0x00;
        else
            data[12] = 0x01;    //open temp ctrl
        data[13] = (param1>>8)&0xFF;
        data[14] = param1&0xFF;
        FormatBytes(0x02,data);
    }
    else if(action=="V1")
    {
        bool reset = false;
        bool hasspeed = false;
        if (value == 1)
            reset = true;
        if (reset)
        {
            data.resize(13);
            data[9] = 2;
            data[11] = 0x1E;
        }
        else if(hasspeed){
            data.resize(17);
            data[9] = 6;
            if (param2<0)
            {
                data[11] = 75;
                param2 = -param2;
            }
            else {
                data[11] = 74;
            }
            data[12] = (param1>>8)&0xFF;
            data[13] = param1&0xFF;
            data[14] = (param2>>8)&0xFF;
            data[15] = param2&0xFF;
        }
        else{
            data.resize(15);
            data[9] = 4;
            data[11] = 0x1F;
            if (value == 1)
            {

            }
            else if(value == 2){
                data[12] = (ExGlobal::V1SoftHomeX>>8)&0xFF;
                data[13] = ExGlobal::V1SoftHomeX&0xFF;
            }
            else if (value == 3)
            {
                data[12] = (ExGlobal::V1WorkX>>8)&0xFF;
                data[13] = ExGlobal::V1WorkX&0xFF;
            }
            else if(value == 9)
            {
                data[12] = (param1>>8)&0xFF;
                data[13] = param1&0xFF;
            }
        }

        data[6] = 0x00;
        data[7] = 0x70;
        data[8] = 0x00;
        data[10] = 0x01;

        FormatBytes(0x01,data);
    }
    else if(action=="V2")
    {
        bool reset = false;
        bool hasspeed = false;
        if (value == 1)
            reset = true;
        if (reset)
        {
            data.resize(13);
            data[9] = 2;
            data[11] = 0x20;
        }
        else if(hasspeed){
            data.resize(17);
            data[9] = 6;
            if (param2<0)
            {
                data[11] = 77;
                param2 = -param2;
            }
            else {
                data[11] = 76;
            }
            data[12] = (param1>>8)&0xFF;
            data[13] = param1&0xFF;
            data[14] = (param2>>8)&0xFF;
            data[15] = param2&0xFF;
        }
        else{
            data.resize(15);
            data[9] = 4;
            data[11] = 0x21;
            if (value == 1)
            {

            }
            else if(value == 2){
                data[12] = (ExGlobal::V2SoftHomeX>>8)&0xFF;
                data[13] = ExGlobal::V2SoftHomeX&0xFF;
            }
            else if (value == 3)
            {
                data[12] = (ExGlobal::V2WorkX>>8)&0xFF;
                data[13] = ExGlobal::V2WorkX&0xFF;
            }
            else if(value == 9)
            {
                data[12] = (param1>>8)&0xFF;
                data[13] = param1&0xFF;
            }
        }

        data[6] = 0x00;
        data[7] = 0x70;
        data[8] = 0x00;
        data[10] = 0x01;

        FormatBytes(0x01,data);
    }
    else if(action=="V3")
    {
        bool reset = false;
        bool hasspeed = false;
        if (value == 1)
            reset = true;
        if (reset)
        {
            data.resize(13);
            data[9] = 2;
            data[11] = 0x22;
        }
        else if(hasspeed){
            data.resize(17);
            data[9] = 6;
            if (param2<0)
            {
                data[11] = 79;
                param2 = -param2;
            }
            else {
                data[11] = 78;
            }
            data[12] = (param1>>8)&0xFF;
            data[13] = param1&0xFF;
            data[14] = (param2>>8)&0xFF;
            data[15] = param2&0xFF;
        }
        else{
            data.resize(15);
            data[9] = 4;
            data[11] = 0x23;
            if (value == 1)
            {

            }
            else if(value == 2){
                data[12] = (ExGlobal::V3SoftHomeX>>8)&0xFF;
                data[13] = ExGlobal::V3SoftHomeX&0xFF;
            }
            else if (value == 3)
            {
                data[12] = (ExGlobal::V3WorkX>>8)&0xFF;
                data[13] = ExGlobal::V3WorkX&0xFF;
            }
            else if(value == 9)
            {
                data[12] = (param1>>8)&0xFF;
                data[13] = param1&0xFF;
            }
        }

        data[6] = 0x00;
        data[7] = 0x70;
        data[8] = 0x00;
        data[10] = 0x01;

        FormatBytes(0x01,data);
    }
    else if(action=="V123")
    {
        if (value == 1)
        {
            data.resize(13);
            data[9] = 2;
            data[11] = 0x09;
        }
        else if(value == 2)
        {
            data.resize(21);
            data[9] = 8;
            data[11] = 0x08;

            data[12] = 0x00;
            data[13] = 0x08;
            data[14] = (ExGlobal::V1SoftHomeX>>8)&0xFF;
            data[15] = ExGlobal::V1SoftHomeX&0xFF;
            data[16] = (ExGlobal::V2SoftHomeX>>8)&0xFF;
            data[17] = ExGlobal::V2SoftHomeX&0xFF;
            data[18] = (ExGlobal::V3SoftHomeX>>8)&0xFF;
            data[19] = ExGlobal::V3SoftHomeX&0xFF;
        }
        else if(value == 3)
        {
            data.resize(21);
            data[9] = 8;
            data[11] = 0x08;

            data[12] = 0x00;
            data[13] = 0x08;
            data[14] = (ExGlobal::V1WorkX>>8)&0xFF;
            data[15] = ExGlobal::V1WorkX&0xFF;
            data[16] = (ExGlobal::V2WorkX>>8)&0xFF;
            data[17] = ExGlobal::V2WorkX&0xFF;
            data[18] = (ExGlobal::V3WorkX>>8)&0xFF;
            data[19] = ExGlobal::V3WorkX&0xFF;
        }

        data[6] = 0x00;
        data[7] = 0x70;
        data[8] = 0x00;
        data[10] = 0x01;

        FormatBytes(0x01,data);
    }
    else if(action=="VP")
    {
        bool reset = false;
        bool hasspeed = false;
        if (value == 1)
            reset = true;
        if (reset)
        {
            data.resize(13);
            data[9] = 2;
            data[11] = 0x24;
        }
        else if(hasspeed){
            data.resize(17);
            data[9] = 6;
            if (param2<0)
            {
                data[11] = 81;
                param2 = -param2;
            }
            else {
                data[11] = 80;
            }
            data[12] = (param1>>8)&0xFF;
            data[13] = param1&0xFF;
            data[14] = (param2>>8)&0xFF;
            data[15] = param2&0xFF;
        }
        else{
            data.resize(15);
            data[9] = 4;
            data[11] = 0x25;
            if (value == 1)
            {

            }
            else if(value == 2){
                data[12] = (ExGlobal::VPSoftHomeX>>8)&0xFF;
                data[13] = ExGlobal::VPSoftHomeX&0xFF;
            }
            else if (value == 3)
            {
                data[12] = (ExGlobal::VPWorkX>>8)&0xFF;
                data[13] = ExGlobal::VPWorkX&0xFF;
            }
            else if(value == 9)
            {
                data[12] = (param1>>8)&0xFF;
                data[13] = param1&0xFF;
            }
        }

        data[6] = 0x00;
        data[7] = 0x70;
        data[8] = 0x00;
        data[10] = 0x01;

        FormatBytes(0x01,data);
    }
    else if(action == "Pump")
    {
        if (value == 1)//hardware reset
        {
            data.resize(13);
            data[9] = 0x02;
            data[11] = 0x1B;
        }
        else if (value == 2 || value == 3 ||value == 9)//software reset or
        {
            data.resize(15);
            data[9] = 4;
            if (value == 2)
                param1 = ExGlobal::PumpSoftHomeX;
            else if(value == 3)
                param1 = ExGlobal::PumpToolHomeX;

            if (param1 >= 0)
            {
                data[11] = 0x1a;
            }
            else {
                data[11] = 0x19;
                param1 = -param1;
            }
            data[12] = (param1>>8)&0xFF;
            data[13] = param1&0xFF;
        }
        else if(value == 4 || value == 5)
        {
            double step = (double)param2;
            step = step/0.010797;
            //param2 = (int)step;
            data.resize(17);
            data[9] = 0x06;
            if (value == 4)
                data[11] = 0x1C;
            else if(value == 5)
                data[11] = 0x1D;
            data[12] = (param1>>8)&0xFF;
            data[13] = param1&0xFF;
            data[14] = (param2>>8)&0xFF;
            data[15] = param2&0xFF;
        }        

        data[6] = 0x00;
        data[7] = 0x70;
        data[8] = 0x00;
        data[10] = 0x01;    //保留

        FormatBytes(0x01,data);
    }
    else if(action == "Version")
    {
        data.resize(11);

        data[6] = 0x00;
        data[7] = 0x62;
        data[8] = 0x00;
        data[9] = 0x00;

        if (value == 1)
            FormatBytes(0x02,data);
        else
            FormatBytes(0x01,data);
    }
    else
        return DrvParamToByte(action,value,param1,param2,param3);
    return data;
}

QByteArray ActionParser::ActionToByte(const QDomElement &action)
{
    return ParamToByte(action.attribute("Device"),action.attribute("ActionValue").toInt(),action.attribute("ActionParam1").toInt(),
                       action.attribute("ActionParam2").toInt(),action.attribute("ActionParam3").toInt());
}

void ActionParser::FormatBytes(quint8 device,QByteArray &data)
{
    data[0] = 0xAA;
    data[1] = device;
    data[2] = data.size();
    data[3] = data[0];
    data[4] = nserial++;
    data[5] = 0;
    data[data.length()-1] = 0x55;
    for (int i = 1; i<data.size();i++)
    {
        if (i != 3) data[3] = data[3] ^ data[i];
    }
}
