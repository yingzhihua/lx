import QtQuick 2.12
import QtQuick.Controls 2.5
import QtCharts 2.3

import Dx.Sequence 1.0
import Dx.Global 1.0
import "../components"
Page {
    property int showtype: 0
    property int showlight: 1
    property bool idle: false
    id: testProcessT

    Rectangle{
        id:mainAnaImage
        x:10
        y:10
        width:  1040//652
        height: 780//490
        color:"lightskyblue"

        Text{
            id:viewText
            font.pixelSize: 40
            anchors.centerIn: parent
        }
        Image {
            id: anaMainBox
            anchors.centerIn: parent
            width: parent.width - 4
            height: parent.height - 4
            source: ""
        }
    }

    Button{
        id:removemask
        anchors.left: mainAnaImage.left
        anchors.leftMargin: 50
        anchors.top: mainAnaImage.bottom
        anchors.topMargin: 10
        text: qsTr("去除蒙板")
        font.pixelSize: 30
        onClicked: {
            showtype = 1;
            Sequence.showAnaImg(showtype,showlight);
        }
    }
    Button{
        id:addmask
        anchors.left: removemask.right
        anchors.leftMargin: 20
        anchors.top: mainAnaImage.bottom
        anchors.topMargin: 10
        text: qsTr("显示蒙板")
        font.pixelSize: 30
        onClicked: {
            showtype = 0;
            Sequence.showAnaImg(showtype,showlight);
        }
    }
    Button{
        id:updatelight
        anchors.left: addmask.right
        anchors.leftMargin: 20
        anchors.top: mainAnaImage.bottom
        anchors.topMargin: 10
        text: qsTr("调整亮度")
        font.pixelSize: 30
        onClicked: {
            showlight = (showlight+1)%4;
            Sequence.showAnaImg(showtype,showlight);
        }
    }
    Text{
        id: message
        height: 100
        width: parent.width - 40
        text:"ExGlobal.s"
        font.pixelSize: 20
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.top: removemask.bottom
        anchors.topMargin: 10
        wrapMode: Text.Wrap
    }

    Grid{
        id: gridSerialSelect
        anchors.horizontalCenter: chartView.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: mainAnaImage.right
        anchors.leftMargin: 10
        columns:6
        rowSpacing: 20
        columnSpacing: 4

        Repeater{
            //model: ["IC","IFA","IFB","RSV-A","RSV-B","ADV-1","ADV-2","PIV-1","PIV-2","PIV-3","PIV-4","MP","BP","BP-IS97","HRV","MP-r2","BP-893"]
            model: ExGlobal.getPosNameArray()
            CheckBox{
                checked: true
                text:modelData
                objectName: "cb"
                onCheckedChanged: {
                    //console.log(this.text,this.checked,chartView.count);
                    for (var i = 0; i < chartView.count; i++){
                        if (this.text === chartView.series(i).name)
                        chartView.series(i).visible = this.checked;
                    }
                }
            }
        }
    }

    ChartView {
        id: chartView
        title: "实时测试曲线"
        anchors.top: gridSerialSelect.bottom
        anchors.topMargin: 10
        anchors.left: mainAnaImage.right
        anchors.leftMargin: 10
        height: 600
        width: 860
        legend.visible: false
        //legend.alignment: Qt.AlignTop
        animationOptions: ChartView.SeriesAnimations
        antialiasing: true

        ValueAxis{
            id: axisX
            min: 0
            max: 50
            tickCount: 6
            minorTickCount: 9
            labelFormat: "%d"
        }

        ValueAxis{
            id:axisY
            min:0
            max:50
            labelFormat: "%d"
        }
    }

    Button {
        id: btCannel
        font.pixelSize: 30
        width: 200
        text: qsTr("取消测试")
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 50
        anchors.right: parent.right
        anchors.rightMargin: 20
        onClicked: {
            if (idle == true)
            {                
                mainView.pop();
                mainView.push("qrc:/HomeUI/Idle.qml");
            }
            else
                Sequence.sequenceCancel();
        }
    }

    AutoCloseMsg{
        id:closemsg
        anchors.fill: parent
        onCloseAck: {
            Sequence.uiStage = Sequence.Stage_idle;
            mainView.pop();
            mainView.push("qrc:/HomeUI/Idle.qml");
        }
    }

    Component.onCompleted: {
        Sequence.uiStage = Sequence.Stage_test;
        Sequence.sequenceDo(Sequence.Sequence_Test);
        console.log(ExGlobal.getPosNameArray());
        //var newline = chartView.createSeries(ChartView.SeriesTypeLine,"lx");
        //newline.axisX = axisX;
        //newline.axisY = axisY;
        //newline.append(2,30);
        //newline.append(12,60);
    }

    Connections{
        target: Sequence
        onProcessFinish:{
            /*
            console.log("TestProcess.qml,total:"+total+",finish:"+finish);
            var ffinish = finish*1000/total;
            if (ffinish >= 1000)
            {
                testCompleted.width = 0;
                testunCompleted.width = 0;
            }
            else
            {
                testCompleted.width = ffinish*headerRec.width/1000;
                testunCompleted.x = testCompleted.width;
                testunCompleted.width = (1000-ffinish)*headerRec.width/1000;
            }

            if (total == finish)
            {
                headerMsg.text = "测试完成！";
                idle = true;                
            }
            else
                headerMsg.text = "预计剩余"+((total-finish)/1000)+"秒，"+"  耗时："+Sequence.getCurrTestTime();
                */
            message.text = Sequence.sequenceMessage();
        }

        onSequenceFinish:{
            if (result == Sequence.Result_SelfCheck_ok)
            {
                mainView.pop();
                mainView.push("qrc:/HomeUI/Login.qml");
            }
            else if(result == Sequence.Result_CannelTest_ok)
            {                
                mainView.pop();
                mainView.push("qrc:/HomeUI/Idle.qml");
            }
            else if(result == Sequence.Result_Test_finish){
                headerMsg.text = "测试完成！";
                mainView.pop();
                //mainView.push("qrc:/HomeUI/Idle.qml");
                mainView.push("qrc:/DataUI/DataView.qml");
                ExGlobal.setDataEntry(1);
                Sequence.uiStage = Sequence.Stage_idle;
            }
            else if(result == Sequence.Result_Test_unfinish){
                headerMsg.text = "测试完成！";
                closemsg.show(qsTr("试剂盒异常，测试非正常完成！"))
            }

            console.log("TestProcessT.qml,result:"+result);
        }

        onCallQmlRefeshAnaMainImg:{
            anaMainBox.source = "";
            anaMainBox.source = "image://CodeImg/AnaMain";
            console.log("CameraSetup.qml,onCallQmlRefeshAnaMainImg");
        }

        onCallQmlRefeshData:{
            if (index == 1){
                chartView.removeAllSeries();
                for (var i = 0; i < item.length; i++)
                {
                    var newline =chartView.createSeries(ChartView.SeriesTypeLine,ExGlobal.getPosName(item[i]));
                    newline.axisX = axisX;
                    newline.axisY = axisY;

                    var list = gridSerialSelect.children;
                    for (var ch in list)
                        if (list[ch].objectName === "cb" && list[ch].text === newline.name)
                            newline.visible = list[ch].checked;
                }
            }

            for (i = 0; i < item.length; i++)
            {
                if (value[i] > axisY.max - 10)
                    axisY.max = value[i] + 10;
                chartView.series(i).append(index,value[i]);                
            }

            console.log(index,item,value);
        }
    }
}
