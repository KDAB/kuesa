/*
    main.qml

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

import QtQml 2.2
import Qt3D.Core 2.10
import Qt3D.Render 2.10
import Qt3D.Input 2.0
import Qt3D.Extras 2.10
import Kuesa 1.0 as Kuesa
import Kuesa.Effects 1.0

Entity {
    id: scene

    OffscreenEntity {
        id: offscreen
        enabled: _controller.renderIntoFbo
    }

    Kuesa.ForwardRenderer {
        id: forwardRenderer
        camera: mainCamera
        backToFrontSorting: _controller.alphaBlending
        clearColor: _controller.clearColor
        frustumCulling: _controller.frustumCulling
        zFilling: _controller.zFill
    }

    RenderSurfaceSelector {
        id: deferredRenderer
        // 1st pass: render Kuesa scene into FBO
        Kuesa.ForwardRenderer {
            id: deferredKuesaRenderer
            camera: mainCamera
            backToFrontSorting: _controller.alphaBlending
            clearColor: _controller.clearColor
            frustumCulling: _controller.frustumCulling
            zFilling: _controller.zFill
            renderTarget: offscreen.fbo
        }
        // 2nd pass: render quad with scene fbo texture
        CameraSelector {
            camera: mainCamera
            LayerFilter {
                layers: [ offscreen.layer ]
                filterMode: LayerFilter.AcceptAnyMatchingLayers
                ClearBuffers {
                    buffers: ClearBuffers.ColorDepthBuffer
                    clearColor: "black"
                }
            }
        }
    }

    components: [
        InputSettings { },
        RenderSettings {
            activeFrameGraph: _controller.renderIntoFbo ? deferredRenderer : forwardRenderer
        }
    ]

    function enablePostProcessingEffect(effect, enable) {
        var renderer = _controller.renderIntoFbo ? deferredKuesaRenderer : forwardRenderer
        if (enable)
            renderer.addPostProcessingEffect(effect);
        else
            renderer.removePostProcessingEffect(effect);
    }

    function movePostProcessingEffect(oldFG, newFG, effect) {
        oldFG.removePostProcessingEffect(effect);
        newFG.addPostProcessingEffect(effect);
    }

    Connections {
        target: _controller
        onThresholdEffectChanged: enablePostProcessingEffect(thresholdComponent, _controller.thresholdEffect)
        onBloomEffectChanged: enablePostProcessingEffect(bloomComponent, _controller.bloomEffect)
        onBlurEffectChanged: enablePostProcessingEffect(blurComponent, _controller.blurEffect)
        onOpacityMaskEffectChanged: enablePostProcessingEffect(opacityMaskComponent, _controller.opacityMaskEffect)
        onRenderIntoFboChanged: {
            var oldKuesaRenderer = _controller.renderIntoFbo ? forwardRenderer : deferredKuesaRenderer
            var newKuesaRenderer = _controller.renderIntoFbo ? deferredKuesaRenderer : forwardRenderer
            if (_controller.thresholdEffect) movePostProcessingEffect(oldKuesaRenderer, newKuesaRenderer, thresholdComponent);
            if (_controller.bloomEffect) movePostProcessingEffect(oldKuesaRenderer, newKuesaRenderer, bloomComponent);
            if (_controller.blurEffect) movePostProcessingEffect(oldKuesaRenderer, newKuesaRenderer, blurComponent);
            if (_controller.opacityMaskEffect) movePostProcessingEffect(oldKuesaRenderer, newKuesaRenderer, opacityMaskComponent);
        }
    }

    ThresholdEffect {
        id: thresholdComponent
        threshold: _controller.threshold
    }

    BlurEffect{
        id: blurComponent
        blurPassCount: _controller.blurPassCount
    }

    BloomEffect {
        id: bloomComponent
        exposure: _controller.bloomExposure
        threshold: _controller.bloomThreshold
        blurPassCount: _controller.bloomBlurPassCount
    }

    OpacityMask {
        id: opacityMaskComponent
        premultipliedAlpha: _controller.opacityMaskPremultipliedAlpha
        mask: TextureLoader { source: "./opacitymask.png" }
    }

    Camera {
        id: mainCamera
        position: Qt.vector3d(0, 0, 10)
        viewCenter: Qt.vector3d(0, 0, 0)
    }

    OrbitCameraController {
        camera: mainCamera
    }

    EntityLoader {
         source: "./scenes/" + _controller.sceneName + ".qml"
    }
 }
