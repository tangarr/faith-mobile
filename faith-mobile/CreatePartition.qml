import QtQuick 2.0
import QtQuick.Controls 1.1

Rectangle {
    width: 100
    height: 62
    id: window

    signal closeWindow()
    property QtObject disk: null

    Flickable
    {
        anchors.fill: parent
        clip: true
        contentWidth: window.width
        contentHeight: column.height

        interactive: (contentHeight > width)

        Column
        {
            id: column
            width: window.width
            spacing: 5

            Label { text: "Minimum size [Megabytes]:" }
            TextField
            {
                id: minSize
                implicitWidth: window.width
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator {bottom: 0}
                onEditingFinished:
                {
                    var min = parseInt(minSize.text)
                    var max = parseInt(maxSize.text)
                    if (max > 0 && max < min) maxSize.text = min
                }

            }
            Label { text: "Maximum size [Megabytes]:" }
            TextField
            {
                id: maxSize
                implicitWidth: window.width
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator {bottom: 0}
                onEditingFinished:
                {
                    var min = parseInt(minSize.text)
                    var max = parseInt(maxSize.text)
                    if (max==0) text=""
                    else if (max < min) text = min
                }
            }
            Label { text: "Filesystem:" }
            ComboBox
            {
                id: fstype
                implicitWidth: window.width
                model: ["ext4", "ext3", "reiserfs", "xfs", "ntfs", "vfat", "swap"]
                onCurrentIndexChanged:
                {
                    if (currentText=="swap") mountPoint.editText = "swap"
                    else
                    {
                        if (mountPoint.editText == "swap") mountPoint.editText = ""
                    }
                }
            }
            Label { text: "Mountpoint"}
            ComboBox
            {
                id: mountPoint
                implicitWidth: window.width
                model: ["-", "/", "/boot", "/home", "/usr", "/var", "/tmp"]
                enabled: (fstype.currentText=="swap") ? false : true
                editable: true
            }
            CheckBox
            {
                id: checkboxBootable
                text: "Bootable"
            }
            CheckBox
            {
                id: checkboxPreserved
                text: "Preserved"
            }
            RowOkCancel
            {
                width: window.width
                enabledOK: (parseInt(minSize.text)>0 && disk.canCreatePartition(mountPoint.editText))
                onOkClicked:
                {
                    var min = parseInt(minSize.text)
                    var max = parseInt(maxSize.text)
                    disk.createPartition(min, max, fstype.currentText, mountPoint.editText, checkboxBootable.checked, checkboxPreserved.checked)
                    closeWindow()
                }
                onCancelClicked: { closeWindow() }
            }
        }
    }

}
