import QtQuick
import QtQuick.Layouts 1.11

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
