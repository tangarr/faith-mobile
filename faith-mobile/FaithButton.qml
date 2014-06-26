import QtQuick 2.0

Rectangle {
    id: button
    width: 100
    height: 3*textField.height
    property string text: "Press me!"
    property bool enabled: true
    signal signalPressed()
    color: enabled ? "green" : "gray"
    gradient: Gradient {
            GradientStop { position: 0.0; color: enabled ? "lightsteelblue" : "gray" }
            GradientStop { position: 1.0; color: enabled ? "blue" : "gray"}
        }
    Text
    {
        id: textField
        anchors.centerIn: button
        text: button.text
    }
    radius: 0.1*height
    MouseArea
    {
        anchors.fill: parent

        onEntered:
        {
            flash.opacity = 0.2
        }
        onExited:
        {
            flash.opacity = 0
        }
        onClicked:
        {
            flash.opacity = 0
            signalPressed()
        }
    }
    Rectangle
    {
        id: flash
        anchors.fill: parent
        color: "white"
        radius: button.radius
        opacity: 0
    }
}
