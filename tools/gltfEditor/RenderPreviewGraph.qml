/*
    RenderPreviewGraph.qml

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jim Albamont <jim.albamont@kdab.com>

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

import Qt3D.Core 2.10
import Qt3D.Render 2.10
import Qt3D.Extras 2.10


FrameGraphNode {
    id: root
    property alias layers: layerFilter.layers
    property alias capturePreview: renderCapture
    property alias surface: renderSurfaceSelector.surface
    property var receiver: null
    property size previewSize: Qt.size(512,512)
    property color clearColor: "white"

    property var captureReply

    function requestCapture()
    {
        captureReply = capturePreview.requestCapture()
        captureReply.completeChanged.connect(onCaptureComplete)
    }

    function onCaptureComplete()
    {
        receiver.onCapture(captureReply)
    }

    LayerFilter {
        id: layerFilter
        filterMode: LayerFilter.AcceptAllMatchingLayers

        RenderSurfaceSelector {
            id: renderSurfaceSelector
            surface: frameGraph.renderSurface
            Viewport {
                RenderStateSet {
                    renderStates: [
                        MultiSampleAntiAliasing{},
                        DepthTest { depthFunction: DepthTest.Less }
                    ]
                    RenderTargetSelector {
                        target: RenderTarget {
                            attachments: [
                                RenderTargetOutput {
                                    attachmentPoint: RenderTargetOutput.Color0
                                    texture: Texture2DMultisample {
                                        width: previewSize.width
                                        height: previewSize.height
                                        format: Texture.RGBAFormat
                                        generateMipMaps: false
                                        magnificationFilter: Texture.Linear
                                        minificationFilter: Texture.Linear
                                        wrapMode {
                                            x: WrapMode.ClampToEdge
                                            y: WrapMode.ClampToEdge
                                        }
                                    }
                                },
                                RenderTargetOutput {
                                    attachmentPoint: RenderTargetOutput.Depth
                                    texture: Texture2DMultisample {
                                        width: previewSize.width
                                        height: previewSize.height
                                        format: Texture.DepthFormat
                                    }
                                }
                            ]
                        }

                        ClearBuffers {
                            buffers: ClearBuffers.ColorDepthBuffer
                            clearColor: root.clearColor
                            NoDraw {}
                        }
                        TechniqueFilter {
                            matchAll: FilterKey { name: "renderingStyle"; value: "forward" }
                            RenderPassFilter {
                                matchAny: FilterKey { name: "KuesaDrawStage"; value: "Opaque" }
                                CameraSelector {
                                    camera: Camera {
                                        projectionType: CameraLens.OrthographicProjection
                                        left: -1
                                        right: 1
                                        bottom: -1
                                        top: 1
                                        position: Qt.vector3d(0.0, 0.0, 5.0)
                                        upVector: Qt.vector3d(0.0, 1.0, 0.0)
                                        viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
                                    }
                                    RenderCapture {
                                        id: renderCapture
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
