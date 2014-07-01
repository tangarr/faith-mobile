import QtQuick 2.0
import QtQuick.Controls 1.1

Rectangle {
    id: window
    height: column.height
    signal showUserConfig(string username, bool usernameEnabled)
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
            text: "Users:"
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
                }
                implicitWidth: buttonRow.buttonWidth
            }
            Button
            {
                enabled: false
                text: "Edit User"
                onClicked:
                {
                    showUserConfig("", true)
                }
                implicitWidth: buttonRow.buttonWidth
            }
            Button
            {
                enabled: false
                text: "Remove User"
                onClicked:
                {

                }
                implicitWidth: buttonRow.buttonWidth
            }
        }
    }
}
