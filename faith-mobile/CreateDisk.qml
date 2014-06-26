import QtQuick 2.0
import QtQuick.Controls 1.1
import faith.mobile.disklabel 1.0

Rectangle {
    width: 100
    height: 62

    property QtObject laboratory: null

    signal closeWindow()

    MouseArea
    {
        anchors.fill:parent
    }

    Column
    {
        width: window.width
        Label
        {
            text: "Disk Layout:"
        }
        Column
        {
            ExclusiveGroup { id: group }
            RadioButton
            {
                id: buttonLabelMSDOS
                text: "msdos"
                exclusiveGroup:  group
                checked: true
            }
            RadioButton
            {
                id: buttonLabelGPT
                text: "gpt"
                exclusiveGroup:  group
            }
        }

        Label
        {
            width: window.width
            text: "Device:"
        }
        TextField
        {
            id: textDevName
            width: window.width
            text: {
                if (laboratory!=null)
                {
                    return laboratory.proposedNewDiskName()
                }
                else return "no laboratory selected!"
            }
        }
        Row
        {
            x: 5
            spacing: 5
            Button
            {
                implicitWidth: window.width / 2 -10
                text: "OK"
                onClicked:
                {
                    if (laboratory!=null)
                    {
                        var label = (buttonLabelMSDOS.checked)?DiskLabel.MsDos:DiskLabel.GPT
                        console.log(label)
                        var ready = laboratory.createDisk(textDevName.text, label)
                        if (ready) closeWindow()
                    }
                }
            }
            Button
            {
                implicitWidth: window.width / 2 -10
                text: "Cancel"
                onClicked:
                {
                    closeWindow()
                }
            }
        }
    }

}
