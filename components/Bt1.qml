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
        source: "qrc:/images/btexit.png"
    }

    Text{
        id:label
        x:130
        y:26
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
