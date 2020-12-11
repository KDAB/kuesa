import QtQuick 2.12
import Qt3D.Core 2.12
import Qt3D.Render 2.12
import QtQuick.Scene2D 2.12
import Kuesa 1.3 as Kuesa

/*!
    \qmltype QuickSceneMaterial
    \inqmlmodule Kuesa
    \inherits Entity

    \brief Convenience wrapper that renders QtQuick content onto a Qt 3D
    texture set on a Kuesa Material as the diffuseMap property.

    QuickSceneMaterial takes care of instantiating a Scene2D with a proper
    RenderTarget and Texture2D color attachment. The Texture2D is then set on a
    Kuesa::GLTF2MaterialProperties node as the diffuseMap if present, otherwise
    as the baseColorMap.

    \qml
    import QtQuick 2.15
    import Kuesa 1.3 as Kuesa

    Item {
        id: root

        Kuesa.View3D {
            id: scene3D
            anchors.fill: parent
            source: "qrc:/car.gltf"
            camera: "CamSweep_Orientation"

            // Renders QtQuick into a Texture and set it as the diffuseMap
            // property of the MatScreen material
            Kuesa.QuickSceneMaterial {
                name: "MatScreen"
                collection: scene3D.materials

                Item {
                    width: 512
                    height: 512

                    Text {
                        anchors.centerIn: parent
                        text: "Hello! I'm a QtQuick Text Item"
                        font.pixelSize: 36
                        color: "white"
                    }
                }
            }
        }
    }
    \endqml
 */

Entity {
    id: root
    property alias collection: asset.collection
    property alias name: asset.name
    property alias width: offscreenTexture.width
    property alias height: offscreenTexture.height
    property alias format: offscreenTexture.format
    property alias generateMipMaps: offscreenTexture.generateMipMaps
    property alias magnificationFilter: offscreenTexture.magnificationFilter
    property alias minificationFilter: offscreenTexture.minificationFilter
    property alias wrapMode: offscreenTexture.wrapMode
    property alias mouseEnabled: qmlTexture.mouseEnabled
    property alias renderPolicy: qmlTexture.renderPolicy
    property alias entities: qmlTexture.entities
    default property alias item: qmlTexture.item

    Kuesa.Asset {
        id: asset
        onNodeChanged: if (node) {
           // Note all materials have a diffuseMap property
           // but they all have a baseColorMap
           if (node.diffuseMap !== undefined)
               node.diffuseMap = offscreenTexture
           else if (node.baseColorMap !== undefined)
               node.baseColorMap = offscreenTexture
            offscreenTexture.width = node.diffuseMap.width
            offscreenTexture.height = node.diffuseMap.height
        }
    }

    // Create empty texture
    Texture2D {
        id: offscreenTexture
        width: 512
        height: 512
        format: Texture.RGBA8_UNorm
        generateMipMaps: true
        magnificationFilter: Texture.Linear
        minificationFilter: Texture.LinearMipMapLinear
        wrapMode {
            x: WrapMode.ClampToEdge
            y: WrapMode.ClampToEdge
        }
    }

    // Use Scene2D to render QtQuick into the offscreen texture
    Scene2D {
        id: qmlTexture
        output: RenderTargetOutput {
            attachmentPoint: RenderTargetOutput.Color0
            texture: offscreenTexture
        }
        mouseEnabled: false
    }
}
