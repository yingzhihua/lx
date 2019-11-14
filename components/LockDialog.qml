import QtQuick 2.12
import QtQuick.Controls 2.2
import QtQuick.Window 2.12

import QtQuick 2.0

Rectangle {
    id: lockDialog
    visible: false
    color: Qt.rgba(0.5,0.5,0.5,0.5)
    Rectangle{
        width: 400
        height: 200
        anchors.centerIn: parent
        border.color: "darkgray"
        radius: 20
    }
    function show(){
        lockDialog.visible = true
    }
    function exit(){
        lockDialog.visible = false;
    }
    MouseArea{
        anchors.fill: parent
    }
}
