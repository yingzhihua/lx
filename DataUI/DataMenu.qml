import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Global 1.0
import Dx.Sequence 1.0
Rectangle {    
    id: data_page
    Row{
        id:datatitle
        x:20
        y:0
        width: parent.width
        height: 100
        Text{color:"#888888";font.pixelSize: 36;verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;width:100;height:80;text:qsTr("序号")}
        Text{color:"#888888";font.pixelSize: 36;verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;width:200;height:80;text:qsTr("是否异常")}
        Text{color:"#888888";font.pixelSize: 36;verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;width:500;height:80;text:qsTr("测试项目")}
        Text{color:"#888888";font.pixelSize: 36;verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;width:300;height:80;text:qsTr("样本号")}
        Text{color:"#888888";font.pixelSize: 36;verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;width:200;height:80;text:qsTr("操作员")}
        Text{color:"#888888";font.pixelSize: 36;verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;width:200;height:80;text:qsTr("审核员")}
        Text{color:"#888888";font.pixelSize: 36;verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;width:400;height:80;text:qsTr("测试时间")}
    }

    ScrollView{
        id: datatb
        anchors.top: datatitle.bottom
        width: parent.width
        height: parent.height - datatitle.height
        clip: true
        //ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        //ScrollBar.vertical.background: Rectangle{color:"#808080"}
        ScrollBar.vertical.width: 50
        ListView{
            id:listView
            anchors.fill: parent
            spacing: 0
            model:testModel
            delegate: Rectangle{
                width:listView.width
                height:100
                color: (model.index%2 == 1)?"#ffffff":"#f8f8f8"
                Row{
                    x:20
                    y:0
                    width: parent.width
                    height: parent.height
                    //spacing: 10
                    Text{verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;width:100;height:parent.height;text:Testid}
                    Image {
                        width: 200
                        height: 100
                        fillMode: Image.Pad
                        verticalAlignment: Text.AlignVCenter
                        source: ResultType==2?"":"qrc:/images/thrAlarmSmall.png"
                    }
                    //Text{verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;width:200;text:SerialNo}
                    Text{verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;width:500;text:SampleInfo;clip:true;elide:Text.ElideRight;maximumLineCount: 2;wrapMode: Text.Wrap;height:parent.height}
                    Text{verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;width:300;height:parent.height;text:SampleId}
                    Text{verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;width:200;height:parent.height;text:User}
                    Text{verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;width:200;height:parent.height;text:Checker}
                    Text{verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;width:400;height:parent.height;text:TestTime}
                }

                MouseArea{
                    anchors.fill: parent
                    onPressed: {
                        console.log("Pressed:"+index);
                    }
                    onReleased: {
                        console.log("Released:"+index);
                    }

                    onClicked: {
                        if (ExGlobal.projectMode() !== 2){
                            busyIndicator.running = true;
                            console.log("click:"+index);
                            testModel.setCurrTest(index);
                            if (ResultType==2)
                                mainView.push("qrc:/DataUI/DataView.qml");
                            else
                                mainView.push("qrc:/DataUI/DataInvaildView.qml");
                            ExGlobal.setDataEntry(0);
                            busyIndicator.running = false;
                        }
                    }
                }
            }
        }
    }

    BusyIndicator{
        id:busyIndicator
        anchors.centerIn: parent
        implicitWidth: 200
        implicitHeight: 200
        opacity: 1.0
        running: false
        MouseArea{
            anchors.fill: parent
            enabled: busyIndicator.running?true:false
        }
    }

    Component.onCompleted: {
        Sequence.setTitle("datamenu");
        if (testModel.rowCount() > 8)
            datatb.ScrollBar.vertical.policy = ScrollBar.AlwaysOn
        console.log("DataMenu",testModel.rowCount())
    }
}
