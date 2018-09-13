var previousHeight;

function paint(ctx, gridSize, points) {
    for(var ring = 0; ring < gridSize; ring++) {
        ctx.beginPath();
        ctx.moveTo(points[gridSize * ring].x, points[gridSize * ring].y);
        for(var i = 0; i < gridSize; i++)
            ctx.lineTo(points[((i + 1) % gridSize) + gridSize * ring].x, points[((i + 1) % gridSize) + gridSize * ring].y);
        ctx.stroke();
        ctx.closePath();
    }
    for(var ring = 0; ring < gridSize - 1; ring++) {
        for(var i = 0; i < gridSize; i++) {
            ctx.beginPath();
            ctx.moveTo(points[i + ring * gridSize].x, points[i + ring * gridSize].y);
            ctx.lineTo(points[i + (ring + 1) * gridSize].x, points[i + (ring + 1) * gridSize].y);
            ctx.stroke();
            ctx.closePath();
        }
    }
    for(var i = 0; i < gridSize; i++) {
        ctx.beginPath();
        ctx.moveTo(points[i].x, points[i].y);
        ctx.lineTo(points[points.length - 1].x, points[points.length - 1].y);
        ctx.stroke();
        ctx.closePath();
    }
    for(var i = 0; i < points.length; i++) {
        ctx.beginPath();
        if(!points[i].locked) {
            ctx.arc(points[i].x, points[i].y, 3,0,2*Math.PI);
            ctx.fillStyle = "rgba(0, 0, 255, 0.3)";
        } else {
            ctx.rect(points[i].x - 3, points[i].y - 3, 6, 6);
            ctx.fillStyle = "rgba(0, 0, 255, 0.8)";
        }
        ctx.fill();
        ctx.closePath();
    }
}

function initialise(gridSize, points, height) {
    for(var ring = 0; ring < gridSize; ring++) {
        for(var i = 0; i < gridSize; i++) {
            previousHeight = height;
            var radius = height / 2;
            points[ring * gridSize + i].locked = gridSize == ring + 1;
            points[ring * gridSize + i].x = points[ring * gridSize + i].x * radius / gridSize * (ring + 1) + radius;
            points[ring * gridSize + i].y = points[ring * gridSize + i].y * radius / gridSize * (ring + 1) + radius;
        }
    }
    points[repeater.count - 1].x = radius;
    points[repeater.count - 1].y = radius;
    points[repeater.count - 1].locked = true;
}

function resize(newHeight, points) {
    var newCentroid = newHeight / 2;
    var scale = newHeight / previousHeight;
    for(var i = 0; i < points.length; i++) {
        points[i].x *= scale;
        points[i].y *= scale;
    }
    previousHeight = newHeight;
}

