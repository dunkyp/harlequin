import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.0

import splash.fish.harlequin 1.0

ApplicationWindow
{
    visible: true
    width: 640
    height: 480
    title: qsTr("Harlequin")
    property url source_url: "file:///Users/duncan/code/personal/c++/harlequin/images/woman.jpg" 
    property url clut_url: "file:///Users/duncan/code/personal/c++/harlequin/images/Kodak_Kodachrome_64.png"
    property int split_mode: 0
    
    GridLayout {
        id: grid
        columns: 2
        anchors.fill: parent
        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Image {
                id: source_image
                source: source_url
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    sourceDialog.visible = true;
                }
            }
        }
        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Image {
                source: clut_url
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    clutDialog.visible = true;
                }
            }
        }
        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.columnSpan: 2
            HALD {
                id: result
                clut: clut_url
                source: source_url
                width: source_image.paintedWidth
                height: source_image.paintedHeight
                anchors.horizontalCenter: parent.horizontalCenter
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onPositionChanged: {
                        let posX;
                        let posY;
                        if(split_mode == 0) {
                            posX = 0;
                            posY = 0;
                        }
                        if(split_mode == 1) {
                            posX = mouseX / result.width;
                            posY = 0;
                        }
                        if(split_mode == 2) {
                            posX = 0;
                            posY = mouseY / result.height;
                        }
                        result.x_cut = posX;
                        result.y_cut = posY;
                    }
                }
            }
        }
    }
    Button {
        id: xy
        text: "="
        onClicked: {
            split_mode = (split_mode + 1) % 3
            if(split_mode == 0)
                xy.text = "=";
            if(split_mode == 1)
                xy.text = "|";
            if(split_mode == 2)
                xy.text = "-";
        }
    }


    FileDialog {
        id: clutDialog
        title: "Choose a clut"
        folder: shortcuts.home
        Component.onCompleted: visible = false
        onAccepted: {
            clut_url = clutDialog.fileUrl
        }
    }
    FileDialog {
        id: sourceDialog
        title: "Choose a source"
        folder: shortcuts.home
        Component.onCompleted: visible = false
        onAccepted: {
            source_url = sourceDialog.fileUrl
        }
    }
}
