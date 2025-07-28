import QtQuick 2.15
import QtCharts 2.15
import QtQuick.Controls 2.15

import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import Qt.labs.platform 1.1

Item {
    id: item1
    width: 1600
    height: 800

    Rectangle {
        id: demoContent
        x: 0
        y: 0
        width: parent.width
        height: parent.height
        radius: 20
        color: "transparent"

        property var paraList: {
            "infer_device": "Nvidia GPU",
            "task_type": "MOT",
            "network": ""
        }

        // 存储所有Camera信息的模型
        ListModel {
            id: maincameraModel
        }

        function stringifyAndSend(paraList, caminfo){
            var jsonString = JSON.stringify(paraList)
            console.log(jsonString)

            var cameraData = [];
            for (var i = 0; i < caminfo.count; i++) {
                var item = caminfo.get(i);
                cameraData.push({
                    type: item.type,       // 访问 Q_PROPERTY
                    cameraId: item.cameraId
                });
            }
            var jsonStr = JSON.stringify(cameraData);
            console.log(jsonStr)

            utility.parseJSValue(paraList, cameraData)
        }


        Rectangle{
            id: canvas
            width: 1300
            height: 500
            color: "transparent"
            border.color: "black"
            border.width: 3
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 5

            Image {
                id: imageRGB
                width: 640
                height: 480
                source: "qrc:/assets/videocam.png"
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                fillMode: Image.PreserveAspectFit
                cache: false
                asynchronous: false
                sourceSize.width: 640
                sourceSize.height: 480
                // Reload image
                function reload(){
                    source = ""
                    source = "image://live/image0/" + Date.now()
                }
                // Set default image
                function setDefault(){
                    source = "qrc:/assets/videocam.png"
                }

//                RoiSetting {
//                    id: roiset
//                }
            }

            Image {
                id: imageIFR
                width: 640
                height: 480
                source: "qrc:/assets/videocam.png"
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: imageRGB.right
                anchors.leftMargin: 15
                fillMode: Image.PreserveAspectFit
                cache: false
                asynchronous: false
                sourceSize.width: 640
                sourceSize.height: 480
                // Reload image
                function reload(){
                    source = ""
                    source = "image://live/image1/" + Date.now()
                }
                // Set default image
                function setDefault(){
                    source = "qrc:/assets/videocam.png"
                }
            }
        }


        Rectangle {
            id: controlCamPanel
            height: 120
            color: "#ffffff"
            anchors.left: canvas.right
            anchors.leftMargin: 15
            anchors.right: parent.right
            anchors.top: canvas.top


            property var cameraForms: []

            ColumnLayout {
                anchors.fill: parent
                spacing: 10

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    Layout.leftMargin: 15

                    Label {
                        text: "Source Info"
                        font.pixelSize: 18
                        Layout.alignment: Qt.AlignLeft
                    }

                    Button {
                        text: "Add"
                        onClicked: controlCamPanel.addCameraInfo()
                        Layout.alignment: Qt.AlignRight
                    }

                    Button {
                        text: "Save"
                        onClicked: controlCamPanel.saveCameraInfo()
                        Layout.alignment: Qt.AlignRight
                    }
                }

                ScrollView {
                    id: mainScrollView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    Column {
                        id: formsContainer
                        width: mainScrollView.width
                        spacing: 10
//                        padding: 10
                    }
                }
            }

            // 添加新的Camera信息表单(ListView)
            function addCameraInfo() {
                var component = Qt.createComponent("CameraForm.qml")
                if (component.status === Component.Ready) {
                    var newForm = component.createObject(formsContainer, {
                        "width": formsContainer.width - 20
                    })
                     cameraForms.push(newForm)
                } else {
                    console.error("Error creating component:", component.errorString())
                }
            }

            function saveCameraInfo() {
                // 清空主模型
                maincameraModel.clear()

                cameraForms.forEach(function(form) {
                    maincameraModel.append({
                        "type": form.cameraType,
                        "cameraId": form.cameraId,
                    })
                })

            }

            Component.onCompleted: {
                // 初始添加一个表单
                addCameraInfo()
            }

         }

         Rectangle {
            id: controlModelPanel
            height: 400
            color: "#ffffff"
            anchors.top: controlCamPanel.bottom
            anchors.topMargin: 15
            anchors.left: canvas.right
            anchors.leftMargin: 15
            anchors.right: parent.right


            Text {
                id: inferDeviceText
                text: qsTr("Device")
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.topMargin: 20
                font.pixelSize: 15
                font.family: "Fredoka Light"
                anchors.leftMargin: 15
            }

            ComboBox {
                id: inferDevice
                width: 130
                height:35
                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: inferDeviceText.verticalCenter
                font.pixelSize: 14
                font.family: "Fredoka Light"
                model: ["Nvidia GPU", "Intel CPU"]
                onDisplayTextChanged: {
                    demoContent.paraList["infer_device"] = displayText
                }
            }

            Text {
                id: modelTypeText
                text: qsTr("Task Type")
                anchors.left: parent.left
                anchors.top: inferDeviceText.bottom
                font.pixelSize: 15
                font.family: "Fredoka Light"
                anchors.leftMargin: 15
                anchors.topMargin: 40
            }

            ComboBox {
                id: modelTypeSelect
                width: 130
                height:35
                anchors.verticalCenter: modelTypeText.verticalCenter
                anchors.horizontalCenter: inferDevice.horizontalCenter
                font.pixelSize: 15
                font.family: "Fredoka Light"
                model: ["MOT", "VOT", "FUSION"]
                 onDisplayTextChanged: {
                     demoContent.paraList["task_type"] = displayText
                 }
            }

            Text {
                id: networkText
                text: qsTr("Network file directory")
                anchors.left: parent.left
                anchors.top: modelTypeText.bottom
                font.pixelSize: 15
                font.family: "Fredoka Light"
                anchors.leftMargin: 15
                anchors.topMargin: 40
            }

            Text {
                id: networkDir
                width: 220
                height: 30
                anchors.left: parent.left
                anchors.top: networkText.bottom
                anchors.leftMargin: 10
                anchors.topMargin: 15
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
                anchors.verticalCenter: networkDir.verticalCenter
                anchors.left: networkDir.right
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
                    networkDir.text = url.toString().slice(7)
                    demoContent.paraList["network_directory"] = networkDir.text
                }
                onRejected: {
                    networkDir.text = qsTr("Must provide a valid folder path...")
                }
            }


            Button {
                id: saveModelParaBtn
                text: qsTr("Save and init Engine")
                anchors.top: selectFile.bottom
                font.family: "Fredoka Light"
                font.pixelSize: 12
                highlighted: true
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 40
                flat: false
                property bool validParameter : false
                onClicked: {
                    // This button slot function:
                    // 1. stringify js/qml object file and send to C++ backend
                    // 2. check the parameters, return status from C++

                    demoContent.stringifyAndSend(demoContent.paraList, maincameraModel)

                    checkParameter()
                }
                function checkParameter(){
                    validParameter = utility.getEngineStatus()
                    if(validParameter){
                        console.log("Successfully found the files, you may begin to capture")
                        startCaptureBtn.enabled = true
                        //startNetWorkBtn.enabled = true
                    } else {
                        console.log("Error: Requested files cannot be found.")
                        startNetWorkBtn.enabled = false
                        //startCaptureBtn.enabled = false
                    }
                }
            }

            MessageDialog {
                id: messageDialog
                title: qsTr("Error Occurs!")
                text: utility.errorMessage

                buttons: MessageDialog.Ok
                onOkClicked: {
                    messageDialog.close()
                }
            }

            Text {
                id: helpText
                width: 231
                height: 80
                text: qsTr("You should set the parameters here and click 'SAVE AND INIT ENGINE' button before you start running the program on the left.")
                anchors.left: parent.left
                anchors.top: saveModelParaBtn.bottom
                font.pixelSize: 12
                font.family: "Fredoka Light"
                anchors.leftMargin: 20
                anchors.topMargin: 30
                wrapMode: Text.Wrap
            }

         }


        ToolBar {
            id: toolBar
            width: 300
            height: 60
            position: ToolBar.Footer
            anchors.top: canvas.bottom
            anchors.horizontalCenter: canvas.horizontalCenter
            anchors.topMargin: 40

            ToolButton {
                id: startCaptureBtn
                height: 50
                background: Rectangle {
                        color: "lightgray"
                    }
                enabled: false
                text: qsTr("START CAPTURE")
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                font.pixelSize: 14
                font.family: "Fredoka Light" 
                anchors.leftMargin: 10
                property bool start_capture: false
                onClicked: {
                    start_capture = !start_capture
                    if(start_capture){
                        console.log("Start Capture...")
                        text = qsTr("END CAPTURE")
                        startNetWorkBtn.enabled = true
                        controller.imageThreadStart()
                    } else {
                        console.log("End Capture...")
                        text = qsTr("START CAPTURE")
                        controller.imageThreadFinished()
                        startNetWorkBtn.enabled = false
                        delayTimer.start() // set to default img
                    }
                }
            }

            ToolButton {
                id: startNetWorkBtn
                height: 50
                background: Rectangle {
                        color: "lightgray"
                    }
                text: qsTr("START MODEL")
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: startCaptureBtn.right
                anchors.leftMargin: 40
                font.pixelSize: 14
                font.family: "Fredoka Light"
                flat: false
                enabled: false                  // default false
                property bool run_model: false
                onClicked: {
                    run_model = !run_model
                    if(run_model){
                        console.log("Starting Model...")
                        text = qsTr("END MODEL")
                    } else {
                        console.log("Ending Model...")
                        text = qsTr("START MODEL")
                    }
                    controller.startModelDetect()
                }
            }
        }

    }

    Behavior on visible {
        PropertyAnimation{
            target: demoContent
            property: "opacity"
            from: 0
            to: 1
            duration: 600
            easing.type: Easing.InExpo
        }
    }

    Connections {
        target: liveImageProvider
        function onImageChanged() {
            imageRGB.reload()
            imageIFR.reload()
        }
        Component.onDestruction: {
            controller.imageThreadFinished()
        }
    }

    Connections {
        target: utility
        function onErrorMessageChanged() {
            messageDialog.text = utility.getErrorMessage()
            messageDialog.open()
            saveModelParaBtn.validParameter = false
        }
    }

    Timer {
        id: delayTimer
        interval: 200
        repeat: false
        triggeredOnStart: true
        onTriggered: {
            imageRGB.setDefault()
            imageIFR.setDefault()
        }
    }
}

/*##^##
Designer {
    D{i:0;formeditorZoom:1.25}D{i:21}
}
##^##*/
