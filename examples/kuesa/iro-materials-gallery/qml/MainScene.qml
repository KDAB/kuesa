/*
    MainScene.qml

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

//! [0]
import Qt3D.Core 2.12
import Qt3D.Render 2.12
import Qt3D.Input 2.12
import Qt3D.Extras 2.12
import QtQuick 2.12

import Kuesa 1.2 as Kuesa
import Kuesa.Effects 1.1 as KuesaFX
import Kuesa.Utils 1.3 as KuesaUtils

Kuesa.SceneEntity {
    id: root3D
    signal clickedAt(int x, int y)
//! [0]

    components: [
        RenderSettings {
//! [2.2]
            activeFrameGraph: Kuesa.ForwardRenderer {
                id: frameGraph
                camera: camera
                clearColor: Qt.rgba(0.1, 0.1, 0.1, 1.0)
            }
//! [2.2]
            pickingSettings.pickMethod: PickingSettings.TrianglePicking
        },
        InputSettings { },
        EnvironmentLight {
            irradiance: TextureLoader {
                source: _assetsPrefix + "pink_sunrise_16f_irradiance" + ((!root3D.es2) ? ".dds" : "_es2.dds")

                minificationFilter: Texture.LinearMipMapLinear
                magnificationFilter: Texture.Linear
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
                generateMipMaps: false
            }
            specular: TextureLoader {
                source: _assetsPrefix + "pink_sunrise_16f_specular" + ((!root3D.es2) ? ".dds" : "_es2.dds")

                minificationFilter: Texture.LinearMipMapLinear
                magnificationFilter: Texture.Linear
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
                generateMipMaps: false
            }
        },
//! [3]
        ObjectPicker {
            onClicked: {
                _materialInspector.inspect(pick)
                root3D.clickedAt(pick.position.x, pick.position.y)
            }
        }
//! [3]
    ]

//! [2.1]
    Camera {
        id: camera
        position: Qt.vector3d(15.0, 5.0, 15)
        upVector: Qt.vector3d(0.0, 1.0, 0.0)
        viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
        aspectRatio: _view.width / _view.height
    }

    KuesaUtils.OrbitCameraController {
        camera: camera
        windowSize: Qt.size(_view.width, _view.height)
    }
//! [2.1]

//! [1]
    Kuesa.GLTF2Importer {
        id: gltf2importer
        sceneEntity: root3D
        assignNames: true
        source: _assetsPrefix + "iro-materials-gallery.gltf"
        options.generateTangents: true
    }
//! [1]
}
