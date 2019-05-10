import QtQuick 2.12

import "Grid.js" as Grid

Item {
    id: grid
    property int resolution: 8

    onHeightChanged: {
        canvas.requestPaint();
        Grid.rescale(grid);
    }

    Canvas {
        id: canvas
        anchors.fill: parent
        onPaint: {
            Grid.paint();
        }
    }

    Component.onCompleted: {
        Grid.initialise(grid, resolution);
        Grid.centroid.onMoved.connect(Grid.centroidMoved);
        Grid.centroid.onMoved.connect(canvas.requestPaint);
        Grid.points.forEach(point => {
            point.onMoved.connect(Grid.cartToPolar);
            point.onMoved.connect(canvas.requestPaint);
        });
        canvas.requestPaint();
    }

}
