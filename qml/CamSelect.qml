import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3

import Qt.labs.platform 1.1

Item {
    id: camSelect
    width: 250  // 显式设置宽度（根据内容调整）
    height: 40  // 显式设置高度（ComboBox 高度 + 边距）

    property var paraList
    property bool is_source0

    // 左边的 ComboBox
    ComboBox {
        id: camTypeBox
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: 15
        width: 80
        height: 35
        font.pixelSize: 12
        font.family: "Fredoka Light"
        model: ["USB", "RTSP", "VID", "DIR"]
        currentIndex: 0
        onCurrentIndexChanged: {
            // 根据选项切换 Item A 和 Item B 的可见性
            usbComboBox.visible = (currentIndex === 0);
            rtspInput.visible = (currentIndex === 1);
            fileSelect.visible = (currentIndex === 2 || currentIndex === 3)
        }
    }

    // 右边的控件
    Item {
        id: sourceComponent
        anchors.top: parent.top
        anchors.left: camTypeBox.right
        anchors.leftMargin: 10
        width:120
        height:35
        // USB 模式下的 ComboBox
        ComboBox {
            id: usbComboBox
            anchors.fill: parent
            font.pixelSize: 12
            font.family: "Fredoka Light"
            model: [0, 1, 2, 3, 4, 5]
            currentIndex: 0
            visible: true // 默认显示
            onCurrentIndexChanged: {
                if (visible === true && is_source0 === true) {
                    paraList["source0"] = displayText
                    paraList["source0_type"] = camTypeBox.displayText
                } else {
                    paraList["source1"] = displayText
                    paraList["source1_type"] = camTypeBox.displayText
                }
            }
        }

        Rectangle {
            id: rtspInput
            color: "#E8E8E8"
            anchors.fill: parent
            visible: false // 默认隐藏
            // RTSP 模式下的 TextInput
            TextInput {
                id: rtspAddress
                anchors.fill: parent // 填充整个 Rectangle
                anchors.margins: 5 // 设置边距
                verticalAlignment: Text.AlignVCenter // 垂直居中
                font.pixelSize: 12
                font.family: "Fredoka Light"
                text: qsTr("192.168.")
                onTextChanged: {
                    if (visible === true && is_source0 === true) {
                        paraList["source0"] = displayText
                        paraList["source0_type"] = camTypeBox.displayText
                    } else {
                        paraList["source1"] = displayText
                        paraList["source1_type"] = camTypeBox.displayText
                    }
                }
            }
        }

        Item{
            id: fileSelect
            anchors.fill: parent
            visible: false // 默认隐藏
            Text {
                id: fileDir
                anchors.fill: parent
                font.pixelSize: 12
                verticalAlignment: Text.AlignVCenter    // 文本垂直居中
                leftPadding: 5                          // 左侧内边距，避免文本紧贴边框
                elide: Text.ElideLeft                   // 文本过长，保留右边
                font.family: "Fredoka Light"
                text: qsTr("Path")
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
                title: camTypeBox.currentIndex === 3 ? "选择视频" : "选择序列目录"
                //selectFolder: camTypeBox.currentIndex === 3
                property string url: ""
                onAccepted: {
                    var url = networkFolderDialog.file
                    fileDir.text = url.toString().slice(7)
                    if (is_source0 === true) {
                        paraList["source0"] = displayText
                        paraList["source0_type"] = camTypeBox.displayText
                    } else {
                        paraList["source1"] = displayText
                        paraList["source1_type"] = camTypeBox.displayText
                    }
                }
                onRejected: {
                    fileDir.text = qsTr("Must provide a valid folder path...")
                }
            }
        }
    }

    Button {
       text: "Save"
       font.pixelSize: 12
       font.family: "Fredoka Light"
       width:50
       height: 35
       anchors.left: sourceComponent.right
       anchors.verticalCenter: sourceComponent.verticalCenter
       onClicked: {
           localParaList = JSON.parse(JSON.stringify(paraList));
           paraListArray.push(localParaList);
       }
    }
}
