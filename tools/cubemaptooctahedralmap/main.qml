/*
    main.qml

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Paul Lemire <paul.lemire@kdab.com>

    Licensees holding valid proprietary KDAB Kuesa licenses may use this file in
    accordance with the Kuesa Enterprise License Agreement provided with the Software in the
    LICENSE.KUESA.ENTERPRISE file.

    Contact info@kdab.com if any conditions of this licensing are not clear to you.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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

    readonly property bool shouldRunComputeShader: cubeMapTexture.status === Texture.Ready &&
                                                   textureWidthHasBeenSet &&
                                                   textureHeightHasBeenSet &&
                                                   copyOutputToSSBO.isInitialized &&
                                                   !copyOutputToSSBO.isComplete
    readonly property int textureWidth: cubeMapTexture.width
    readonly property int textureHeight: cubeMapTexture.height
    property bool textureWidthHasBeenSet: false
    property bool textureHeightHasBeenSet: false

    onTextureWidthChanged: textureWidthHasBeenSet = true
    onTextureHeightChanged: textureHeightHasBeenSet = true

    Texture2D {
        id: octahedralTexture
        // To account for mipmaps
        width: textureWidth * 1.5
        height: textureHeight

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
