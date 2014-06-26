import QtQuick 2.0
import QtQuick.Controls 1.1

Row
{
    id: window
    x: 5
    spacing: 5

    property alias enabledOK: buttonOK.enabled
    property alias enabledCanel: buttonCancel.enabled

    signal okClicked()
    signal cancelClicked()

    Button
    {
        id: buttonOK
        implicitWidth: window.width / 2 -10
        text: "OK"
        onClicked:
        {
            okClicked()
        }
    }
    Button
    {
        id: buttonCancel
        implicitWidth: window.width / 2 -10
        text: "Cancel"
        onClicked:
        {
            cancelClicked()
        }
    }
}

