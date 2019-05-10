import QtQuick.Controls 2.5

ComboBox {
    id: root
    property string label

    Label {
        text: root.label
        font.pixelSize: Qt.application.font.pointSize - 2
    }
}
