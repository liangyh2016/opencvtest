import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Window 2.12

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    property real index: 9;
    property string path: "";
    property var paths: [
        "D:/Code/opencvtest/1.jpg",
        "D:/Code/opencvtest/2.jpg",
        "D:/Code/opencvtest/3.jpg",
        "D:/Code/opencvtest/4.jpg",
        "D:/Code/opencvtest/5.jpg",
        "D:/Code/opencvtest/6.jpg",
        "D:/Code/opencvtest/7.jpg",
        "D:/Code/opencvtest/8.jpg",
        "D:/Code/opencvtest/9.jpg",
        "D:/Code/opencvtest/10.jpg",
        "D:/Code/opencvtest/11.jpg"
    ];

    Image {
        width: 200
        height: 200
        horizontalAlignment: Image.AlignHCenter
        verticalAlignment: Image.AlignVCenter
        fillMode: Image.PreserveAspectFit
        source: path ? "file:///" + path : ""
    }

    Item {
        id: can
        anchors.fill: parent

        property real ox: 100;
        property real oy: 100;
        property real sc: 0.15;

        function requestPaint() {
            c1.requestPaint();
            c2.requestPaint();
            c3.requestPaint();
        }

        CCanvas {
            id: c1
            anchors.fill: parent
            color: "#FF0000"
            ox: can.ox
            oy: can.oy
            sc: can.sc
        }

        CCanvas {
            id: c2
            anchors.fill: parent
            color: "#00FF00"
            ox: can.ox
            oy: can.oy
            sc: can.sc
        }

        CCanvas {
            id: c3
            anchors.fill: parent
            color: "#0000FF"
            ox: can.ox
            oy: can.oy
            sc: can.sc
        }

        MouseArea {
            id: ma
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            property real lastX: 0;
            property real lastY: 0;

            onWheel: {
                var oldsc = can.sc;
                var newsc = can.sc;
                if (wheel.angleDelta.y > 0) {
                    newsc += 0.05;
                } else {
                    newsc -= 0.05;
                }
                if (newsc < 0.05) newsc = 0.05;
                can.ox = wheel.x - (wheel.x - can.ox) / oldsc * newsc;
                can.oy = wheel.y - (wheel.y - can.oy) / oldsc * newsc;
                can.sc = newsc;

                can.requestPaint();
            }

            onPressed: {
                if (pressedButtons & Qt.RightButton) {
                    lastX = mouseX;
                    lastY = mouseY;
                }
            }

            onPositionChanged: {
                if (pressedButtons & Qt.RightButton) {
                    can.ox += (mouseX - lastX);
                    can.oy += (mouseY - lastY);

                    lastX = mouseX;
                    lastY = mouseY;

                    can.requestPaint();
                }
            }
        }
    }

    Button {
        width: 50
        height: 50
        onClicked: {
            path = paths[(index++)%paths.length];

            if (tool.process(path)) {
                let obj = tool.getResult();
                c1.lines = obj.src;
                c2.lines = obj.mid;
                c3.lines = obj.dst;
                can.requestPaint();
            }
        }
    }
}
