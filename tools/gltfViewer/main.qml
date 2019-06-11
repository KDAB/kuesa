import Qt3D.Core 2.12
import Qt3D.Render 2.12
import Qt3D.Animation 2.12
import Qt3D.Input 2.12
import Qt3D.Extras 2.12
import Kuesa 1.1 as Kuesa
import KuesaViewer 1.0 as KuesaViewer

Kuesa.SceneEntity {
    id: root
    property int __currentCamera: -1

    property url source: _gltfFile
    onSourceChanged: {
        root.clearCollections()
        animations.running = false
        importer.source = source
    }

    Entity {
        parent: frameGraph.camera
        components: [
            Kuesa.PointLight {}
        ]
    }

    components: [
        RenderSettings {
            activeFrameGraph: Kuesa.ForwardRenderer {
                id: frameGraph
                camera: cameraAsset.node ? cameraAsset.node : fallbackCamera
                clearColor: "white"
            }
        },
        InputSettings { },
        EnvironmentLight {
            id: envLight
            property string envMapFormat: Qt.platform.os == "osx" ? "_16f" : ""

            irradiance: TextureLoader {
                source: "qrc:/wobbly_bridge" + envLight.envMapFormat + "_irradiance.dds"
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
                generateMipMaps: false
            }
            specular: TextureLoader {
                source: "qrc:/wobbly_bridge" + envLight.envMapFormat + "_specular.dds"
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
                generateMipMaps: false
            }
        }
    ]

    Camera {
        id: fallbackCamera
        objectName: "fallbackCamera"

        position: Qt.vector3d(0.0, 0.0, 7.0)
        upVector: Qt.vector3d(0.0, 1.0, 0.0)
        viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
        aspectRatio: _winSize.width / _winSize.height
    }

    KuesaViewer.OrbitCameraController {
        id: controller
        camera: frameGraph.camera
        windowSize: _winSize
    }

    Kuesa.Asset {
        id: cameraAsset
        name: __currentCamera >= 0 && __currentCamera < root.cameras.names.length ? root.cameras.names[__currentCamera] : ""
        collection: root.cameras
        onNodeChanged: {
            if (node)
                node.aspectRatio = _winSize.width / _winSize.height
        }
    }

    Kuesa.GLTF2Importer {
        id: importer
        sceneEntity: root
        assignNames: true
    }

    onLoadingDone: {
        console.log(importer.source)
        console.log("Cameras:", root.cameras.names)
        console.log("Animations:", root.animationClips.names)
        if (root.cameras.names.length === 0) {
            __currentCamera = -1
            _app.viewAll(fallbackCamera)
        } else {
            __currentCamera = 0
            for (var i=0; i<root.cameras.names.length; ++i) {
                if (root.cameras.names[i] === _gltfCamera) {
                    __currentCamera = i
                    break
                }
            }
        }
        animations.model = root.animationClips.names.length ? _gltfAnimations : 0
        animations.running = root.animationClips.names.length > 0
    }

    NodeInstantiator {
        id: animations
        property bool running: true
        delegate: Kuesa.AnimationPlayer {
            sceneEntity: root
            clip: model.modelData
            loops: _gltfLoopAnimations ? Kuesa.AnimationPlayer.Infinite : 1
            running: animations.running
        }
    }

    KeyboardDevice { id: kb }
    KeyboardHandler {
        sourceDevice: kb
        focus: true

        onTabPressed: {
            if (__currentCamera != -1) {
                if (++__currentCamera >= root.cameras.names.length)
                    __currentCamera = 0
            }
        }
        onBacktabPressed: {
            if (__currentCamera != -1) {
                if (--__currentCamera < 0)
                    __currentCamera = root.cameras.names.length - 1
            }
        }
        onSpacePressed: animations.running = !animations.running
        onPressed: {
            if (event.key === Qt.Key_R && event.modifiers === Qt.ControlModifier)
                _app.reload()
        }
    }
}
