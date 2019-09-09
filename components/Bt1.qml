import QtQuick 2.0

Rectangle {
    id: root
    property alias text: label.text
    signal clicked

    width: 180
    height: 70
    //border.color: "orange"
    //color: "olivedrab"
    Image {
        id: img
        x:3
        y:3
        width: 64
        height: 64
        source: "qrc:/image/quit.png"
    }
    Text{
        id:label
        anchors.left: img.right
        anchors.leftMargin: 10
        anchors.verticalCenter: img.verticalCenter
        font.pixelSize: 40
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
