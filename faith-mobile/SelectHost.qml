import QtQuick 2.0
import QtQuick.Controls 1.1

Rectangle
{
    id: selectHostViewBody
    width: window.width
    height: window.height

    property QtObject laboratory: null
    property int selectedIndex: -1

    signal closeWindow()

    ExclusiveGroup { id: group1 }
    Flickable
    {
        width: window.width
        height: window.height
        contentWidth: window.width
        contentHeight: selectHostViewBodyColumn.height
        interactive: (selectHostViewBodyColumn.height>height)
        Column
        {
            id: selectHostViewBodyColumn
            Repeater
            {
                model: laboratory.hostsCount()
                RadioButton
                {
                    text: laboratory.hostName(index)
                    exclusiveGroup: group1
                    onClicked:
                    {
                        selectHostViewBody.selectedIndex = index
                    }
                }
            }
            RowOkCancel
            {
                width: window.width
                onOkClicked:
                {
                    laboratory.loadDiskSchemaFromHost(index)
                    closeWindow()
                }
                onCancelClicked: { closeWindow() }
                enabledOK: (selectHostViewBody.selectedIndex>=0)
            }
        }
    }
}
