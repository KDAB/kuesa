import Qt3D.Core 2.12
import Qt3D.Render 2.12
import Qt3D.Input 2.0
import Qt3D.Extras 2.12
import QtQml 2.2

Entity {
    components: [
        RenderSettings {
            activeFrameGraph: RenderSurfaceSelector {
                // Render Each Face of CubeMap
                Viewport {
                    LayerFilter {
                        layers: octahedronLayer
                        RenderTargetSelector {
                            target: RenderTarget {
                                id: octahedralTarget
                                attachments: [
                                    RenderTargetOutput {
                                        attachmentPoint: RenderTargetOutput.Color0
                                        texture: octahedralTexture
                                    }
                                ]
                                ClearBuffers {
                                    buffers: ClearBuffers.ColorBuffer
                                    RenderStateSet {
                                        renderStates: [
                                            SeamlessCubemap {}
                                        ]
                                    }
                                }
                            }
                        }
                    }

                    // Execute Compute shader after all memory access
                    // operations
                    MemoryBarrier {
                        waitFor: MemoryBarrier.All
                        enabled: shouldRunComputeShader
                        DispatchCompute {
                            id: dispatchCompute
                            enabled: shouldRunComputeShader
                        }
                    }

                    // Retrieve SSBO written by Compute Shader
                    MemoryBarrier {
                        enabled:shouldRunComputeShader
                        waitFor: MemoryBarrier.All
                        BufferCapture {
                            enabled: shouldRunComputeShader
                        }
                    }

                    // Clear Screen to show previews
                    ClearBuffers {
                        buffers: ClearBuffers.ColorDepthBuffer
                        NoDraw {}
                    }

                    Viewport {
                        normalizedRect: Qt.rect(0, 0, 0.25, 0.25)
                        // OctaHedral Texture preview
                        LayerFilter {
                            layers: preview2DLayer
                        }
                    }

                    CameraSelector {
                        camera: camera
                        LayerFilter {
                            layers: skyboxLayer
                            RenderStateSet {
                                renderStates: [
                                    CullFace { mode: CullFace.Front },
                                    DepthTest { depthFunction: DepthTest.LessOrEqual }
                                ]
                            }
                        }
                    }
                }
            }
        },
        InputSettings {}
    ]

    Layer { id: octahedronLayer }
    Layer { id: preview2DLayer }
    Layer { id: skyboxLayer }


    TextureLoader {
        id: cubeMapTexture
        source: _cubeMapPath
        onStatusChanged: console.log("Status "  + status)
    }

    readonly property bool shouldRunComputeShader: cubeMapTexture.status === Texture.Ready && !copyOutputToSSBO.isComplete
    readonly property int textureWidth: 2048

    Texture2D {
        id: octahedralTexture
        // To account for mipmaps
        width: cubeMapTexture.width * 1.5
        height: cubeMapTexture.height

        // Use format capable of holding HDR information
        format: Texture.RGBA32F
        generateMipMaps: false
        magnificationFilter: Texture.Linear
        minificationFilter: Texture.Linear
        wrapMode {
            x: WrapMode.ClampToEdge
            y: WrapMode.ClampToEdge
        }
    }

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, 40.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    OrbitCameraController {
        camera: camera
    }

    FlatOctahedronEntity {
        cubeMap: cubeMapTexture
        layer: octahedronLayer
        firstLevelWidth: textureWidth
    }

    CopyOutputToSSBO {
        id: copyOutputToSSBO
        fboTexture: octahedralTexture
    }

    OctahedralMapPreview {
        octahedralTexture: octahedralTexture
        layer: preview2DLayer
    }

    OctahedralSkybox {
        octahedralTexture: octahedralTexture
        layer: skyboxLayer
    }
}
