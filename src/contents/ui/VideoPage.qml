import QtQuick 2.1
import QtQuick.Controls 2.0 as Controls
import org.kde.kirigami 2.4 as Kirigami

import org.kde.plasmatube 1.0

Kirigami.Page {
    id: root

    property string url

    title: renderer.getProperty("media-title")
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    MpvObject {
        id: renderer
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent
            onClicked: {
                renderer.command(["cycle", "pause"])
            }
        }
    }

    Component.onCompleted: {
        renderer.command(["loadfile", url])
        root.title = renderer.getProperty("media-title")
    }
}
