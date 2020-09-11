import QtQuick 2.0

Rectangle {
    id: root
    property alias text: label.text
    signal clicked

    width: 299
    height: 106
    //border.color: "orange"
    color: Qt.rgba(0,0,0,0)

    anchors.bottom: parent.bottom
    anchors.bottomMargin: 40
    anchors.right: parent.right
    anchors.rightMargin: 80

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
        x:140
        y:24
        font.pixelSize: 40        
        text:qsTr("返回")
    }


    MouseArea{
        anchors.fill: parent
        onClicked: root.clicked()
        onPressed: img.x = 10
        onReleased: img.x = 0
    }
}
