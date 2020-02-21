import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Global 1.0
Rectangle {
    property string titlemsg: qsTr("历史数据")
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
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:1000;text:SampleInfo}
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:300;text:TestTime}
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:200;text:User}
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:100;text:ResultType}
            }
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    console.log("click:"+index);
                    testModel.setCurrTest(index);
                    dataView.push("qrc:/DataUI/DataView.qml");
                    ExGlobal.setDataEntry(0);
                }
            }
        }
    }
    onTitlemsgChanged: headerMsg.text = titlemsg;
}
