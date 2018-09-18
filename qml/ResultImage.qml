import QtQuick 2.11
import QtQuick.Controls 2.4

import splash.fish.harlequin 1.0

Rectangle {
    property url sourceUrl
    property url clutUrl
    property real sourceWidth
    property real sourceHeight
    property int splitMode: 0
    property var splitLabels: ["A/B", "‖", "═"]
    property alias result: result


    Column {
        anchors.fill: parent
        ToolBar {
            Row {
                anchors.fill: parent
                ToolButton {
                    text: splitLabels[splitMode]
                    onClicked: {
                        splitMode = (splitMode + 1) % 3;
                        result.xCut = 0;
                        result.yCut = 0;
                    }
                }
            }
        }
        HALD {
            id: result
            clut: clutUrl
            source: sourceUrl
            width: sourceWidth
            height: sourceHeight
            anchors.horizontalCenter: parent.horizontalCenter

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onPositionChanged: {
                    let posX;
                    let posY;
                    if(splitMode == 0) {
                        posX = 0;
                        posY = 0;
                    }
                    if(splitMode == 1) {
                        posX = mouseX / result.width;
                        posY = 0;
                    }
                    if(splitMode == 2) {
                        posX = 0;
                        posY = mouseY / result.height;
                    }
                    result.xCut = posX;
                    result.yCut = posY;
                }
            }
        }
    }
}

