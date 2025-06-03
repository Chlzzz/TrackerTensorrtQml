import QtQuick 2.15

Item {
    property point startPoint: Qt.point(0, 0)
    property point endPoint: Qt.point(0, 0)
    property bool selecting: false

    Rectangle {
        id: roiRect
        color: "transparent"
        border.color: "red"
        border.width: 2
        visible: false
    }

    MouseArea {
        anchors.fill: parent
        onPressed: {
            imageRGB.startPoint = Qt.point(mouse.x, mouse.y)
            imageRGB.selecting = true
            roiRect.visible = true
        }
        onPositionChanged: {
            if (imageRGB.selecting) {
                imageRGB.endPoint = Qt.point(mouse.x, mouse.y)
                imageRGB.updateROIRect()
            }
        }
        onReleased: {
            imageRGB.endPoint = Qt.point(mouse.x, mouse.y)
            imageRGB.selecting = false
            roiRect.visible = false
            var roi = imageRGB.getROI()
            console.log("Selected ROI:", roi)
            controller.setROI(roi)
        }
    }

    function updateROIRect() {
        roiRect.x = Math.min(imageRGB.startPoint.x, imageRGB.endPoint.x)
        roiRect.y = Math.min(imageRGB.startPoint.y, imageRGB.endPoint.y)
        roiRect.width = Math.abs(imageRGB.endPoint.x - imageRGB.startPoint.x)
        roiRect.height = Math.abs(imageRGB.endPoint.y - imageRGB.startPoint.y)
    }

    function getROI() {
        return Qt.rect(
            Math.min(imageRGB.startPoint.x, imageRGB.endPoint.x),
            Math.min(imageRGB.startPoint.y, imageRGB.endPoint.y),
            Math.abs(imageRGB.endPoint.x - imageRGB.startPoint.x),
            Math.abs(imageRGB.endPoint.y - imageRGB.startPoint.y)
        )
    }
}
