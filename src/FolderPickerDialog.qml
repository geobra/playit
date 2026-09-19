import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.folderlistmodel
import QtCore
import org.kde.kirigami as Kirigami

Kirigami.Dialog {
    id: dialog

    title: "Select Folder"
    width: Kirigami.Units.gridUnit * 28
    height: Kirigami.Units.gridUnit * 36

    property url currentFolder: StandardPaths.writableLocation(StandardPaths.HomeLocation)
    property string selectedPath: ""
    signal finished(string path)

    standardButtons: Kirigami.Dialog.NoButton

    Component.onCompleted: console.log("Folder:", currentFolder)


    ColumnLayout {
        anchors.fill: parent
        spacing: Kirigami.Units.smallSpacing

        // Breadcrumb / path display
        Kirigami.Heading {
            text: currentFolder
            level: 4
            wrapMode: Text.WrapAnywhere
            Layout.fillWidth: true
        }

        // Folder list
        ListView {
            id: folderView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            model: FolderListModel {
                id: folderModel
                folder: dialog.currentFolder
                showDirs: true
                showFiles: false
                showDotAndDotDot: false
            }

            // Parent folder entry
            header: ItemDelegate {
                visible: dialog.currentFolder !== StandardPaths.writableLocation(StandardPaths.HomeLocation)
                width: ListView.view.width
                height: Kirigami.Units.gridUnit * 3
                text: ".."

                onClicked: {
                    let url = new URL(dialog.currentFolder)
                    let parts = url.pathname.split("/")
                    parts.pop()
                    let parent = parts.join("/")
                    if (parent.length === 0)
                        parent = "/"

                    dialog.currentFolder = parent
                }
            }

            delegate: ItemDelegate {
                width: ListView.view.width
                height: Kirigami.Units.gridUnit * 3

                contentItem: RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    Kirigami.Icon {
                        source: "folder"
                        width: Kirigami.Units.iconSizes.medium
                        height: width
                    }

                    Label {
                        text: fileName
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                    }
                }

                onClicked: {
                    dialog.currentFolder = fileURL
                    dialog.selectedPath = fileURL.toString().replace("file://", "")
                }
            }
        }

        // Buttons
        RowLayout {
            Layout.alignment: Qt.AlignRight
            spacing: Kirigami.Units.largeSpacing

            Button {
                text: "Cancel"
                onClicked: {
                    dialog.finished("")
                    dialog.close()
                }
            }

            Button {
                text: "OK"

                onClicked: {
                    let result = dialog.selectedPath !== "" ? dialog.selectedPath : dialog.currentFolder
                    dialog.finished(result)
                    dialog.close()
                }
            }
        }
    }
}
