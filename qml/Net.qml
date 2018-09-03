import QtQuick 2.11

Item {
    id: net
    property int gridSize: 20
    property int parentHeight: parent.height
    property var graph

    Repeater {
        id: repeater
        model: gridSize
        Point {
            x: Math.cos(((Math.PI * 2) / gridSize) * index)
            y: Math.sin(((Math.PI * 2) / gridSize) * index)
        }
    }



    Item {
        anchors.fill: parent

        Canvas {
            id: canvas
            anchors.fill: parent
            property var points: []

            onPaint: {
                var ctx = canvas.getContext('2d');
                ctx.moveTo(points[0].x, points[0].y);
                for(var i = 0; i < points.length; i++)
                    ctx.lineTo(points[(i + 1) % points.length].x, points[(i + 1) % points.length].y);
                ctx.stroke();
            }
            Component.onCompleted: {
                for(var i = 0; i < gridSize; i++) {
                    var radius = parentHeight / 2;
                    repeater.itemAt(i).x = repeater.itemAt(i).x * radius + radius;
                    repeater.itemAt(i).y = repeater.itemAt(i).y * radius + radius;
                    points.push(repeater.itemAt(i));
                }

                for(var i = 0; i < points.length; i++)
                    points[i].dragged.connect(repaint);
            }

            function repaint() {
                var ctx = getContext("2d");
                ctx.clearRect(0, 0, canvas.width, canvas.height);
                ctx.beginPath();
                requestPaint()
            }
        }
    }
}
