import QtQuick 2.0
import QtQuick.Controls 1.1
import faith.mobile.disklayout 1.0

Rectangle {
    id: window
    width: 200
    height: 62
    clip: true

    Column
    {
        ListView
        {
            id: listView0
            width: window.width
            height: window.height - row0.height
            clip: true
            model: labModel
            delegate: testDelegate
        }
        Row
        {
            id: row0
            width: window.width
            visible: (labModel.selectedHostCount>0)
            Button
            {
                text: "Request Installation"
                implicitWidth: window.width
            }
        }
    }
    Component
    {
        id: testDelegate
        Item
        {
            id: parentItem
            width: parent.width
            height: tmp.height

            property Item tmp: null

            Component.onCompleted:
            {
                if (isParent)
                    tmp = headerDelegate.createObject(parentItem)
                else
                    tmp = insiderDelegate.createObject(parentItem)
            }
            Component.onDestruction:
            {
                if (tmp != null) tmp.destroy()
            }

            Component
            {
                id: headerDelegate
                Column
                {
                    property int prim_index: -1
                    width: parent.width

                    Rectangle
                    {
                        id: tab
                        width: parent.width
                        height: textField.height*2
                        color: hasDiskLayout ? "green" : "gray"
                        border
                        {
                            color: "black"
                            width: 1
                        }
                        MouseArea
                        {
                            anchors.fill: tab
                            onClicked:
                            {                                
                                if (isExpanded)
                                    labModel.colapse(index)
                                else
                                    labModel.expand(index)
                            }

                            width: parent.width
                            height: parent.height
                            CheckBox
                            {
                                id: checkBox1
                                width: parent.height
                                height: width
                                checked: isPartiallyChecked ? !isChecked : isChecked
                                checkedState: isPartiallyChecked ? Qt.PartiallyChecked : (isChecked ? Qt.Checked : Qt.Unchecked)
                                anchors
                                {
                                    top: parent.top
                                    left: parent.left
                                    rightMargin: parent.height * 0.1
                                    leftMargin: parent.height * 0.1
                                    topMargin: parent.height * 0.1
                                }
                                onClicked:
                                {
                                    labModel.changeCheckedLab(index, checked)
                                    checked = Qt.binding(function() {return isChecked})
                                    checkedState = Qt.binding(function() {return isPartiallyChecked ? Qt.PartiallyChecked : (isChecked ? Qt.Checked : Qt.Unchecked)})
                                }
                            }
                            Item
                            {
                                anchors
                                {
                                    top: parent.top
                                    left: checkBox1.right
                                    right: image1.left
                                    bottom: parent.bottom
                                }

                                Text {
                                    id: textField
                                    text: name
                                    anchors.centerIn: parent
                                }
                            }

                            Image {
                                id: image1
                                source: "qrc:///settings_active.png"
                                height: parent.height * 0.8
                                width: image1.height
                                anchors
                                {
                                    right: parent.right
                                    rightMargin: parent.height * 0.1
                                    leftMargin: parent.height * 0.1
                                    top: parent.top
                                    topMargin: parent.height * 0.1
                                }
                                MouseArea
                                {
                                    anchors.fill: image1
                                    onClicked:
                                    {
                                        var wnd = laboratoryConfig.createObject(window, {"laboratory": labModel.getLab(index)})
                                    }
                                }
                            }
                        }
                    }                
                }
            }

            Component
            {
                id: insiderDelegate
                Rectangle
                {                    
                    width: 0.9*parent.width
                    height: 2*textField.height
                    x: 0.05*parent.width
                    color: {
                        if (diskLayoutStatus==DiskLayout.Unknown) return "gray"
                        else if (diskLayoutStatus==DiskLayout.Ready) return "green"
                        else if (diskLayoutStatus==DiskLayout.NeedRepartition) return "orange"
                        else return "red"
                    }

                    border
                    {
                        color: "black"
                        width: 1
                    }

                    CheckBox
                    {
                        enabled: (diskLayoutStatus==DiskLayout.Ready || diskLayoutStatus==DiskLayout.NeedRepartition) ? true : false
                        id: checkBox1
                        width: parent.height
                        height: width
                        checked: isChecked
                        anchors
                        {
                            top: parent.top
                            left: parent.left
                            rightMargin: parent.height * 0.1
                            leftMargin: parent.height * 0.1
                            topMargin: parent.height * 0.1
                        }
                        onClicked:
                        {
                            labModel.changeCheckedHost(index, checked)
                            checked = Qt.binding(function() {return isChecked})
                        }
                    }
                    Item
                    {
                        anchors
                        {
                            top: parent.top
                            left: checkBox1.right
                            right: parent.right
                            bottom: parent.bottom
                        }

                        Text {
                            id: textField
                            text: name
                            anchors.centerIn: parent
                        }
                    }
                }
            }
            Component
            {
                id: laboratoryConfig                
                LaboratoryConfig
                {
                    anchors.fill: parent
                }
            }
        }
    }

}


