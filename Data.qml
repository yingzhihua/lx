import QtQuick 2.12
import QtQuick.Controls 2.5

Page {
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
            }
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    console.log("click:"+index);
                }
            }
        }
    }

/*
    Column{
        Repeater{
            model: dataModel

            Rectangle{
                height: 90
                width: dataPage.width
                color: (model.index%2 == 1)?"darkgrey":"#ffffff"
                Grid{
                    rows: 2
                    columns: 2
                    rowSpacing: 2
                    columnSpacing: 30
                    anchors.bottom: parent.bottom
                    Text {
                        text: dataTime
                    }
                    Text {
                        text: dataCode
                    }
                    Text {
                        text: dataDate
                    }
                    Text {
                        text: dataInfo
                    }
                }
            }
        }
    }
    */
    Component.onCompleted: {
        //console.log("data-model:"+testModel.rowCount());
        //console.log("data-dt:"+testModel[0].dt);
    }
    onTitlemsgChanged: headerMsg.text = titlemsg;
}
