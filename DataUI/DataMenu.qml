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
        anchors.top: datatitle.bottom
        width: parent.width
        height: parent.height - datatitle.height
        clip: true
        //ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        ScrollBar.vertical.width: 50
        ListView{
            id:listView
            anchors.fill: parent
            spacing: 0
            model:testModel
            delegate: Rectangle{
                width:listView.width
                height:100
                //color: (model.index%2 == 1)?"darkgrey":"#ffffff"
                Row{
                    x:20
                    y:0
                    width: parent.width
                    //spacing: 10
                    Text{verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;width:100;text:Testid}
                    Text{verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;width:200;text:SerialNo}
                    Text{verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;width:500;text:SampleInfo;clip:true}
                    Text{verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;width:300;text:SampleId}
                    Text{verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;width:200;text:User}
                    Text{verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;width:200;text:Checker}
                    Text{verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;width:400;text:TestTime}

                    /*
                    Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:100;text:Testid}
                    Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:200;text:PanelCode}
                    Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:200;text:SerialNo}
                    Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:200;text:SampleId}
                    Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:500;text:SampleInfo;clip: true}
                    Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:300;text:TestTime}
                    Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:150;text:User}
                    Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:150;text:Checker}
                    Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:50;text:Checker!=""?"√":""}
                    */
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
                            console.log("click:"+index);
                            testModel.setCurrTest(index);
                            mainView.push("qrc:/DataUI/DataView.qml");
                            ExGlobal.setDataEntry(0);
                        }
                    }
                }
            }
        }
    }
    Component.onCompleted: {
        Sequence.changeTitle(qsTr("历史数据"));
    }
}
