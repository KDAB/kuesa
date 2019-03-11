import QtQuick 2.11
import QtQuick.Scene3D 2.0

Scene3D {
    id: scene3D
    anchors.fill: parent
    multisample: true
    aspects: ["input", "animation", "logic"]

    MainScene { }
}
