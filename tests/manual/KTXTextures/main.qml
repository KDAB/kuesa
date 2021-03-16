/*
    main.qml

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Casafranca <juan.casafranca@kdab.com>

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

import QtQml 2.15
import Qt3D.Core 2.12
import Qt3D.Render 2.12
import Qt3D.Extras 2.12
import Kuesa 1.3 as Kuesa

Kuesa.SceneEntity {
    id: root

    components: [
        RenderSettings {
            activeFrameGraph: Kuesa.ForwardRenderer {
                id: frameGraph
                camera: fallbackCamera
                clearColor: "white"
                exposure: 0
            }
        }
    ]

    Camera {
        id: fallbackCamera

        position: Qt.vector3d(0.0, 1.0, 0.0)
        upVector: Qt.vector3d(0.0, 0.0, 1.0)
        viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
        projectionType: CameraLens.OrthographicProjection
        left: -texture.width
        right: texture.width
        top: texture.width
        bottom: -texture.width
//        aspectRatio: _winSize.width / _winSize.height
    }

    Kuesa.KTXTexture
    {
        id: texture
    }

    Timer {
// Doesn't work, non full size upload not supported
        property var sources: [
          "501.ktx2",
          "501_bcmp.ktx2", // Not supported yet in libktx
          "501.ktx",
          "501_uastc.ktx2", // Not supported yet in libktx
          "501_zcmp.ktx2",
          "disturb_ASTC4x4.ktx",
          "disturb_BC1.ktx",
          "disturb_ETC1.ktx",
          "disturb_PVR2bpp.ktx", // Q_UNREACHABLE. Misssing this format in Qt
          "lensflare_ASTC8x8.ktx",
          "lensflare_BC3.ktx",
          "lensflare_PVR4bpp 2.ktx", // Q_UNREACHABLE. Missing this format in Qt
          "lensflare_PVR4bpp.ktx", // Q_UNREACHABLE. Missing this format in Qt
          "out.ktx",
          "mipmaps.ktx",
          "rect.ktx",
          "rgb-reference.ktx",
          "sample_etc1s.ktx2", // Not supported yet in libktx
          "sample_uastc.ktx2", // Not supported yet in libktx
          "test.ktx"
        ]
        property int currentSource: 0

        interval: 2000
        repeat: true
        running: true
        triggeredOnStart: true

        onTriggered: {
            currentSource = (currentSource + 1) % sources.length
            console.log(sources[currentSource])
            texture.source = "qrc:/textures/" + sources[currentSource]
        }
    }

    PlaneMesh
    {
        id: mesh
        width: texture.width
        height: texture.height
        meshResolution: Qt.size(2,2)
    }

    Kuesa.UnlitMaterial
    {
        id: unlit
        effect: Kuesa.UnlitEffect
        {
            baseColorMapEnabled: true
            usingTexCoordAttribute: true
        }

        materialProperties: Kuesa.UnlitProperties
        {
            baseColorMap: texture
        }

    }

    Entity {
        id: viewer2D

        components: [
            mesh,
            unlit
        ]
    }
}
