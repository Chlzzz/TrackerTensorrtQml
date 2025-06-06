import QtQuick 2.15
import QtCharts 2.15
import QtQuick.Controls 2.15

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
            "source_mode": "DUAL",
            "source0_type": "USB",
            "source0": "0",
            "source1_type": "USB",
            "source1": "0",
            "infer_device": "Nvidia GPU",
            "task_type": "MOT",
            "network": ""
        }

        function stringifyAndSend(paraList){
            var jsonString = JSON.stringify(paraList)
            console.log(jsonString)
            console.log("Send to C++")
            // balabala send!
            utility.parseJSValue(paraList)
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
                anchors.leftMargin: 20
                fillMode: Image.PreserveAspectFit
                cache: false
                asynchronous: false
                sourceSize.width: 640
                sourceSize.height: 480
                // Reload image
                function reload(){
//                    source = ""
                    source = "image://live/image0?" + Date.now()
                }
                // Set default image
                function setDefault(){
                    source = "qrc:/assets/videocam.png"
                }

                RoiSetting {
                    id: roiset
                }

//                property point startPoint: Qt.point(0, 0)
//                property point endPoint: Qt.point(0, 0)
//                property bool selecting: false

//                Rectangle {
//                    id: roiRect
//                    color: "transparent"
//                    border.color: "red"
//                    border.width: 2
//                    visible: false
//                }

//                MouseArea {
//                    anchors.fill: parent
//                    onPressed: {
//                        imageRGB.startPoint = Qt.point(mouse.x, mouse.y)
//                        imageRGB.selecting = true
//                        roiRect.visible = true
//                    }
//                    onPositionChanged: {
//                        if (imageRGB.selecting) {
//                            imageRGB.endPoint = Qt.point(mouse.x, mouse.y)
//                            imageRGB.updateROIRect()
//                        }
//                    }
//                    onReleased: {
//                        imageRGB.endPoint = Qt.point(mouse.x, mouse.y)
//                        imageRGB.selecting = false
//                        roiRect.visible = false
//                        var roi = imageRGB.getROI()
//                        console.log("Selected ROI:", roi)
//                        controller.setROI(roi)
//                    }
//                }

//                function updateROIRect() {
//                    roiRect.x = Math.min(imageRGB.startPoint.x, imageRGB.endPoint.x)
//                    roiRect.y = Math.min(imageRGB.startPoint.y, imageRGB.endPoint.y)
//                    roiRect.width = Math.abs(imageRGB.endPoint.x - imageRGB.startPoint.x)
//                    roiRect.height = Math.abs(imageRGB.endPoint.y - imageRGB.startPoint.y)
//                }

//                function getROI() {
//                    return Qt.rect(
//                        Math.min(imageRGB.startPoint.x, imageRGB.endPoint.x),
//                        Math.min(imageRGB.startPoint.y, imageRGB.endPoint.y),
//                        Math.abs(imageRGB.endPoint.x - imageRGB.startPoint.x),
//                        Math.abs(imageRGB.endPoint.y - imageRGB.startPoint.y)
//                    )
//                }

            }
            Image {
                id: imageIFR
                width: 640
                height: 480
                source: "qrc:/assets/videocam.png"
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: imageRGB.right
                anchors.leftMargin: 20
                fillMode: Image.PreserveAspectFit
                cache: false
                asynchronous: false
                sourceSize.width: 640
                sourceSize.height: 480
                // Reload image
                function reload(){
//                    source = ""
                    source = "image://live/image1?" + Date.now()
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

            Text {
                id: cameraText
                text: qsTr("Source")
                anchors.top: parent.top
                anchors.topMargin: 10
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 15
                font.family: "Fredoka Light"   
            }

            Rectangle {
                anchors.top: cameraText.bottom
                height: 90
                width: parent.width
                Column {
                    id: camcol
                    spacing: 2
                    SourceSelect{
                        paraList: demoContent.paraList
                        is_source0: true
                    }
                    SourceSelect{
                        paraList: demoContent.paraList
                        is_source0: false
                    }
                }
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
                    demoContent.stringifyAndSend(demoContent.paraList)

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
        function onImageChanged(camIndex) {
            if(camIndex === 0){
                imageRGB.reload()
            }else if(camIndex === 1) {
                imageIFR.reload()
            }
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
