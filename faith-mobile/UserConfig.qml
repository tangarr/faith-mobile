import QtQuick 2.0
import QtQuick.Controls 1.1

Rectangle {
    id: window
    width: 100
    height: 62
    property alias usernameEnabled: username.enabled
    property alias usernameText: username.text
    property QtObject lab: null

    signal rootPasswordChanged()

    Flickable
    {
        width: window.width
        height: window.height
        contentWidth: window.width
        contentHeight: mainColumn.height
        interactive: mainColumn.height > window.height
        Column
        {
            id: mainColumn
            width: window.width
            spacing: 5
            Label
            {
                text: "Username:"
                visible: !(usernameText=="root" && !usernameEnabled)
            }
            TextField
            {
                id: username
                implicitWidth: window.width
                visible: !(usernameText=="root" && !usernameEnabled)
            }
            Label
            {
                text: "Password " + ((password1.text.length<6)?"[Password too short]":"")
            }
            TextField
            {
                id: password1
                echoMode: TextInput.Password
                implicitWidth: window.width
            }
            Label
            {
                text: "Repeat Password " + ((password1.text!=password2.text)?"[Password not match]":"")
            }
            TextField
            {
                id: password2
                echoMode: TextInput.Password
                implicitWidth: window.width
            }
            Label
            {
                text: "Shell"
                visible: !(usernameText=="root" && !usernameEnabled)
            }
            ComboBox
            {
                id: shell
                editable: true
                model: ["/bin/sh", "/bin/bash", "/bin/false", "/usr/sbin/nologin"]
                implicitWidth: window.width
                visible: !(usernameText=="root" && !usernameEnabled)
            }
            CheckBox
            {
                id: homeCreate
                text: "Create home directory"
                checked: true
                visible: !(usernameText=="root" && !usernameEnabled)
            }
            TextField
            {
                id: homeDir
                enabled: homeCreate.checked
                text: enabled?("/home/"+username.text):""
                implicitWidth: window.width
                visible: !(usernameText=="root" && !usernameEnabled)
            }
            RowOkCancel
            {
                width: window.width
                enabledOK: username.text.length > 0 && password1.text.length >= 6 && password1.text == password2.text
                onOkClicked:
                {
                    if (lab!= null)
                    {
                        if (usernameText=="root" && !usernameEnabled)
                        {
                            rootPasswordChanged()
                            lab.setRootPassword(password1.text)
                        }
                        else
                        {
                        }
                        window.destroy()
                    }
                }
                onCancelClicked:
                {
                    window.destroy()
                }
            }
        }
    }
}
