import QtQuick 2.12

Item {
    id: positionMarker
    
    property var position: null

    onPositionChanged: {
        canvas.requestPaint();
    }


    onHeightChanged: {
        canvas.requestPaint();
    }

    Canvas {
        id: canvas
        anchors.fill: parent
        onPaint: {
            var ctx = canvas.getContext('2d');
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            if(position !== null) {
                let x = canvas.width / 2 + position[0] * canvas.width;
                let y = canvas.width / 2 + position[1] * canvas.height;
                ctx.strokeStyle = "white";
                ctx.lineWidth = 3;
                ctx.beginPath();
                ctx.arc(x, y, 10, 0, Math.PI * 2, true);
                ctx.stroke();
                ctx.closePath();
                ctx.strokeStyle = "black";
                ctx.beginPath();
                ctx.moveTo(x - 7, y - 7);
                ctx.lineTo(x + 7, y + 7);
                ctx.moveTo(x - 7, y + 7);
                ctx.lineTo(x + 7, y - 7);
                ctx.stroke();
                ctx.closePath();
            }
        }
    }
}

