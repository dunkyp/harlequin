import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.11
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.3

import splash.fish.harlequin 1.0

import "Colours.js" as Colours

ApplicationWindow
{
    id: root
    title: qsTr("Harlequin")
    property url sourceUrl: "qrc://../images/woman.jpg"
    property url clutUrl: "qrc://../images/identity.png"

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        Rectangle {
            SplitView.minimumWidth: 550
            SplitView.maximumWidth: root.height - toolBar.height
            ColumnLayout {
                anchors.fill: parent
                ToolBar {
                    id: toolBar
                    Row {
                        anchors.fill: parent
                        Column {
                            Label {
                                text: "Colour Space"
                                font.pointSize: Qt.application.font.pointSize;
                            }
                            ComboBox {
                                id: colourSpaceCombo
                                textRole: "text"
                                currentIndex: 0
                                model: ListModel {
                                    id: spaceItems
                                    ListElement { text: "HSV"; space: ColourWheel.HSP }
                                    ListElement { text: "L*C*h*"; space: ColourWheel.LAB }
                                }
                            }
                        }
                        Slider {
                            id: brightnessSlider
                            height: 50
                            to: 1
                            value: 0.7
                        }
                        Slider {
                            id: greySlider
                            height: 50
                            to: 2
                            from: 1
                            value: 1
                        }
                    }
                }

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Item {
                        anchors.fill: parent

                        Item {
                            id: colourSliceContainer
                            width: 40
                            height: parent.height

                            ColourSlice {
                                id: colourSlice
                                width: parent.width - 10
                                height: wheel.height
                                space: spaceItems.get(colourSpaceCombo.currentIndex).space
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                        }

                        Rectangle {
                            height: parent.height
                            width: parent.width - colourSliceContainer.width
                            anchors.right: parent.right

                            ColourWheel {
                                id: wheel
                                width: parent.width - 10
                                height: wheel.width
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.verticalCenter: parent.verticalCenter
                                brightness: brightnessSlider.value
                                greyFactor: greySlider.value
                                space: spaceItems.get(colourSpaceCombo.currentIndex).space
                                Connections {
                                    target: root
                                    onSourceUrlChanged: wheel.dirty()
                                }
                            }
                            ImageProjection {
                                width: wheel.height - 2 // fudged to account for edge blending in colourwheel
                                height: width
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.verticalCenter: parent.verticalCenter
                                image: mainImageView.sourceImage
                                space: spaceItems.get(colourSpaceCombo.currentIndex).space
                                greyFactor: greySlider.value
                                CurrentPositionMarker {
                                    id: currentPosition
                                    anchors.fill: parent
                                }
                            }
                            SampleGrid {
                                width: wheel.width + 10
                                height: wheel.height + 10
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            SplitView.fillWidth: true
            SplitView.fillHeight: true

            ExtendedImage {
                id: mainImageView
                source: root.sourceUrl
                anchors.centerIn: parent
                height: sourceHeight > sourceWidth ? parent.height : parent.width * sourceHeight / sourceWidth
                width: Math.min(sourceWidth > sourceHeight ? parent.width : parent.height * sourceWidth / sourceHeight, parent.width)
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        dialog.visible = true;
                    }
                    onPositionChanged: {
                        let x = Math.round(mainImageView.sourceWidth * (mouse.x / mainImageView.width));
                        let y = Math.round(mainImageView.sourceHeight * (mouse.y / mainImageView.height));
                        let color = mainImageView.color(x, y)
                        if(spaceItems.get(colourSpaceCombo.currentIndex).space == ColourWheel.LAB) {
                            let lch = Colours.rgb2lch(color)
                            currentPosition.position = [lch[1] / 100.0 * Math.cos(lch[2]) / 2, lch[1] / 100.0 * Math.sin(lch[2]) / 2];
                            colourSlice.hue = currentPosition.position[1];//lch[2];
                            colourSlice.chroma = currentPosition.position[2];//lch[1];
                            console.log(currentPosition.position);
                            //console.log([color.r, color.g, color.b], '   ->   ', lch[0], colourSlice.chroma, colourSlice.hue);
                        } else {
                            let hsv = Colours.rgb2hsv(color);
                            currentPosition.position = [hsv[1] / 100 * Math.cos(hsv[0] * (Math.PI / 180)) / 2, hsv[1] / 100 * Math.sin(hsv[0] * (Math.PI / 180)) / 2];
                            colourSlice.hue = hsv[0] / 360;
                            colourSlice.chroma = 0.5;//hsv[1] / 100;
                            console.log(hsv);
                        }
                    }
                    onExited: {
                        currentPosition.position = null;
                    }
                }

                FileDialog {
                    id: dialog
                    folder: shortcuts.home
                    Component.onCompleted: visible = false
                    onAccepted: {
                        root.sourceUrl = fileUrl
                    }
                }
            }
        }
    }

    Component.onCompleted: showMaximized()
}
