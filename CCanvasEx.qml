import QtQuick 2.0

Item {
    id: root
    property var liness: [];
    property string color: "#000000";

    property real ox: 100;
    property real oy: 100;
    property real sc: 0.15;

    property var colors: [
        "#87CEFA",
        "#00CED1",
        "#006400",
        "#20B2AA",
        "#00FF00",
        "#EEE8AA",
        "#CD5C5C",
        "#F5DEB3",
        "#FA8072",
        "#FF4500",
        "#FF00FF",
        "#9932CC",
        "#FFD700",
        "#7CCD7C",
        "#8B658B",
        "#8B5A2B",
        "#FF6EB4",
        "#FFDAB9",
        "#FFE4E1",
        "#191970"
    ]

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

            for (var i = 0; i < liness.length; ++i) {
                let lines = liness[i];

                ctx.strokeStyle = colors[i];
                ctx.beginPath();
                for (var j = 0; j < lines.length; ++j) {
                    let line = lines[j];
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
}

