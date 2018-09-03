var target, targetHeight, targetWidth, targetCount, component;

function createGrid(count, height, width, parent) {
    target = parent;
    targetHeight = height;
    targetWidth = width;
    targetCount = count;
    component = Qt.createComponent("NetSquare.qml");
    for(var ring = 0; ring < targetCount; ring++) {
        for(var i = 0; i < targetCount; i++) {
            if (component.status === Component.Ready)
                finishCreation(i, ring);
            else
                component.statusChanged.connect(finishCreation);
            if(ring == 0) // only one point in the centre
                break;
        }
    }
}

function circlePoint(radius, currentPoint, totalPoints) {
    var theta = ((Math.PI * 2) / totalPoints);
    var angle = theta * currentPoint;
    return {x: radius * Math.cos(angle), y: radius * Math.sin(angle)};
}

function finishCreation(i, ring) {
    var fullRadius = targetWidth / 2
    var centreX = fullRadius - 5;
    var radius = fullRadius / targetCount * (ring ? ring + 1 : ring);
    var point = circlePoint(radius, i, targetCount);
    var arrayPosition;
    if (component.status === Component.Ready) {
        var sprite = component.createObject(target, {x: centreX + point.x,
                                                     y: centreX + point.y});
        if(!ring) {
            target.graph = {nodes: []};
        }
        target.graph.nodes.push(sprite);

        if (sprite === null) {
            // Error Handling
            console.log("Error creating object");
        }
    } else if (component.status === Component.Error) {
        // Error Handling
        console.log("Error loading component:", component.errorString());
    }
}

function getPositions(nodes) {
    var positions = []
    for(var i = 0; i < nodes.length; i++) {
        var node = nodes[i];
        var parent = node.parent;
        var x = node.x
        var y = node.y
        positions.push({x: x, y: y});
    }
    return positions;
}
