import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

Rectangle {
    id: window
    height: column.height
    property QtObject laboratory: null

    signal buttonNewLayoutPressed()
    signal buttonLoadLayoutPressed()
    signal buttonNewDiskPressed()
    signal buttonNewPartitionPressed()

    property int selectedDisk: -1
    property int selectedPartition: -1

    Column
    {
        id: column
        Text {
            id: labelTitle
            text: "Disk Layout:"
        }
        Column
        {
            id: partitionLayoutView

            Repeater
            {
                model: laboratory.diskCount
                Rectangle
                {
                    id: diskView
                    property int diskIndex: -1                    
                    property QtObject disk: laboratory.disk(diskView.diskIndex)

                    width: window.width
                    height: labelTitle.height*3
                    color: (window.selectedDisk==index && window.selectedPartition == -1)?"yellow":"gray"
                    border
                    {
                        color: "black"
                        width: 1
                    }
                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked:
                        {
                            window.selectedDisk = index
                            window.selectedPartition = -1
                        }
                    }

                    Row
                    {
                        x: 10
                        Repeater
                        {
                            id: partitionView
                            property int partitionCount: (diskView.disk != null) ? diskView.disk.partitionCount : 0
                            model: partitionView.partitionCount

                            Rectangle
                            {                                
                                property QtObject currentPartition: (diskView.disk != null) ? diskView.disk.partition(index) : null
                                width: (diskView.width-20)/partitionView.partitionCount
                                height: diskView.height
                                color: (diskIndex==window.selectedDisk && (index==window.selectedPartition || window.selectedPartition==-1))?"yellow":"white"
                                border.color: "black"
                                border.width: 1
                                clip: true

                                Column
                                {
                                    anchors.fill: parent
                                    Text {
                                        text: {
                                            if (currentPartition!=null)
                                            {
                                                if (currentPartition.isBootable) return currentPartition.name+"*"
                                                else return currentPartition.name
                                            }
                                            else return ""
                                        }
                                    }
                                    Text
                                    {
                                        text: (currentPartition!=null)?("("+ currentPartition.fstype+")"):""
                                    }
                                    Text
                                    {
                                        text: (currentPartition!=null)?(currentPartition.mountpoint):""
                                    }

                                }
                                MouseArea
                                {
                                    anchors.fill: parent
                                    onClicked:
                                    {
                                        window.selectedDisk = diskIndex
                                        window.selectedPartition = index
                                        partitionInfo.setValues()
                                    }
                                }                    
                            }                        
                        }
                    }
                    Component.onCompleted:
                    {
                        diskIndex = index
                    }
                }
            }
        }
        Column
        {
            id: diskInfo
            width: window.width

            property QtObject disk : laboratory.disk(window.selectedDisk)

            Column
            {
                width: window.width
                visible: (window.selectedDisk!=-1 && window.selectedPartition==-1)
                spacing: 5
                Text {text: (diskInfo.disk!=null)?diskInfo.disk.devName():"disk not selected" }
                Text {text: "minimum size: "+((diskInfo.disk!=null)?diskInfo.disk.minimumSize:"0")}
            }
            PartitionInfo
            {
                id: partitionInfo
                partition: (diskInfo.disk!=null)?diskInfo.disk.partition(window.selectedPartition):null
                width: window.width
                visible: (window.selectedDisk!=-1 && window.selectedPartition!=-1)
            }
        }
        GridLayout
        {
            id: buttonGrid
            Layout.fillWidth: true
            property real buttonWidth: window.width/3-5
            columns: 3
            Button {
                text: "New Layout"
                implicitWidth: buttonGrid.buttonWidth
                onClicked:
                {
                    laboratory.removeDisks()
                }
            }
            Button {
                text: "Add Disk"
                implicitWidth: buttonGrid.buttonWidth
                onClicked: { buttonNewDiskPressed() }
            }
            Button {
                text: "Add Partition"
                implicitWidth: buttonGrid.buttonWidth
                enabled: (window.selectedDisk!=-1) ? true : false
                onClicked: { buttonNewPartitionPressed() }
            }
            Button {
                text: "Load from Host"
                implicitWidth: buttonGrid.buttonWidth
                onClicked:
                {
                    buttonLoadLayoutPressed()
                }
            }
            Button {
                text: "Remove Disk"
                implicitWidth: buttonGrid.buttonWidth
                enabled: (window.selectedDisk!=-1 && window.selectedPartition==-1) ? true : false
                onClicked:
                {
                    laboratory.removeDisk(window.selectedDisk)
                    window.selectedPartition = -1
                    window.selectedDisk = -1
                }
            }
            Button {
                text: "Remove Partition"
                implicitWidth: buttonGrid.buttonWidth
                enabled: (window.selectedPartition!=-1) ? true : false
                onClicked:
                {
                    laboratory.removePartition(window.selectedDisk, window.selectedPartition)
                    window.selectedPartition = -1
                }
            }
        }
    }   
}
