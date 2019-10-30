import Qt3D.Core %{Qt3DCoreVersion}
import Qt3D.Render %{Qt3DVersion}
import Qt3D.Input %{Qt3DInputVersion}
import Qt3D.Extras %{Qt3DVersion}
import Kuesa %{KuesaVersion} as Kuesa

Kuesa.SceneEntity {
    id: root3D

    components: [
        RenderSettings {
            activeFrameGraph: Kuesa.ForwardRenderer {
                id: frameGraph
                camera: cameraAsset.node ? cameraAsset.node : fallbackCamera
                clearColor: "white"
            }
        },
        InputSettings { },
        DirectionalLight {
            worldDirection: frameGraph.camera.viewVector
            intensity: 0.8
            color: "white"
        }
    ]

    Camera {
        id: fallbackCamera
        position: Qt.vector3d(0.0, 0.0, 7.0)
        upVector: Qt.vector3d(0.0, 1.0, 0.0)
        viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
    }

    OrbitCameraController {
        id: controller
        camera: frameGraph.camera
        linearSpeed: 5
        lookSpeed: 180
    }

    Kuesa.Asset {
        id: cameraAsset
        name: root3D.cameras.names.length ? root3D.cameras.names[0] : ""
        collection: root3D.cameras
    }

@if "%{GLTF2FilePath}" !== ""
    Kuesa.GLTF2Importer {
        id: gltf2importer
        sceneEntity: root3D
        source: "file://%{GLTF2FilePath}"
    }
@else
    Entity {
        components: [
            Kuesa.MetallicRoughnessMaterial { id: material; baseColorFactor: "red" },
            CuboidMesh {},
            Transform { rotationX: 45; rotationZ: 45 }
        ]
    }
@endif
}
