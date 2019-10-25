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
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:200;text:Testid}
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:300;text:PanelCode}
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:300;text:SerialNo}
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:300;text:SampleInfo}
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:400;text:TestTime}
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:300;text:User}
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:300;text:ResultType}
            }
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    console.log("click:"+index);
                    testModel.setCurrTest(index);
                    dataView.push("qrc:/DataUI/DataView.qml");
                }
            }
        }
    }
    onTitlemsgChanged: headerMsg.text = titlemsg;
}
