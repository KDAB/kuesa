import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Scene3D 2.15
import Qt3D.Core 2.15
import Qt3D.Render 2.15
import Qt3D.Input 2.15
import Kuesa 1.3 as Kuesa

import Test 1.0

Item {

    Scene3D
    {
        id: scene3D
        anchors.fill: parent
        multisample: true
        focus: true
        aspects: ["animation", "logic", "input"]

        Kuesa.SceneEntity {
            id: kuesaScene

            components: [
                RenderSettings {
                    // Configures renderer (which camera to use, which effects to apply ...)
                    activeFrameGraph: Kuesa.ForwardRenderer {
                        id: frameGraph
                        clearColor: Qt.rgba(0.01, 0.01, 0.01, 1.0);
                        camera: sceneCamera.node
                        showDebugOverlay: true
                        usesStencilMask: true
                        postProcessingEffects: [
                            StencilEffect {
                                color: "blue"
                            }
                        ]
                    }


                    renderPolicy: RenderSettings.Always
                },
                InputSettings { }
            ]

            Kuesa.Asset {
                id: cube
                sceneEntity: kuesaScene
                name: "Cube"
            }

            Kuesa.Asset {
                id: sceneCamera
                collection: kuesaScene.cameras
                name: "Camera_Orientation"
            }

            StencilHelper {
                entity: cube.node
            }

            // Load glTF2 file
            Kuesa.GLTF2Importer {
                id: gltfImporter
                sceneEntity: kuesaScene
                source: "qrc:/scene.gltf"
            }
        }
    }
}
