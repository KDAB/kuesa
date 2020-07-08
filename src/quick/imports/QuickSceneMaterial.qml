import QtQuick 2.12
import Qt3D.Core 2.12
import Qt3D.Render 2.12
import QtQuick.Scene2D 2.12
import Kuesa 1.3 as Kuesa

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
        onNodeChanged: {
            node.diffuseMap = offscreenTexture
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
