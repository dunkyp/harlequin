import QtQuick 2.11
import "grid.js" as MeshGrid

Item {
    id: net
    property int gridSize: 15
    property int parentHeight: parent.height
    property var graph


    function updateNet() {
        children = [];
        MeshGrid.createGrid(gridSize, parentHeight, parentHeight, net);
    }

    Component.onCompleted: {
        updateNet();
    }

    onGridSizeChanged: updateNet()

    onParentHeightChanged: updateNet()
}
