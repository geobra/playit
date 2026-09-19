import QtQuick 2.15
import QtQuick.Controls 2.15

Control {
    id: root

    property real from: 0
    property real to: 100
    property real value: 0

    signal seekRequested(real position)

    implicitHeight: 32
    implicitWidth: 200

    readonly property real position: (value - from) / (to - from)

    function valueFromPosition(x) {
        let ratio = Math.max(0, Math.min(1, x / width))
        return from + ratio * (to - from)
    }

    background: Item {
        anchors.fill: parent

        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width
            height: 6
            radius: 3
            color: "#888888"
        }

        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            width: root.position * parent.width
            height: 6
            radius: 3
            color: "#3daee9"
        }
    }

    Rectangle {
        id: handle
        x: root.position * (root.width - width)
        y: (root.height - height) / 2
        width: 16
        height: 16
        radius: 8
        color: "white"
        border.color: "#666"
        z: 2
    }

    MouseArea {
        anchors.fill: parent

        onPressed: function(mouse) {
            let v = root.valueFromPosition(mouse.x)
            root.value = v
            root.seekRequested(v)
        }

        onPositionChanged: function(mouse) {
            if (mouse.buttons & Qt.LeftButton) {
                let v = root.valueFromPosition(mouse.x)
                root.value = v
                root.seekRequested(v)
            }
        }
    }
}
