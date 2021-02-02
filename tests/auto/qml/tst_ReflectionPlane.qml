/*
    tst_ReflectionPlane.qml
    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

import QtTest 1.15
import Qt3D.Core 2.12
import Qt3D.Render 2.12
import Kuesa 1.3
import Kuesa.Effects 1.3

TestCase {

    Layer { id: layer1 }
    Layer { id: layer2 }

    property Layer emptyLayer

    ReflectionPlane {
        id: plane
        layers: [
            layer1,
            emptyLayer
        ]
    }

    SignalSpy {
        id: layersChangedSpy
        target: plane
        signalName: "layersChanged"
    }

    function test_layers()
    {
        // THEN
        verify(layersChangedSpy.valid)
        compare(plane.layers.length, 1)
        compare(layersChangedSpy.count, 1) // One Add (emptyLayer not added)

        // WHEN
        emptyLayer = layer2

        // THEN
        compare(plane.layers.length, 2)
        compare(layersChangedSpy.count, 4) // Clear / Add / Add
    }

    SignalSpy {
        id: equationChangedSpy
        target: plane
        signalName: "equationChanged"
    }

    function test_equation()
    {
        // THEN
        verify(equationChangedSpy.valid)

        // WHEN
        plane.equation = Qt.vector4d(1.0, 0.0, 0.0, 0.0)

        // THEN
        compare(equationChangedSpy.count, 1)
        compare(plane.equation, Qt.vector4d(1.0, 0.0, 0.0, 0.0))
    }
}
