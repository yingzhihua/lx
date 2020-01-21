import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Global 1.0

import "../components"

Rectangle {
    id: wifi_page
    ListView{
        id:listView
        x:0
        y:0
        width: parent.width*2/3
        height: parent.height
        spacing: 0
        model:wifiModel
        delegate: Rectangle{
            width:listView.width
            height:60
            //color: (model.index%2 == 1)?"darkgrey":"#ffffff"
            Row{
                anchors.fill: parent
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:600;text:SSID}
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:200;text:Signal}
                Text{height:parent.height;verticalAlignment: Text.AlignVCenter;width:200;text:Connected}
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {                    
                    //console.log("click:"+wifiModel.data(wifiModel.index(index,0),257));
                    queryDlg.show(wifiModel.data(wifiModel.index(index,0),257))
                }
            }
        }
    }
    Bt1 {
        id: btCannel
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20
        onClicked: {
            setupView.pop();
        }
    }

    Query{
        id: queryDlg
        qtitle: qsTr("输入WIFI密码")
        qlable: qsTr("密码：")
        qbutton: qsTr("连接")
        anchors.fill: parent
        onQueryAck: {
            console.log(wifiModel.data(wifiModel.index(listView.currentIndex,0),257),res);
            if (wifiModel.connect(wifiModel.data(wifiModel.index(listView.currentIndex,0),257),res)){
                console.log("conneced");
            }
        }
    }

    Button {
        id: btflash
        text: qsTr("刷新列表")
        anchors.right: parent.right
        anchors.rightMargin: 50
        anchors.bottom: btCannel.top
        anchors.bottomMargin: 100
        onClicked: {
            wifiModel.refresh()
            //queryDlg.show()
        }
    }
}
