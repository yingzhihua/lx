import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Global 1.0
import Dx.Sequence 1.0
Rectangle {    
    id: data_page
    ListView{
        id:listView
        anchors.fill: parent
        spacing: 0
        model:testModel
        delegate: Rectangle{
            width:listView.width
            height:60
            color: (model.index%2 == 1)?"darkgrey":"#ffffff"
            Row{
                anchors.fill: parent
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:100;text:Testid}
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:200;text:PanelCode}
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:200;text:SerialNo}
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:200;text:SampleId}
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:500;text:SampleInfo}
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:300;text:TestTime}
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:150;text:User}
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:150;text:Checker}
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:50;text:Checker!=""?"√":""}
            }
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    console.log("click:"+index);
                    testModel.setCurrTest(index);
                    mainView.push("qrc:/DataUI/DataView.qml");
                    ExGlobal.setDataEntry(0);
                }
            }
        }
    }

    Component.onCompleted: {
        Sequence.changeTitle(qsTr("历史数据"));
    }
}
