import QtQuick 2.0
Item {
    id: root
    property bool locked: false
    property int offset
    signal dragged(int offset)

    Rectangle {
        anchors.centerIn: parent
        width: 10
        height: 10
        color: "transparent"
        opacity: 0.3

        MouseArea {
            anchors.fill: parent
            drag.target: root.locked ? root : null
            drag.threshold: 0
            onPositionChanged: {
                if(drag.active) {
                    dragged(offset)
                }
            }
        }
    }
}
