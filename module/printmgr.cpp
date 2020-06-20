#include "printmgr.h"
#include "exglobal.h"
#include <QCoreApplication>
#include <QPrinter>
#include <QPainter>
#include <QPrinterInfo>
#include <QVector>
#include <QLine>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QTextCodec>
#include "PrinterLibs.h"
#include "log.h"

printmgr::printmgr(QObject *parent) : QThread(parent)
{

}

void printmgr::run(){

    QByteArray res;
    res.resize(10);
    res[0] = '\xaa';
    res[1] = '\x04';
    res[2] = '\x01';
    res[7] = '\xB1';
    res[8] = '\x00';

    //printPDF();
    if (!printPTP())
        res[8] = '\x01';

    //Log::Logdb(LOGTYPE_PRINT,printok?1:0,ExGlobal::User);
    msleep(1000);
    emit finishPrint(res);
}

bool printmgr::printPDF(){
    QPrinter printer;
    QDir dir(QCoreApplication::applicationDirPath()+"/../report");
    QList<QPrinterInfo> list = QPrinterInfo::availablePrinters();
    qDebug()<<"print size="<<list.size();
    for (int i = 0; i < list.size(); i++)
        qDebug()<<list.at(i).printerName();

    if (!dir.exists())
        dir.mkpath(QCoreApplication::applicationDirPath()+"/../report");

    //printer.setOrientation(QPrinter::Landscape);
    //printer.setPageSize(QPrinter::Custom);
    //printer.setPageSizeMM(QSize(220,800));
    printer.setPageSize(QPrinter::A4);
#if 0
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(QCoreApplication::applicationDirPath()+"/../report/"+sampCode.remove(QRegExp("\\s"))+QDateTime::currentDateTime().toString("-yyyyMMdd-hhmmss")+".pdf");
#else
    if (list.size()==0)
    {
        return false;
    }

    printer.setOutputFormat(QPrinter::NativeFormat);
    printer.setPrinterName(list.at(0).printerName());
#endif
    QPainter *painter = new QPainter();
    painter->begin(&printer);
    painter->setRenderHint(QPainter::Antialiasing,true);
    painter->setPen(QPen(QColor(255,255,255),2));
    painter->setBrush(QColor(240,240,240));
    QRect rect(3,0,800,600);
    painter->fillRect(rect,QColor(255,255,255));
    painter->drawRect(rect);

    painter->setPen(QPen(QColor(0,0,0),1));
    QVector<QLine> lines;
    lines.append(QLine(QPoint(50,50),QPoint(750,50)));
    lines.append(QLine(QPoint(750,50),QPoint(750,550)));
    lines.append(QLine(QPoint(50,550),QPoint(750,550)));
    lines.append(QLine(QPoint(50,50),QPoint(50,550)));
    lines.append(QLine(QPoint(50,120),QPoint(750,120)));
    lines.append(QLine(QPoint(50,180),QPoint(750,180)));
    lines.append(QLine(QPoint(50,200),QPoint(750,200)));
    lines.append(QLine(QPoint(50,490),QPoint(750,490)));
    lines.append(QLine(QPoint(120,180),QPoint(120,200)));
    lines.append(QLine(QPoint(220,180),QPoint(220,200)));
    lines.append(QLine(QPoint(300,180),QPoint(300,200)));
    lines.append(QLine(QPoint(400,180),QPoint(400,490)));
    painter->drawLines(lines);

    QFont font;
    font.setPointSize(13);
    font.setFamily("黑体");
    //font.setItalic(true);
    painter->setFont(font);

    //first
    painter->drawText(50,50,700,40,Qt::AlignCenter,"深圳闪量科技有限公司");
    painter->drawText(QPoint(650,110),"No:");
    font.setPointSize(20);
    painter->setFont(font);
    painter->drawText(50,80,700,40,Qt::AlignCenter,ExGlobal::panelName());
    font.setPointSize(10);
    painter->setFont(font);
    //second
    painter->drawText(60,120,200,60,Qt::AlignVCenter,QString("样本ID： ")+sampCode);
    painter->drawText(250,120,200,60,Qt::AlignVCenter,QString("样本信息： ")+sampInfo);
    painter->drawText(450,120,200,60,Qt::AlignCenter,QString("仪器： ")+ExGlobal::SysName);
    //three
    painter->drawText(50,180,70,20,Qt::AlignCenter,"序号");
    painter->drawText(120,180,100,20,Qt::AlignCenter,"检测项目");
    painter->drawText(220,180,80,20,Qt::AlignCenter,"Ct");
    painter->drawText(300,180,100,20,Qt::AlignCenter,"结果");
    //four
#if 0
    painter->drawText(50,210,70,20,Qt::AlignCenter,"1");
    painter->drawText(120,210,100,20,Qt::AlignCenter,"ADV-2");
    //painter->drawText(220,210,80,20,Qt::AlignCenter,"12");
    painter->drawText(300,210,100,20,Qt::AlignCenter,"阴性");
#endif
    QMapIterator<QString,int> it(itemMap);
    int itemIndex = 1;
    int xPos = 205;
    while(it.hasNext()){
        it.next();
        if (it.value() != 0)
        {
            double ct = (double)(it.value())/10;
            painter->setPen(QPen(QColor(255,0,0),1));
            painter->drawText(220,xPos,80,20,Qt::AlignCenter,QString("%1").arg(ct));
            painter->drawText(300,xPos,100,20,Qt::AlignCenter,"阳性");
        }
        else
            painter->drawText(300,xPos,100,20,Qt::AlignCenter,"阴性");

        painter->drawText(50,xPos,70,20,Qt::AlignCenter,QString::number(itemIndex));
        painter->drawText(120,xPos,100,20,Qt::AlignCenter,it.key());
        painter->setPen(QPen(QColor(0,0,0),1));
        itemIndex++;
        xPos += 20;
    }
    //five
    painter->drawText(60,490,700,30,Qt::AlignVCenter,QString("送检者： "));
    painter->drawText(360,490,700,30,Qt::AlignVCenter,QString("检验者： ")+user);
    painter->drawText(590,490,700,30,Qt::AlignVCenter,QString("审核者： "));
    painter->drawText(60,520,700,30,Qt::AlignVCenter,QString("检测日期： ")+testTime);
    painter->drawText(360,520,700,30,Qt::AlignVCenter,QString("报告日期： ")+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    //draw coor
    int co_left = 50;
    int co_top = 700;
    font.setPointSize(6);
    painter->setFont(font);
    painter->drawLine(QLine(QPoint(co_left+50,co_top+100),QPoint(co_left+50+50*5,co_top+100)));
    for (int i = 0; i < 11; i++){
        painter->drawText(co_left+50-5+25*i,co_top+100,10,10,Qt::AlignCenter,QString::number(i*5));
    }
    int prenumber = qrand()%100;
    int currnumber = 0;
    painter->setPen(QPen(QColor(100,100,200),1));
    for (int i = 2; i < 40; i++){
        currnumber = qrand()%100;
        painter->drawLine(QLine(QPoint(co_left+50+(i-1)*5,co_top+100-prenumber),QPoint(co_left+50+i*5,co_top+100-currnumber)));
        prenumber = currnumber;
    }
    painter->end();

    return true;
}

bool printmgr::printPTP(){
    void *handle = Port_OpenUSBIO("/dev/usb/lp0");
    if (handle == nullptr)
    {
        qDebug()<<"Can not open printer";
        return false;
    }
    else
    {
        QString printStr;
        Port_SetPort(handle);
        Pos_Reset();
        Pos_PrintNVLogo(1);
        Pos_FeedLine();
        QString testName = ExGlobal::panelName()+"\n";
        Pos_Text(testName.toStdString().c_str(),0,-2,1,1,0,0);
        Pos_FeedLine();

        printStr = QString("仪器： ")+ExGlobal::SysName+"\n";
        Pos_Text(printStr.toStdString().c_str(),0,-1,0,0,0,0);
        printStr = QString("样本ID： ")+sampCode+"\n";
        Pos_Text(printStr.toStdString().c_str(),0,-1,0,0,0,0);
        printStr = QString("样本信息： ")+sampInfo+"\n";
        Pos_Text(printStr.toStdString().c_str(),0,-1,0,0,0,0);

        Pos_FeedLine();

        QMapIterator<QString,int> it(itemMap);
        int itemIndex = 1;
        while(it.hasNext()){
            it.next();
            printStr = QString::number(itemIndex)+" "+it.key();
            int n = printStr.length();
            while (n<26) {
                printStr+=" ";
                n++;
            }
            if (it.value() != 0){
                printStr += "阳性\n";
                Pos_Text(printStr.toStdString().c_str(),0,-1,0,0,0,8);
            }
            else
            {
                printStr += "阴性\n";
                Pos_Text(printStr.toStdString().c_str(),0,-1,0,0,0,0);
            }
            itemIndex++;
        }

        Pos_FeedLine();
        printStr = QString("检验者： ")+user+"\n";
        Pos_Text(printStr.toStdString().c_str(),0,-1,0,0,0,0);
        printStr = QString("审核者： ")+checker+"\n";
        Pos_Text(printStr.toStdString().c_str(),0,-1,0,0,0,0);
        printStr = QString("检测日期： ")+testTime+"\n";
        Pos_Text(printStr.toStdString().c_str(),0,-1,0,0,0,0);
        printStr = QString("报告日期： ")+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+"\n";
        Pos_Text(printStr.toStdString().c_str(),0,-1,0,0,0,0);

        Pos_FeedLine();
        Pos_Reset();
        Pos_Align(1);
        Pos_Qrcode("https://www.ubaike.cn/show_10459701.html\n",4);

        Pos_FeedLine();
        Pos_Barcode("SLX 123",0x45,0,3,96,0,2);
        Pos_Feed_N_Line(3);

        Port_ClosePort(handle);
    }
    return true;
}
