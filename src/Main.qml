import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import QtQml.Models 2.15

// common icons
// https://specifications.freedesktop.org/icon-naming/latest/

Kirigami.ApplicationWindow {
    id: root
    title: "PlayIt"

    property bool playRunning: false
    property bool playPause: false
    property int currentPosSecond: 0
    property int observedActiveIndex: -1

    Component.onCompleted: {
        Qt.callLater(() => flick.scrollToItem(customModel.getActiveIndex()))

        observedActiveIndex = customModel.getActiveIndex()
    }

    Connections {
        target: media
        function onPositionChanged(position) {
            positionSlider.value = position;
        }
    }

    Connections {
        target: media
        function onDurationChanged(duration) {
            positionSlider.to = duration;
        }
    }

    Connections {
        target: customModel
        function onDataChanged() {
            Qt.callLater(() => {
                var activeIndex = customModel.getActiveIndex()
                if (activeIndex !== root.observedActiveIndex) {
                    root.observedActiveIndex = activeIndex
                    if (activeIndex >= 0 && !flick.isItemVisible(activeIndex)) {
                        flick.scrollToItem(activeIndex)
                    }
                }
            })
        }
    }


    FolderDialog {
        id: folderDialog
        title: "Please choose a folder"
        currentFolder: settings.CurrentPath
       // selectFolder: true // This sets it to folder mode

        onAccepted: {
            console.log("Selected folder: " + folderDialog.selectedFolder)
            folderDialog.close()
            media.scanDirectory(folderDialog.selectedFolder)
        }
        onRejected: {
            folderDialog.close()
        }
    }

    Kirigami.Dialog {
        id: speed
        title: i18nc("@title:window", "Select Playback Speed")
        standardButtons: Kirigami.Dialog.Ok
        padding: Kirigami.Units.largeSpacing
        preferredWidth: Kirigami.Units.gridUnit * 20
        implicitHeight: parent.height / 2

        ColumnLayout {
             anchors.fill: parent
             spacing: Kirigami.Units.smallSpacing

             Repeater {
                 model: ["1.0", "1.25", "1.5", "1.75", "2.0"]

                 delegate: Controls.RadioButton {
                     text: modelData
                     checked: index === settings.CurrentPlaybackSpeed
                     Layout.fillWidth: true

                     onClicked: {
                         //console.log("Selected:", modelData)
                         media.setPlaybackSpeed(modelData)
                     }
                 }
             }
         }
    }

    globalDrawer: Kirigami.GlobalDrawer {
        isMenu: true
        actions: [
            Kirigami.Action {
                text: i18n("Quit")
                icon.name: "application-exit-symbolic"
                onTriggered: Qt.quit()
            },
            Kirigami.Action {
                text: i18n("Clear")
                icon.name: "user-trash-full"
                onTriggered: {
                    media.stop()
                    playRunning = false
                    playPause = false
                    customModel.clearData()
                }
            },
            Kirigami.Action {
                text: "Playback Speed"
                onTriggered: speed.open()
            }
        ]
    }

    pageStack.initialPage: Kirigami.Page {
        actions: [
            Kirigami.Action {
                id: dirChooser
                icon.name: "folder"
                text: ""
                onTriggered: {
                    folderDialog.open();
                }
            },
            Kirigami.Action {
                icon.name: "audio-speakers"
                text: i18n("Output Device")
                onTriggered: outputDeviceDialog.open()
            }
        ]


        Kirigami.Dialog {
            id: outputDeviceDialog
            title: i18nc("@title:window", "Select Audio Output")
            standardButtons: Kirigami.Dialog.Cancel
            padding: Kirigami.Units.largeSpacing
            preferredWidth: Kirigami.Units.gridUnit * 20
            implicitHeight: parent.height / 2

            contentItem: ColumnLayout {
                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: audioOutputDevices

                    delegate: Controls.ItemDelegate {
                        required property int index
                        required property string deviceName
                        required property bool isDefault
                        required property bool isSelected

                        width: ListView.view.width
                        text: deviceName + (isDefault ? " (" + i18n("Default") + ")" : "")
                        highlighted: isSelected
                        onClicked: {
                            audioOutputDevices.selectDevice(index)
                            outputDeviceDialog.close()
                        }
                    }
                }

                Controls.Label {
                    Layout.fillWidth: true
                    visible: audioOutputDevices.count === 0
                    text: i18n("No audio output devices found")
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            // crollable area (80%)
            Flickable {
                id: flick
                Layout.fillWidth: true
                Layout.preferredHeight: parent.height * 0.8
                contentWidth: width
                clip: true

                // Function to scroll to a specific index
                function scrollToItem(index) {
                    console.log("scrollToItem " + index);
                    var targetItem = mediaRepeater.itemAt(index)
                    if (targetItem) {
                        // Map the item's top-left corner (0,0) to the Flickable's contentItem
                        var mappedPoint = targetItem.mapToItem(flick.contentItem, 0, 0)

                        console.log("got the mediaRepeaterItem! " + mappedPoint.y)

                        // Assign the Y coordinate to contentY (bounds behavior applies automatically)
                        flick.contentY = mappedPoint.y
                    }
                }

                function isItemVisible(index) {
                    var targetItem = mediaRepeater.itemAt(index)
                    if (!targetItem) {
                        return false
                    }

                    var mappedPoint = targetItem.mapToItem(flick.contentItem, 0, 0)
                    return mappedPoint.y + targetItem.height > flick.contentY
                            && mappedPoint.y < flick.contentY + flick.height
                }

                ColumnLayout {
                    //anchors.fill: parent
                    //spacing: 0
                    id: contentColumn
                    width: flick.width

                    Repeater {
                        id: mediaRepeater
                        model: customModel
                        delegate: Kirigami.AbstractCard {
                            onClicked: {
                                //console.log(model.FilePath)
                                //console.log("cm: " + settings.CurrentMedia);

                                media.playFile(model.FilePath)
                                playRunning = true
                                playPause = false
                            }

                            //Layout.fillHeight: true
                            header: Kirigami.Heading {
                                text: model.Filename
                                level: 2
                                color: {
                                    if (model.Active) {
                                        return "blue";
                                    } else {
                                        return "green";
                                    }
                                }
                            }
                            contentItem: Item {
                                implicitWidth: delegateLayout.implicitWidth
                                implicitHeight: delegateLayout.implicitHeight
                                ColumnLayout {
                                    id: delegateLayout
                                    Controls.Label {
                                        text: model.Artist
                                    }
                                    Controls.Label {
                                        text: model.Title
                                    }
                                    Controls.Label {
                                        text: model.Duration + "s"
                                    }
                                }
                            }
                        }
                    }
                }
                contentHeight: contentColumn.height
            }

            // Bottom area (20%)

            RowLayout {
                Layout.fillWidth: true

                Controls.Button {
                    text: "Play"
                    icon.name: "media-playback-start"
                    enabled: !playRunning
                    onClicked: {
                        playRunning = true
                        media.play();
                    }
                }
                Controls.Button {
                    text: "Pause"
                    icon.name: "media-playback-pause"
                    enabled: playRunning && !playPause
                    onClicked: {
                        playPause = true
                        media.pause();
                    }
                }
                Controls.Button {
                    text: "Stop"
                    icon.name: "media-playback-stop"
                    enabled: playRunning
                    onClicked: {
                        playRunning = false;
                        media.stop();
                        playPause = false;
                    }
                }
                Controls.Button {
                    text: "Resume"
                    icon.name: "media-seek-forward"
                    enabled: playPause
                    onClicked: {
                        playPause = false
                        media.resume();
                    }
                }
            }

            SeekBar {
                id: positionSlider
                Layout.fillWidth: true
                from: 0
                to: 0
                value: settings.CurrentPosition

                onSeekRequested: function(pos) {
                    media.seek(pos)
                }
            }
            Controls.Label {
                text: "Position: " + Math.round(positionSlider.value) + "s / " + positionSlider.to + "s"
            }
        }

    }
}
