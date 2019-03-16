import QtQuick 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0 as Controls
import org.kde.kirigami 2.4 as Kirigami

Kirigami.ApplicationWindow {
    id: root
    pageStack.initialPage: mainPageComponent

    Component {
        id: videoPageComponent
        VideoPage {}
    }

    Component {
        id: mainPageComponent
        Kirigami.Page {
            title: "Search"

            RowLayout {
                Controls.TextField {
                    id: linkField
                }
                Controls.Button {
                    text: "Play"
                    onPressed: pageStack.push(videoPageComponent, {"url": linkField.text})
                }
            }
        }
    }
}
