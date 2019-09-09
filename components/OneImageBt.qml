import QtQuick 2.0

Rectangle {
    id: root
    property alias source: img.source
    signal clicked

    Image {
        id: img
        x:3
        y:3
        source: ""
    }

    MouseArea{
        anchors.fill: parent
        onClicked: root.clicked()
        onPressed: img.x = 10
        onReleased: img.x = 0
    }
}
