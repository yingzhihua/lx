import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Global 1.0
import Dx.Sequence 1.0
import "../components"

Rectangle {
    id: user_page

    property int widthx: 800
    property int itemheight: 80

    Component{
        id:listdelegate
        Item{
            id:delegateitem
            width: parent.width-50
            height: itemheight
            Row{
                anchors.verticalCenter: parent.verticalCenter
                Text{id:name;horizontalAlignment: Text.AlignHCenter;width:200;text:Name}
                Text{id:displayname;horizontalAlignment: Text.AlignHCenter;text:DisplayName;width: 300}
                Text{id:check;horizontalAlignment: Text.AlignHCenter;text:qsTr("审核");width: 200;visible: UserType&2?true:false}
            }
            states: State {
                name: "Current"
                when: delegateitem.ListView.isCurrentItem
                PropertyChanges {target: delegateitem; x:20;scale:1.1}
                PropertyChanges {target: name; color:"#59cddf"}
                PropertyChanges {target: displayname; color:"#59cddf"}
                PropertyChanges {target: check; color:"#59cddf"}
            }

            transitions: Transition {
                NumberAnimation{easing.type: Easing.Linear;properties: "x";duration: 200}
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    delegateitem.ListView.view.currentIndex = index
                    userModel.setCurrIndex(index)
                   //inputName.text = userModel.data(userModel.index(index,0),258)
                   //inputDisplayName.text = userModel.data(userModel.index(index,0),260)
                   //auditor.checked = userModel.data(userModel.index(index,0),261)&2;
                }
            }
        }
    }

    Component{
        id:highlightrec
        Rectangle{
            width: widthx
            height: itemheight
            //color: "#f34b08"
            color:"#F2F2F2"
            radius: 5
            border.width: 1
            border.color: "#60f50a"
        }
    }

    Rectangle{
        width: 788
        height: 581
        radius: 22
        anchors.centerIn: parent

        Image {
            anchors.fill: parent
            source: "qrc:/images/userbk.png"
        }

        Row{
            id: headRow
            x:0
            y:0
            width: parent.width
            height: 96
            Text{text:qsTr("帐号");verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;height:parent.height;width: 200;font.pixelSize: 40}
            Text{text:qsTr("显示名");verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;height:parent.height;width: 300;font.pixelSize: 40}
            Text{text:qsTr("权限");verticalAlignment: Text.AlignVCenter;horizontalAlignment:Text.AlignHCenter;height:parent.height;width: 200;font.pixelSize: 40}
        }

        Row{
            id: footerRow
            x:20
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            width: parent.width
            height: 135
            Bt2{
                id: btDelete
                width: 250
                height: parent.height
                textoffsetx: 30
                textoffsety: -3
                textcolor: "#464646"
                text: qsTr("删除")
                image: "qrc:/images/deletebt.png"
                onClicked: {
                    confirmDlg.show(qsTr("确定删除帐号？"))
                }
            }
            Bt2{
                id: btUpdate
                width: 250
                height: parent.height
                textoffsetx: 30
                textoffsety: -3
                textcolor: "#464646"
                text: qsTr("修改")
                image: "qrc:/images/updatebt.png"
                onClicked: {
                    console.log(listView.currentIndex)
                    if (listView.currentIndex >= 0)
                        mainView.push("qrc:/SetupUI/UpdateUser.qml")
                }
            }
            Bt2{
                id: btAdd
                width: 250
                height: parent.height
                textoffsetx: 30
                textoffsety: -3
                textcolor: "#ffffff"
                text: qsTr("添加")
                image: "qrc:/images/addbt.png"
                onClicked: {
                    mainView.push("qrc:/SetupUI/AddUser.qml")
                }
            }
        }

        ScrollView{
            anchors.top: headRow.bottom
            anchors.topMargin: 0
            width: parent.width
            height: parent.height-headRow.height-footerRow.height
            clip: true
            ScrollBar.vertical.policy: ScrollBar.AlwaysOn
            ScrollBar.vertical.width: 50
            ScrollBar.vertical.background: Rectangle{color:"#808080"}
            background: Rectangle{color: "#f5f5f5"}

            ListView{
                id:listView
                width: parent.width
                focus: true
                highlight: highlightrec

                highlightFollowsCurrentItem: true
                model: userModel
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

    SecondConfirm{
        id: confirmDlg
        anchors.fill: parent
        onQueryAck: {
            if (res == "confirm")
                userModel.deleteUser(listView.currentIndex)
        }
    }

    Component.onCompleted:{
        Sequence.setTitle("setup_user")
    }
}

