import QtQuick 2.11

Rectangle {
    id: node
    height: 8
    width: 8
    border.width: 1
    border.color: 'blue'
    color: 'black'

    MouseArea {
        anchors.fill: parent
        drag.target: node
    }
}
