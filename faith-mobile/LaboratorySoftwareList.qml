import QtQuick 2.0
import QtQuick.Controls 1.1

Rectangle {     
    height: column.height
    property QtObject lab: null
    Column
    {
        id: column
        Text {
            text: "Software List:"
        }
        CheckBox
        {
            id: base
            enabled: false
            text: "DEBIAN"
            checked: (lab!=null)?lab.containsSoft(text):false
            onCheckedChanged: { if (lab!=null) lab.changeSoft(text, checked) }
        }
        CheckBox
        {
            id: desktop
            text: "DESKTOP"
            checked: (lab!=null)?lab.containsSoft(text):false
            onCheckedChanged: { if (lab!=null) lab.changeSoft(text, checked) }
        }
        ExclusiveGroup { id: group }
        RadioButton
        {
            id: gnome
            text: "GNOME"
            enabled: desktop.checked
            checked: (lab!=null)?lab.containsSoft(text):false
            exclusiveGroup: group
            onCheckedChanged: { if (lab!=null) lab.changeSoft(text, checked) }
        }
        RadioButton
        {
            id: kde
            text: "KDE"
            enabled: desktop.checked
            exclusiveGroup: group
            checked: (lab!=null)?lab.containsSoft(text):false
            onCheckedChanged: { if (lab!=null) lab.changeSoft(text, checked) }
        }
        RadioButton
        {
            id: lxde
            text: "LXDE"
            enabled: desktop.checked
            exclusiveGroup: group
            checked: (lab!=null)?lab.containsSoft(text):false
            onCheckedChanged: { if (lab!=null) lab.changeSoft(text, checked) }
        }
        RadioButton
        {
            id: xfce
            text: "XFCE"
            enabled: desktop.checked
            exclusiveGroup: group
            checked: (lab!=null)?lab.containsSoft(text):false
            onCheckedChanged: { if (lab!=null) lab.changeSoft(text, checked) }
        }
        CheckBox
        {
            id: developerTools
            text: "DEVELOPER TOOLS"
            checked: (lab!=null)?lab.containsSoft(text):false
            onCheckedChanged: { if (lab!=null) lab.changeSoft(text, checked) }
        }
        CheckBox
        {
            id: www
            text: "WWW SERVER"
            checked: (lab!=null)?lab.containsSoft(text):false
            onCheckedChanged: { if (lab!=null) lab.changeSoft(text, checked) }
        }
    }
}
