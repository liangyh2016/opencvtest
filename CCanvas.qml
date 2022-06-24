import QtQuick 2.0

Item {
    id: root
    property var lines: [];
    property string color: "#000000";

    property real ox: 100;
    property real oy: 100;
    property real sc: 0.15;

    function translatePt(pt) {
        return Qt.point(ox + pt.x*sc, oy + pt.y*sc);
    }

    function requestPaint() {
        can.requestPaint();
    }

    Canvas {
        id: can
        anchors.fill: parent

        onPaint: {
            let ctx = can.getContext("2d");
            ctx.clearRect(0, 0, can.width, can.height);

            ctx.lineWidth = 1;
            ctx.font = "10px sans-serif";

            ctx.strokeStyle = color;
            ctx.beginPath();
            for (var i = 0; i < lines.length; ++i) {
                let line = lines[i];
                let pt1 = Qt.point(line[0], line[1]);
                let pt2 = Qt.point(line[2], line[3]);

                pt1 = translatePt(pt1);
                pt2 = translatePt(pt2);

                ctx.moveTo(pt1.x, pt1.y);
                ctx.lineTo(pt2.x, pt2.y);
            }
            ctx.stroke();
        }
    }
}

