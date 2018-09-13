import QtQuick 2.11
import "net.js" as Net

Item {
    id: net
    property int gridSize: 16
    property int parentHeight: parent.height
    signal samplesChanged(var samples);

    onGridSizeChanged: {
        console.log("getting ready to redraw points");
    }

    Repeater {
        id: repeater
        model: gridSize * gridSize + 1
        Point {
            x: Math.cos(((Math.PI * 2) / gridSize) * index)
            y: Math.sin(((Math.PI * 2) / gridSize) * index)
            offset: index
        }
    }



    Item {
        anchors.fill: parent

        Canvas {
            id: canvas
            anchors.fill: parent
            property var points: []
            onHeightChanged: Net.resize(height, points)

            onPaint: {
                var ctx = canvas.getContext('2d');
                Net.paint(ctx, gridSize, points);
            }


            Component.onCompleted: {
                for(var i = 0; i < repeater.count; i++) {
                    points.push(repeater.itemAt(i));
                    points[i].dragged.connect(repaint);
                }
                Net.initialise(gridSize, points, parentHeight);
            }

            function repaint(point) {
                var x = (points[point].x - height / 2);
                var y = (points[point].y - height / 2);
                var r = Math.sqrt(x * x + y * y);
                var theta = Math.atan2(y, x);
                // pin point in circle
                if(r > height / 2) {
                    r = height / 2;
                    points[point].x = r + r * Math.cos(theta);
                    points[point].y = r + r * Math.sin(theta);
                }

                var scale = r / (height / 2);
                // rotation and scale
                if(points[point].locked && point < points.length - 1) {
                    if(point == points.length - 1) {
                    } else {
                        var ringOffset = point % gridSize;
                        for(var ring = 0; ring < gridSize - 1; ring++) {
                            var i = ring * gridSize + ringOffset;
                            var radius = height / 2;
                            x = (points[i].x - radius);
                            y = (points[i].y - radius);
                            r = radius / gridSize * (ring + 1) * scale;
                            points[i].x = radius + r * Math.cos(theta);
                            points[i].y = radius + r * Math.sin(theta);
                        }
                    }
                }
                samplesChanged(points.map(function(point){return [point.x, point.y];}));
                var ctx = getContext("2d");
                ctx.clearRect(0, 0, canvas.width, canvas.height);
                ctx.beginPath();
                requestPaint()
            }
        }
    }
}
