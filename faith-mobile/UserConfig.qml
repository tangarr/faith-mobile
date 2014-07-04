import QtQuick 2.0
import QtQuick.Controls 1.1

Rectangle {
    id: window
    width: 100
    height: 62
    property alias usernameEnabled: username.enabled
    property alias usernameText: username.text
    property QtObject lab: null

    property alias userHome : homeDir.text
    property alias userHomeCreate: homeCreate.checked
    property alias userShell: shell.editText

    property int currentIndex: -1

    signal rootPasswordChanged()

    MouseArea
    {
        anchors.fill: parent
    }

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
                text: "Username: "+(currentIndex==-1 && lab!=null && lab.userExist(username.text)?"USER ALREADY EXIST":"")
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
                text:
                {
                    var t = "Password ";
                    if (password1.text=="" && currentIndex !=-1) return t+" [Leave empty for old password]"
                    if (password1.text.length<6) return t+"[Password too short]"
                    return t
                }
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
                enabledOK:
                {
                    if (currentIndex==-1)
                    {
                        if (username.text=="root")
                            return password1.text.length >= 6 && password1.text == password2.text
                        else
                            return username.text.length > 0 && password1.text.length >= 6 && password1.text == password2.text && lab!=null && !lab.userExist(username.text)
                    }
                    else
                        return (password1.text.length==0) || password1.text.length >= 6 && password1.text == password2.text
                }
                onOkClicked:
                {
                    if (lab!= null)
                    {
                        if (usernameText=="root" && !usernameEnabled)
                        {
                            rootPasswordChanged()
                            lab.setRootPassword(password1.text)
                        }
                        else if (currentIndex==-1)
                        {                            
                            lab.addUser(username.text, password1.text, shell.editText, homeDir.text)
                        }
                        else
                        {
                            lab.updateUser(currentIndex, password1.text, shell.editText, homeDir.text)
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
