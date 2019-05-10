import QtQuick 2.12

Item {
    id: node
    property bool locked: false
    property bool selected: false
    property real startR
    property real startTheta
    property real unscaledX
    property real unscaledY
    property int index
    signal moved(int id);

    Rectangle {
        anchors.centerIn: parent
        width: 10
        height: 10
        opacity: 0

        MouseArea {
            id: dragArea
            anchors.fill: parent
            drag.target: node.locked ? node : null
            drag.threshold: 0
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            onClicked: {
                locked = mouse.button === Qt.RightButton ? !locked : locked;
                moved(node.index);
            }
            onPressed: {
                selected = locked;
                moved(node.index);
            }
            onReleased: {
                selected = selected && mouse.modifiers & Qt.ShiftModifier;
                moved(node.index);
            }
            onPositionChanged: moved(node.index)
        }
    }
}
