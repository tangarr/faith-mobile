import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

ApplicationWindow {
    id: window
    visible: true
    width: 400
    height: 600
    title: qsTr("Faith-Mobile")

    Component.onCompleted:
    {
        server_address.text = applicationConfig.server_address
        if (applicationConfig.server_port != 0) server_port.text = applicationConfig.server_port
    }
    ColumnLayout
    {
        width: parent.width

        Image {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            source: "qrc:///logo.png"
        }

        Label {
            text: qsTr("Adres serwera:")
        }
        TextField {
            id: server_address
            anchors.right: parent.right
            anchors.left: parent.left
            placeholderText: qsTr("Text Field")
        }
        Label {
            text: qsTr("Port serwera:")
        }
        TextField {
            id: server_port
            anchors.right: parent.right
            anchors.left: parent.left
            placeholderText: qsTr("Text Field")
            inputMethodHints: Qt.ImhDigitsOnly
        }
        Button
        {
            anchors.right: parent.right
            anchors.left: parent.left
            text: "Połącz"
            onClicked:
            {
                applicationConfig.server_address = server_address.text
                applicationConfig.server_port = server_port.text                
                if (applicationManager.updateConfig())
                {
                    var wnd = hostListView.createObject(window, {width:window.width, height:window.height})
                }
                else
                {
                    console.log("jakis blad")
                }
            }
        }
    }
    Component
    {
        id: hostListView
        MouseArea
        {
            anchors.fill: parent
            HostListView
            {
                anchors.fill: parent
            }
        }
    }
}
