import QtQuick 2.0

Rectangle {
    id: root
    property alias image: img.source
    property alias text: bttext.text
    property int textoffsetx: 0
    property int textoffsety: 0
    property alias textcolor: bttext.color
    signal clicked    
    color: "transparent"
    Image {
        id: img
        fillMode: Image.Pad
        anchors.centerIn: parent
        source: ""
    }

    Text {
        id: bttext
        font.pixelSize: 40
        anchors.centerIn: parent
        anchors.horizontalCenterOffset: textoffsetx
        anchors.verticalCenterOffset: textoffsety
    }

    MouseArea{
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
