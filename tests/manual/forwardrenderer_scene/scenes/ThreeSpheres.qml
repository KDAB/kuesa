/*
    ThreeSpheres.qml

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
import Qt3D.Extras 2.10
import Qt3D.Render 2.10
import Kuesa 1.1 as Kuesa

Entity
{
    Entity {
        components: [
            Kuesa.DirectionalLight { intensity: 2 }
        ]
    }

    Entity {
        components: [
            Transform { translation: Qt.vector3d(-3, 0, 0) },
            SphereMesh {},
            Kuesa.MetallicRoughnessMaterial {
                effect: Kuesa.MetallicRoughnessEffect {
                    opaque: false
                }
                metallicRoughnessProperties: Kuesa.MetallicRoughnessProperties {
                    baseColorFactor: Qt.rgba(1, 0, 0, .5)
                    metallicFactor: .5
                    roughnessFactor: .25
                }
            }
        ]
    }

    Entity {
        components: [
            SphereMesh {},
            Kuesa.MetallicRoughnessMaterial {
                effect: Kuesa.MetallicRoughnessEffect {
                    opaque: false
                }
                metallicRoughnessProperties: Kuesa.MetallicRoughnessProperties {
                    baseColorFactor: Qt.rgba(0, 1, 0, .5)
                    metallicFactor: .5
                    roughnessFactor: .25
                }
            }
        ]
    }

    Entity {
        components: [
            Transform { translation: Qt.vector3d(3, 0, 0) },
            SphereMesh {},
            Kuesa.MetallicRoughnessMaterial {
                effect: Kuesa.MetallicRoughnessEffect {
                    opaque: false
                }
                metallicRoughnessProperties: Kuesa.MetallicRoughnessProperties {
                    baseColorFactor: Qt.rgba(0, 0, 1, .5)
                    metallicFactor: .5
                    roughnessFactor: .25
                }
            }
        ]
    }
}
