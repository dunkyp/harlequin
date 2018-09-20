import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11
import QtQuick.Window 2.2

import splash.fish.harlequin 1.0

ApplicationWindow
{
    id: root
    visible: true
    width: 640
    height: 480
    title: qsTr("Harlequin")
    property url sourceUrl: "file:///home/duncan/code/c++/harlequin/images/woman.jpg"
    property url clutUrl: "qrc://../images/identity.png"

    SwipeView {
        id: view
        currentIndex: 0
        anchors.fill: parent
        Item {
            ColumnLayout {
                anchors.fill: parent
                ToolBar {
                    id: toolBar
                    Row {
                        anchors.fill: parent
                        ComboBox {
                            id: colourSpaceCombo
                            textRole: "text"
                            currentIndex: 0
                            model: ListModel {
                                id: spaceItems
                                ListElement { text: "HSP"; space: ColourWheel.HSP }
                                ListElement { text: "LAB"; space: ColourWheel.LAB }
                            }
                        }
                    }
                }
                
                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Slider {
                        id: slider
                        height: 50
                        width: parent.width
                        to: 24
                        value: 16
                    }

                    ColourWheel {
                        id: wheel
                        height: parent.height
                        width: wheel.height
                        anchors.horizontalCenter: parent.horizontalCenter
                        brightness: 70
                        space: spaceItems.get(colourSpaceCombo.currentIndex).space
                        Connections {
                            target: net
                            onSamplesChanged: {
                                wheel.samplesChanged(samples);
                            }
                        }
                        Connections {
                            target: result.result
                            onSourceImageChanged: {
                                wheel.setInputImage(image);
                            }
                        }
                        Component.onCompleted: wheel.setInputImage(result.result.sourceImage)

                        Net {
                            id: net
                            anchors.fill: parent
                            gridSize: slider.value
                        }
                    }
                }
            }
        }
        
        Item {
            GridLayout {
                visible: true
                id: grid
                columns: 2
                anchors.fill: parent

                TextureImage {
                    id: source
                    sourceUrl: root.sourceUrl
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    onSourceUrlChanged: root.sourceUrl = sourceUrl
                }

                TextureImage {
                    id: clut
                    sourceUrl: root.clutUrl
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    onSourceUrlChanged: root.clutUrl = sourceUrl
                }
                
                Result {
                    id: result
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                    sourceUrl: root.sourceUrl
                    clutUrl: root.clutUrl
                    sourceWidth: source.paintedWidth
                    sourceHeight: source.paintedHeight
                }
            }
        }
    }

    PageIndicator {
        id: indicator
        count: view.count
        currentIndex: view.currentIndex
        anchors.bottom: view.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
