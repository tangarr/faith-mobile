import QtQuick 2.0
import QtQuick.Controls 1.1

Rectangle {
    id: window
    height: column.height
    signal showUserConfig(string username, bool usernameEnabled)
    signal showUserEdit(int index)
    property QtObject lab: null
    property bool hasRootPassword: false

    Column
    {
        spacing: 5
        id: column

        Button
        {
            id: rootPasswd
            text: hasRootPassword ? "Change root password" : "Set root password"
            onClicked:
            {
                showUserConfig("root", false)
            }
            implicitWidth: window.width
        }

        Text {
            text: "Users:" + (lab==null?"null":"")
        }
        Repeater
        {
            id: repeater0
            model: (lab!=null)?lab.userCount:0
            property int currentIndex: -1
            Rectangle
            {
                property QtObject user: (lab!=null)?lab.user(index):null
                width: window.width
                clip: true
                height: t.font.pixelSize*2

                MouseArea
                {
                    anchors.fill: parent
                    onClicked:
                    {
                        repeater0.currentIndex=index
                    }
                }
                Rectangle
                {
                    anchors.fill: parent
                    color: (repeater0.currentIndex==index)?"yellow":parent.color
                    border
                    {
                        color: "black"
                        width: 1
                    }
                }
                Text
                {
                    id: t
                    anchors.centerIn: parent
                    text: (user!=null)?user.username():""
                }
            }
        }

        Row
        {
            id: buttonRow
            property real buttonWidth: window.width/3-5
            spacing: 4
            Button
            {
                text: "Add User"
                onClicked:
                {
                    showUserConfig("", true)
                    repeater0.currentIndex = -1
                }
                implicitWidth: buttonRow.buttonWidth
            }
            Button
            {
                enabled: repeater0.currentIndex != -1
                text: "Edit User"
                onClicked:
                {
                    if (lab!=null)
                    {
                        showUserEdit(repeater0.currentIndex)
                    }
                    repeater0.currentIndex = -1
                }
                implicitWidth: buttonRow.buttonWidth
            }
            Button
            {
                enabled: repeater0.currentIndex != -1
                text: "Remove User"
                onClicked:
                {
                    if (lab!=null) lab.removeUser(repeater0.currentIndex)
                    repeater0.currentIndex = -1
                }
                implicitWidth: buttonRow.buttonWidth
            }
        }
    }
}
