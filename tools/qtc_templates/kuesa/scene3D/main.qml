import QtQuick %{QtQuickVersion}
import QtQuick.Scene3D %{Scene3DVersion}

Item {
    id: root

    Scene3D {
        id: scene3d
        anchors.fill: parent
        aspects: ["render", "input", "logic", "animation"]

        SceneEntity {
            id: sceneEntity
        }
    }
}
