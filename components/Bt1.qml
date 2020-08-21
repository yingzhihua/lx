import QtQuick 2.0

Rectangle {
    id: root
    property alias text: label.text
    signal clicked

    width: 299
    height: 106
    //border.color: "orange"
    color: Qt.rgba(0,0,0,0)
    Image {
        id: img
        x:3
        y:3
        width: 299
        height: 106
        source: "qrc:/images/back.png"
    }

    Text{
        id:label
        anchors.left: img.right
        anchors.leftMargin: 10
        anchors.verticalCenter: img.verticalCenter
        font.pixelSize: 40
        visible: false
        //color: "powderblue"
        text:qsTr("返回")
    }


    MouseArea{
        anchors.fill: parent
        onClicked: root.clicked()
        onPressed: img.x = 10
        onReleased: img.x = 0
    }
}
