import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Global 1.0
import "../components"

Rectangle {
    id: user_page

    property int widthx: 400
    property int itemheight: 60

    Component{
        id:listheader
        Rectangle{
            width: widthx
            height: itemheight
            color: "lightgray"
            Label{
                anchors.centerIn: parent
                text:qsTr("用户列表")
            }
        }
    }

    Component{
        id:listdelegate
        Item{
            id:delegateitem
            width: widthx
            height: itemheight
            Row{
                anchors.verticalCenter: parent.verticalCenter
                Text{width:100;text:Userid}
                Text{text:Name}
            }
            states: State {
                name: "Current"
                when: delegateitem.ListView.isCurrentItem
                PropertyChanges {target: delegateitem; x:20;scale:1.1}
            }

            transitions: Transition {
                NumberAnimation{easing.type: Easing.Linear;properties: "x";duration: 200}
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                   delegateitem.ListView.view.currentIndex = index
                   inputName.text = userModel.data(userModel.index(index,0),258)
                }
            }
        }
    }

    Component{
        id:highlightrec
        Rectangle{
            width: widthx
            height: itemheight
            color: "#f34b08"
            radius: 5
            border.width: 1
            border.color: "#60f50a"
        }
    }

    ListView{
        id:listView
        x:20
        y:20
        width: widthx
        height: parent.height - 40
        focus: true
        highlight: highlightrec

        highlightFollowsCurrentItem: true
        header: listheader
        model: userModel
        delegate: listdelegate
    }

    Text{
        id:labelName
        anchors.left: listView.right
        anchors.leftMargin: 450
        anchors.top: parent.top
        anchors.topMargin: 300
        width: 80
        height: 30
        text: qsTr("用户名：")
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 40
    }

    TextField{
        id:inputName
        anchors.verticalCenter: labelName.verticalCenter
        anchors.left: labelName.right
        anchors.leftMargin: 50
        width:300
        height: 60
        verticalAlignment: Text.AlignBottom
        font.pixelSize: 40
        background: Rectangle{
            border.color: "darkgray"
            radius: 5
        }
    }

    Text{
        id:labelPassword
        anchors.right: labelName.right
        anchors.rightMargin: 0
        anchors.top: labelName.bottom
        anchors.topMargin: 60
        width: 80
        height: 30
        text: qsTr("密码：")
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 40
    }

    TextField{
        id:inputPassword
        anchors.verticalCenter: labelPassword.verticalCenter
        anchors.left: labelPassword.right
        anchors.leftMargin: 50
        width:300
        height: 60
        verticalAlignment: Text.AlignBottom
        text:"123456"
        font.pixelSize: 40
        background: Rectangle{
            border.color: "darkgray"
            radius: 5
        }
    }

    Button{
        id: btSave
        anchors.top: labelPassword.bottom
        anchors.topMargin: 80
        anchors.horizontalCenter: labelPassword.horizontalCenter
        anchors.horizontalCenterOffset: 30
        width: 150
        height: 50

        contentItem: Text {
                  text: qsTr("增加")
                  color: "white"
                  horizontalAlignment: Text.AlignHCenter
                  verticalAlignment: Text.AlignVCenter
                  elide: Text.ElideRight
                  font.pixelSize: 30
              }
        background: Rectangle{
            color: "darkgray"
            radius: 5
        }

        onClicked: {
            userModel.addUser(inputName.text,inputPassword.text)
        }
    }

    Button{
        id: btDelete
        anchors.left: btSave.right
        anchors.leftMargin: 80
        anchors.verticalCenter: btSave.verticalCenter
        width: 150
        height: 50

        contentItem: Text {
                  text: qsTr("删除")
                  color: "white"
                  horizontalAlignment: Text.AlignHCenter
                  verticalAlignment: Text.AlignVCenter
                  elide: Text.ElideRight
                  font.pixelSize: 30
              }
        background: Rectangle{
            color: "darkgray"
            radius: 5
        }
        onClicked: {
            userModel.deleteUser(listView.currentIndex)
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
}
