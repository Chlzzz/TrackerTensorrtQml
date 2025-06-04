import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: camSelect
    width: 250  // 显式设置宽度（根据内容调整）
    height: 40  // 显式设置高度（ComboBox 高度 + 边距）

    // 左边的 ComboBox
    ComboBox {
        id: camTypeBox
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: 15
        width: 90
        height: 35
        font.pixelSize: 15
        font.family: "Fredoka Light"
        model: ["USB", "RTSP"]
        currentIndex: 0
        onCurrentIndexChanged: {
            // 根据选项切换 Item A 和 Item B 的可见性
            usbComboBox.visible = (currentIndex === 0);
            rtspInput.visible = (currentIndex === 1);
        }
    }

    // 右边的控件
    Item {
        anchors.top: parent.top
        anchors.left: camTypeBox.right
        anchors.leftMargin: 15
        width:120
        height:35
        // USB 模式下的 ComboBox
        ComboBox {
            id: usbComboBox
            anchors.fill: parent
            font.pixelSize: 15
            font.family: "Fredoka Light"
            model: [0, 1, 2, 3, 4, 5]
            currentIndex: 0
            visible: true // 默认显示
            onCurrentIndexChanged: {
                if (visible === true) {
                    demoContent.paraList["cam_device"] = displayText
                    demoContent.paraList["camera_type"] = camTypeBox.displayText
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
                font.pixelSize: 15
                font.family: "Fredoka Light"
                text: qsTr("192.168.")
                onTextChanged: {
                    if (visible === true) {
                        demoContent.paraList["cam_device"] = text
                        demoContent.paraList["camera_type"] = camTypeBox.displayText
                    }
                }
            }
        }
    }
}
