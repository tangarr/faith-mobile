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
        interactive: contentHeight > height

        Column
        {
            id: column
            width: window.width
            clip: true
            spacing: 5

            Rectangle
            {
                width: window.width
                height: generalColumn.height
                id: generalSettings
                Zones { id: zoneList }

                function saveTimeZone()
                {
                    if (laboratory!=null && generalColumn.ready)
                    {
                        laboratory.setTimeZone(zone0.currentText+"/"+zone1.currentText)
                    }
                }

                Column
                {
                    spacing: 5
                    id: generalColumn
                    property bool ready: false
                    Label { text: "TimeZone" }
                    ComboBox
                    {
                        id: zone0
                        model: zoneList
                        implicitWidth: window.width
                        onCurrentTextChanged: generalSettings.saveTimeZone()
                        currentIndex: {
                            if (laboratory!=null)
                            {
                                console.out("zone0")
                                var tz = laboratory.timeZone(1);
                                var index = find(tz)
                                return index
                            }
                            else return -1
                        }
                        Component.onCompleted:
                        {
                            if (laboratory!=null)
                            {
                                var z1 = laboratory.timeZone(1)
                                var z2 = laboratory.timeZone(2)
                                var index = find(z1)
                                if (index!=-1) currentIndex = index
                                index = zone1.find(z2)
                                if (index!=-1) zone1.currentIndex = index
                                generalColumn.ready = true
                            }
                        }
                    }
                    ComboBox
                    {
                        id: zone1
                        model: zoneList.get(zone0.currentIndex).data
                        implicitWidth: window.width
                        onCurrentTextChanged: generalSettings.saveTimeZone()
                        currentIndex:  {
                            if (laboratory!=null)
                            {
                                console.out("zone1")
                                var tz = laboratory.timeZone(2);
                                var index = find(tz)
                                return index
                            }
                            else return -1
                        }
                    }
                    CheckBox
                    {
                        id: checkBoxUTC
                        text: "UTC Clock"
                        onCheckedChanged:
                        {
                            if (laboratory!=null)
                            {
                                laboratory.setUtc(checkBoxUTC.checked)
                            }
                        }
                        checked: (laboratory!=null)?laboratory.utc():false
                    }
                }
            }

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
                lab: window.laboratory
                width: window.width
            }
            LaboratoryUsersConfig
            {
                id: laboratoryUsersConfig
                width: window.width
                lab: window.laboratory
                hasRootPassword: lab.hasRootPassword()
                onShowUserConfig:
                {
                    var wnd = userConfigView.createObject(window)
                    wnd.usernameText = username
                    wnd.usernameEnabled = usernameEnabled
                }
                onShowUserEdit:
                {
                    if (lab)
                    {
                    var user = lab.user(index)
                        if (user)
                        {
                            var wnd = userConfigView.createObject(window)
                            wnd.usernameText = user.username()
                            wnd.usernameEnabled = false
                            wnd.currentIndex = index
                            wnd.userShell = user.shell()
                            if (user.homeDir()=="")
                            {
                                wnd.userHomeCreate = false
                            }
                            else
                            {
                                wnd.userHomeCreate = true
                                wnd.userHome = user.homeDir()
                            }
                        }
                    }
                }
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
                            laboratory.checkDiskLayout();
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
    Component
    {
        id: userConfigView
        UserConfig
        {
            width: window.width
            height: window.height
            lab: window.laboratory
            onRootPasswordChanged:
            {
                laboratoryUsersConfig.hasRootPassword = true
            }
        }
    }
}
