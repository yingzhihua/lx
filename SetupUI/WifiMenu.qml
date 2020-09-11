import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Global 1.0
import Dx.Sequence 1.0
import "../components"

Rectangle {
    id: wifi_page    
    color: "#f5f5f5"

    function signalSelect(sign){
        if (sign > 70)
            return "qrc:/images/sign3.png";
        else if (sign > 50)
            return "qrc:/image2/sign2.png";
        else if (sign > 30)
            return "qrc:/images/sign1.png";
        return "qrc:/images/sign0.png";
    }

    Component{
        id:listdelegate
        Item{
            id:delegateitem
            width: parent.width-30
            height: 102
            Row{
                anchors.verticalCenter: parent.verticalCenter
                Rectangle{
                    width: 630
                    height: parent.height

                    Image {x:50;y:20;fillMode: Image.Pad;source: "qrc:/images/wifiselect.png";visible: Connected}
                    Text{x:120;y:10;width:500;text:SSID;font.pixelSize: 40;color: "#323232";clip:true}
                    Text{x:120;y:60;text:Security;font.pixelSize:25;color: "#b2b2b2"}
                    Text{x:400;y:60;text:Signal;font.pixelSize:25;color: "#b2b2b2"}
                }
                //Text{horizontalAlignment: Text.AlignHCenter;width:100;text:Signal}
                //Text{horizontalAlignment: Text.AlignHCenter;width:100;text:Connected}
                Image {fillMode: Image.Pad;width:100;height:100;verticalAlignment: Text.AlignVCenter;source: signalSelect(Signal)}
            }
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    delegateitem.ListView.view.currentIndex = index
                    queryDlg.show(wifiModel.data(wifiModel.index(index,0),257))
                }
            }
        }
    }

    Rectangle{
        width: 799
        height: 469
        radius: 22
        anchors.centerIn: parent

        Image {
            anchors.fill: parent
            source: "qrc:/images/wifibg.png"
        }

        Label{
            anchors.horizontalCenter: parent.horizontalCenter
            y:18
            font.pixelSize: 45
            text:qsTr("可用WLAN列表")
        }
        ScrollView{
            y: 95
            width: parent.width
            height: parent.height-120
            clip: true
            ScrollBar.vertical.policy: ScrollBar.AlwaysOn
            ScrollBar.vertical.width: 30
            ScrollBar.vertical.background: Rectangle{color:"#808080"}
            background: Rectangle{color:"#ffffff"}
            ListView{
                id:listView
                width: parent.width
                model: wifiModel
                delegate: listdelegate
            }

        }
    }


    Bt1 {
        id: btCannel
        onClicked: {
            Sequence.setTitle("setup")
            mainView.pop();
        }
    }

    Bt2{
        id: btflash
        anchors.bottom: btCannel.top
        anchors.bottomMargin: 30
        anchors.horizontalCenter: btCannel.horizontalCenter
        height: 106
        width: 299
        textoffsetx: 30
        textoffsety: -4
        textcolor: "#ffffff"
        image: "qrc:/images/confirmbt.png"
        text: qsTr("刷新列表")
        onClicked: {
            wifiModel.refresh()
        }
    }

    Query{
        id: queryDlg
        qtitle: qsTr("输入WIFI密码")
        qlable: qsTr("密码")
        qbutton: qsTr("连接")
        anchors.fill: parent
        onQueryAck: {
            console.log(wifiModel.data(wifiModel.index(listView.currentIndex,0),257),res);
            if (wifiModel.connect(wifiModel.data(wifiModel.index(listView.currentIndex,0),257),res)){
                console.log("connect");
            }
        }
    }

    Component.onCompleted:{
        Sequence.setTitle("setup_wifi")
    }
}
