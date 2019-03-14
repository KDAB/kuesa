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

    components: [
        InputSettings { },
        RenderSettings {
            Kuesa.ForwardRenderer {
                id: renderer
                camera: mainCamera
                backToFrontSorting: _controller.alphaBlending
                clearColor: _controller.clearColor
                frustumCulling: _controller.frustumCulling
                zFilling: _controller.zFill
            }
        }
    ]

    function enablePostProcessingEffect(effect, enable) {
        if (enable)
            renderer.addPostProcessingEffect(effect);
        else
            renderer.removePostProcessingEffect(effect);
    }

    Connections {
        target: _controller
        onThresholdEffectChanged: enablePostProcessingEffect(thresholdComponent, _controller.thresholdEffect)
        onBloomEffectChanged: enablePostProcessingEffect(bloomComponent, _controller.bloomEffect)
        onBlurEffectChanged: enablePostProcessingEffect(blurComponent, _controller.blurEffect)
        onOpacityMaskEffectChanged: enablePostProcessingEffect(opacityMaskComponent, _controller.opacityMaskEffect)
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
