/*
    tst_View.qml
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

import QtTest 1.15
import Qt3D.Core 2.12
import Qt3D.Render 2.12
import Kuesa 1.3
import Kuesa.Effects 1.3

TestCase {

    View {
        id: view
        layers: [
            Layer { id: layer1 },
            Layer { id: layer2 }
        ]
        postProcessingEffects: [
            DepthOfFieldEffect {}
        ]
        reflectionPlanes: [
            ReflectionPlane {}
        ]
    }

    Camera {
        id: camera
    }

    name: "View"
    when: windowShown

    function test_layers()
    {
        // THEN
        compare(view.layers.length, 2)
    }

    function test_effects()
    {
        compare(view.postProcessingEffects.length, 1)
    }

    function test_reflectionPlanes()
    {
        compare(view.reflectionPlanes.length, 1)
    }

    SignalSpy {
        id: viewportChangedSpy
        target: view
        signalName: "viewportRectChanged"
    }

    function test_view_viewport() {
        // THEN
        verify(viewportChangedSpy.valid)

        // WHEN
        view.viewportRect = Qt.rect(0.5, 0.5, 0.5, 0.5)
        // THEN
        compare(viewportChangedSpy.count, 1)
        compare(viewportChangedSpy.signalArguments[0][0], Qt.rect(0.5, 0.5, 0.5, 0.5))

        // WHEN
        viewportChangedSpy.clear()
        view.viewportRect = Qt.rect(0.5, 0.5, 0.5, 0.5)
        // THEN
        compare(viewportChangedSpy.count, 0)
    }

    SignalSpy {
        id: frustumChangedSpy
        target: view
        signalName: "frustumCullingChanged"
    }

    function test_view_frustumculling() {
        // THEN
        verify(frustumChangedSpy.valid)

        // WHEN
        view.frustumCulling = false
        // THEN
        compare(frustumChangedSpy.count, 1)
        compare(frustumChangedSpy.signalArguments[0][0], false)

        // WHEN
        frustumChangedSpy.clear()
        view.frustumCulling = false
        // THEN
        compare(frustumChangedSpy.count, 0)
    }

    SignalSpy {
        id: cameraChangedSpy
        target: view
        signalName: "cameraChanged"
    }

    function test_view_camera() {
        // THEN
        verify(cameraChangedSpy.valid)

        // WHEN
        view.camera = camera
        // THEN
        compare(cameraChangedSpy.count, 1)
        compare(cameraChangedSpy.signalArguments[0][0], camera)

        // WHEN
        cameraChangedSpy.clear()
        view.camera = camera
        // THEN
        compare(cameraChangedSpy.count, 0)
    }

    SignalSpy {
        id: skinningSpy
        target: view
        signalName: "skinningChanged"
    }

    function test_view_skinning() {
        // THEN
        verify(skinningSpy.valid)

        // WHEN
        view.skinning = false
        // THEN
        compare(skinningSpy.count, 1)
        compare(skinningSpy.signalArguments[0][0], false)

        // WHEN
        skinningSpy.clear()
        view.skinning = false
        // THEN
        compare(skinningSpy.count, 0)
    }

    SignalSpy {
        id: backToFrontSortingSpy
        target: view
        signalName: "backToFrontSortingChanged"
    }

    function test_view_backToFrontSorting() {
        // THEN
        verify(backToFrontSortingSpy.valid)

        // WHEN
        view.backToFrontSorting = true
        // THEN
        compare(backToFrontSortingSpy.count, 1)
        compare(backToFrontSortingSpy.signalArguments[0][0], true)

        // WHEN
        backToFrontSortingSpy.clear()
        view.backToFrontSorting = true
        // THEN
        compare(backToFrontSortingSpy.count, 0)
    }

    SignalSpy {
        id: zFillingSpy
        target: view
        signalName: "zFillingChanged"
    }

    function test_view_zFilling() {
        // THEN
        verify(zFillingSpy.valid)

        // WHEN
        view.zFilling = true
        // THEN
        compare(zFillingSpy.count, 1)
        compare(zFillingSpy.signalArguments[0][0], true)

        // WHEN
        zFillingSpy.clear()
        view.zFilling = true
        // THEN
        compare(zFillingSpy.count, 0)
    }

    SignalSpy {
        id: particlesEnabledSpy
        target: view
        signalName: "particlesEnabledChanged"
    }

    function test_view_particlesEnabledSpy() {
        // THEN
        verify(particlesEnabledSpy.valid)

        // WHEN
        view.particlesEnabled = true
        // THEN
        compare(particlesEnabledSpy.count, 1)
        compare(particlesEnabledSpy.signalArguments[0][0], true)

        // WHEN
        particlesEnabledSpy.clear()
        view.particlesEnabled = true
        // THEN
        compare(particlesEnabledSpy.count, 0)
    }
}
