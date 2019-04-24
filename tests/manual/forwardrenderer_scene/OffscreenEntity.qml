import Qt3D.Core 2.11
import Qt3D.Render 2.11
import Qt3D.Extras 2.10

Entity {
    id: root

    property int fboWidth: 1024
    property int fboHeight: 1024

    property alias layer: offscreenLayer
    property alias fbo: renderTarget

    Layer {
        id: offscreenLayer
    }

    CuboidMesh {
        id: mesh
        xExtent: 2.0
        yExtent: 2.0
        zExtent: 2.0
    }

    RenderTarget {
        id: renderTarget
        attachments : [
            RenderTargetOutput {
                attachmentPoint : RenderTargetOutput.Color0
                texture : Texture2D {
                    id : colorAttachment
                    width : root.fboWidth
                    height : root.fboHeight
                    format : Texture.RGBA8_UNorm
                    generateMipMaps : false
                    magnificationFilter : Texture.Linear
                    minificationFilter : Texture.Linear
                    wrapMode {
                        x: WrapMode.ClampToEdge
                        y: WrapMode.ClampToEdge
                    }
                }
            },
            RenderTargetOutput {
                attachmentPoint : RenderTargetOutput.Depth
                texture : Texture2D {
                    width : root.fboWidth
                    height : root.fboHeight
                    format : Texture.D32F
                    generateMipMaps : false
                    magnificationFilter : Texture.Linear
                    minificationFilter : Texture.Linear
                    wrapMode {
                        x: WrapMode.ClampToEdge
                        y: WrapMode.ClampToEdge
                    }
                }
            }
        ]
    }

    TextureMaterial {
        id: material
        texture: colorAttachment
    }

    components: [
        offscreenLayer,
        mesh,
        material
    ]
}

