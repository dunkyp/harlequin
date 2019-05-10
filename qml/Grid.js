var points = [];
var centroid;
var component;
var currentResolution;
var lockedPointColour = "rgba(0, 0, 256, 1)";
var selectedPointColour = "rgba(256, 256, 256, 1)";
var pointColour = "rgba(180, 180, 180, 0.6)";
var lineColour = "black";
var pointSize = 10;
var currentColour = null;

function initialise(parent, resolution) {
    currentResolution = resolution;
    component = Qt.createComponent("Point.qml");
    for(var ring = 0; ring < currentResolution; ring++) {
        if(ring === 0) {
            centroid = component.createObject(parent, {
                index: -1,
                unscaledX: 0.5,
                unscaledY: 0.5,
                locked: true
            });
        } else {
            for(var i = 0; i < currentResolution; i++) {
                let r = 0.5 / (currentResolution - 1) * ring;
                let theta = (Math.PI * 2) / currentResolution * i
                points.push(component.createObject(parent, {
                    index: points.length,
                    startR: r,
                    startTheta: theta,
                    locked: ring === (currentResolution - 1),
                    unscaledX: r * Math.cos(theta) + 0.5,
                    unscaledY: r * Math.sin(theta) + 0.5
                }));
            }
        }
    }
    rescale();
}

function rescale() {
    if(centroid) {
        let scaleFactor = (centroid.parent.width - pointSize);
        [centroid].concat(points).forEach(point => {
            point.x = point.unscaledX * scaleFactor + pointSize / 2;
            point.y = point.unscaledY * scaleFactor + pointSize / 2;
        });
    }
}

function cartToPolar(i) {
    let point = i > -1 ? points[i] : centroid;
    if(point.locked) {
        let scaleFactor = point.parent.width - pointSize;
        let x = point.x - (scaleFactor / 2);
        let y = point.y - (scaleFactor / 2);
        let distanceFromCentre = Math.sqrt(Math.pow(x, 2) + Math.pow(y, 2))
            / (scaleFactor / 2);
        if(distanceFromCentre <= 1) {
            point.unscaledX = (point.x - pointSize / 2) / scaleFactor;
            point.unscaledY = (point.y - pointSize / 2) / scaleFactor;
        } else {
            var angle = Math.atan2(y, x);
            point.unscaledX = Math.cos(angle) / 2 + 0.5;
            point.unscaledY = Math.sin(angle) / 2 + 0.5;
        }
    }
    let lines = i > -1 ? [i % currentResolution] : Array.from({length: currentResolution}, (_, k) => k);
    lines.forEach(line => {
        let startRing = -1;
        for(let ring = 0; ring < (currentResolution - 1); ring++) {
            let endPoint = ring * currentResolution + line;
            if(!points[endPoint].locked)
                continue;
            let startPoint;
            if(startRing == -1) {
                startPoint = centroid;
            } else {
                startPoint = points[startRing * currentResolution + line];
            }
            let unscaledXIncrements = (points[endPoint].unscaledX - startPoint.unscaledX) / (ring - startRing);
            let unscaledYIncrements = (points[endPoint].unscaledY - startPoint.unscaledY) / (ring - startRing);
            for(var innerRing = ring - 1; innerRing > startRing; innerRing--) {
                points[innerRing * currentResolution + line].unscaledX = unscaledXIncrements * (innerRing - startRing) + startPoint.unscaledX;
                points[innerRing * currentResolution + line].unscaledY = unscaledYIncrements * (innerRing - startRing) + startPoint.unscaledY;
            }
            startRing = ring;
        }
    });
    rescale();
}

function centroidMoved() {
    cartToPolar(-1);
    rescale();
}


function paint() {
    var ctx = canvas.getContext('2d');
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.strokeStyle = lineColour;
    ctx.beginPath();
    for(var i = 0; i < currentResolution; i++) {
        ctx.moveTo(centroid.x, centroid.y);
        ctx.lineTo(points[i].x, points[i].y);
    }
    for(var ring = 0; ring < currentResolution - 2; ring++) {
        for(var i = 0; i < currentResolution; i++) {
            var inner = points[ring * currentResolution + i];
            var outer = points[(ring + 1) * currentResolution + i];
            ctx.moveTo(inner.x, inner.y);
            ctx.lineTo(outer.x, outer.y);
        }
    }
    for(var ring = 0; ring < currentResolution - 1; ring++) {
        for(var i = 0; i < currentResolution; i++) {
            var start = points[ring * currentResolution + i];
            var stop = points[ring * currentResolution + ((i + 1) % currentResolution)];

            ctx.moveTo(start.x, start.y);
            ctx.lineTo(stop.x, stop.y);
        }
    }
    ctx.stroke();
    ctx.closePath();

    points.forEach(point=> {
        if(!point.locked) {
            ctx.fillStyle = pointColour;
            ctx.beginPath();
            ctx.arc(point.x, point.y, pointSize / 2, 0, Math.PI * 2, true);
            ctx.fill();
            ctx.closePath();
        }
        if(point.locked) {
            ctx.strokeStyle = lockedPointColour;
            ctx.clearRect(point.x - pointSize / 2, point.y - pointSize / 2, pointSize, pointSize);
            ctx.strokeRect(point.x - pointSize / 2, point.y - pointSize / 2, pointSize, pointSize);
            if(point.selected) {
                ctx.strokeStyle = selectedPointColour;
                ctx.strokeRect(point.x - pointSize / 2 - 1, point.y - pointSize / 2 - 1, pointSize + 2, pointSize + 2);
            }
        }
    });
    ctx.strokeStyle = lockedPointColour;
    ctx.clearRect(centroid.x - pointSize / 2, centroid.y - pointSize / 2, pointSize, pointSize);
    ctx.strokeRect(centroid.x - pointSize / 2, centroid.y - pointSize / 2, pointSize, pointSize);
    if(centroid.selected) {
        ctx.strokeStyle = selectedPointColour;
        ctx.strokeRect(centroid.x - pointSize / 2 - 1, centroid.y - pointSize / 2 - 1, pointSize + 2, pointSize + 2);
    }
}
