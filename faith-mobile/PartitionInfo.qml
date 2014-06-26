import QtQuick 2.0
import QtQuick.Controls 1.1

Grid
{
    id: window
    spacing: 5
    columns: 3

    property QtObject partition: null
    property real controlWidth : width/3-5

    TextField
    {
        id: minSize
        implicitWidth: controlWidth
        inputMethodHints: Qt.ImhDigitsOnly
        validator: IntValidator {bottom: 0}
        onEditingFinished:
        {
            var min = parseInt(minSize.text)
            var max = parseInt(maxSize.text)
            if (max > 0 && max < min) maxSize.text = min
            if (partition!=null)
            {
                partition.minSize=min
                partition.maxSize=max
            }
        }

    }
    TextField
    {
        id: maxSize
        implicitWidth: controlWidth
        inputMethodHints: Qt.ImhDigitsOnly
        validator: IntValidator {bottom: 0}
        onEditingFinished:
        {
            var min = parseInt(minSize.text)
            var max = parseInt(maxSize.text)
            if (max==0) text=""
            else if (max < min) text = min
            if (partition!=null)
            {
                partition.minSize=min
                partition.maxSize=max
            }
        }
    }
    CheckBox
    {
        id: checkboxPreserved
        text: "Preserved"
        onCheckedChanged:
        {
            if (partition!=null)
            {
                partition.isPreserved=checkboxPreserved.checked
            }
        }
    }
    ComboBox
    {
        id: fstype
        implicitWidth: controlWidth
        model: ["ext4", "ext3", "reiserfs", "jfs", "xfs", "ntfs", "vfat", "swap"]
        onCurrentIndexChanged:
        {
            if (currentText=="swap") mountPoint.editText = "swap"
            else
            {
                if (mountPoint.editText == "swap") mountPoint.editText = ""
            }
            if (partition!=null)
            {
                partition.fstype = fstype.currentText
            }
        }
    }
    ComboBox
    {
        id: mountPoint
        implicitWidth: controlWidth
        model: ["/", "/boot", "/home", "/usr", "/var", "/tmp", "-"]
        enabled: (fstype.currentText=="swap") ? false : true
        editable: true
        onEditTextChanged:
        {
            if (partition!=null)
            {
                partition.mountpoint = mountPoint.editText
            }
        }
    }
    CheckBox
    {
        id: checkboxBootable
        text: "Bootable"
        onCheckedChanged:
        {
            if (partition!=null)
            {
                partition.isBootable=checkboxBootable.checked
            }
        }
    }
    function setValues()
    {
        if (partition!=null)
        {
            minSize.text = partition.minSize
            maxSize.text = partition.maxSize
            checkboxPreserved.checked = partition.isPreserved
            checkboxBootable.checked = partition.isBootable
            fstype.currentIndex = fstype.find(partition.fstype)
            mountPoint.editText = partition.mountpoint
        }
        else
        {
            minSize.text = ""
            maxSize.text = ""
            checkboxPreserved.checked = false
            checkboxBootable.checked = false
            fstype.currentIndex = 0
            mountPoint.editText = ""
        }
    }
}
