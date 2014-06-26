import QtQuick 2.0
import QtQuick.Controls 1.1

Rectangle {
    id: window
    width: 100
    height: 62
    property QtObject laboratory: null
    Flickable
    {
        anchors.fill: window
        contentWidth: window.width
        contentHeight: column.height
        clip: true
        interactive: true

        Column
        {
            id: column
            width: window.width
            clip: true

            LaboratoryDiskLayout
            {
                width: window.width
                laboratory: window.laboratory
                onButtonNewDiskPressed:
                {
                    createDiskView.createObject(window, {"laboratory":laboratory})
                }
                onButtonNewPartitionPressed:
                {
                    console.log(laboratory)
                    console.log(selectedDisk)
                    console.log(laboratory.disk(selectedDisk))
                    createPartitionView.createObject(window, {"disk":laboratory.disk(selectedDisk)})
                }
                onButtonLoadLayoutPressed:
                {
                    selectHostView.createObject(window)
                }
            }
            LaboratorySoftwareList
            {
                width: window.width
                //laboratory: window.laboratory
            }
            LaboratoryUsersConfig
            {
                width: window.width
                //laboratory: window.laboratory
            }
            Row
            {
                x: 5
                spacing: 5
                id: buttonsRow
                property real buttonsWidth: window.width/2-10
                Button
                {
                    id: buttonSave
                    text: "Save config"
                    implicitWidth: buttonsRow.buttonsWidth
                    onClicked:
                    {
                        if (laboratory.writeConfiguration())
                        {
                            laboratory.backupDelete();
                            window.destroy()
                        }
                    }
                }
                Button
                {
                    id: buttonCancel
                    text: "Cancel"
                    implicitWidth: buttonsRow.buttonsWidth
                    onClicked:
                    {
                        laboratory.backupRevert();
                        laboratory.backupDelete();
                        window.destroy();
                    }
                }
            }
        }
        Component.onCompleted:
        {
            laboratory.backupCreate();
        }

    }
    Component
    {
        id: createDiskView
        CreateDisk
        {
            width: window.width
            height: window.height
            onCloseWindow: { destroy() }
        }
    }
    Component
    {
        id: createPartitionView
        CreatePartition
        {
            width: window.width
            height: window.height
            onCloseWindow: { destroy() }
        }
    }
    Component
    {
        id: selectHostView
        SelectHost
        {
            width: window.width
            height: window.height
            laboratory: window.laboratory
            onCloseWindow: { destroy() }
        }

    }
}
