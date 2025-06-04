import QtQuick 2.15
import QtQuick.Dialogs 1.3

Item {
    id: fileSelect
    width: 250  // 显式设置宽度（根据内容调整）
    height: 40  // 显式设置高度（ComboBox 高度 + 边距）


    Text {
        id: fileDir
        width: 200
        height: 30
        anchors.leftMargin: 10
        font.pixelSize: 15
        verticalAlignment: Text.AlignVCenter    // 文本垂直居中
        leftPadding: 5                          // 左侧内边距，避免文本紧贴边框
        elide: Text.ElideLeft                   // 文本过长，保留右边
        font.family: "Fredoka Light"
        text: qsTr("Path to a directory...")

        Rectangle {
            width: parent.width
            height: parent.height
            border.width: 1
            border.color: "black"
            color: "transparent"
        }
    }

    Image {
        id: selectFile
        width: 24
        height: 24
        anchors.verticalCenter: fileDir.verticalCenter
        anchors.left: fileDir.right
        source: "qrc:/assets/file_select.png"
        anchors.leftMargin: 10
        fillMode: Image.PreserveAspectFit

        MouseArea{
            width: parent.width
            height: parent.height
            onClicked: {
                networkFolderDialog.open()
            }
        }
    }
    FileDialog {
        id: networkFolderDialog
        property string url: ""
        onAccepted: {
            var url = networkFolderDialog.file
            fileDir.text = url.toString().slice(7)
            demoContent.paraList["network_directory"] = fileDir.text
        }
        onRejected: {
            fileDir.text = qsTr("Must provide a valid folder path...")
        }
    }
}
