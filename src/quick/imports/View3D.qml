/*
    View3D.qml

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mike Krus <mike.krus@kdab.com>

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

import QtQuick 2.12
import QtQuick.Scene3D 2.12
import Qt3D.Core 2.12
import Qt3D.Render 2.12
import Qt3D.Extras 2.12
import Kuesa.Utils 1.3 as KuesaUtils

Scene3D {
    id: root
    property alias source: scene.source
    property var camera: ""
    readonly property real aspectRatio: width / Math.max(1, height)
    readonly property Entity activeCamera: scene.frameGraph.camera
    property alias components: scene.components
    property alias showDebugOverlay: scene.showDebugOverlay
    property alias animations: scene.animations
    property alias transformTrackers: scene.transformTrackers
    property alias animationClips: scene.animationClips
    property alias armatures: scene.armatures
    property alias layers: scene.layers
    property alias materials: scene.materials
    property alias meshes: scene.meshes
    property alias skeletons: scene.skeletons
    property alias textures: scene.textures
    property alias cameras: scene.cameras
    property alias entities: scene.entities
    property alias textureImages: scene.textureImages
    property alias animationMappings: scene.animationMappings
    property alias lights: scene.lights
//    property alias transforms: scene.transforms
    default property alias children: scene.data

    signal loadingDone()

    aspects: ["input", "animation", "logic"]

    onCameraChanged: {
        if (typeof(camera) === 'string') {
            scene.cameraName = camera
        } else {
            scene.frameGraph.camera = camera
        }
    }

    function start() { scene.start() }
    function stop() { scene.stop() }
    function gotoNormalizedTime(t) { scene.gotoNormalizedTime(t) }
    function gotoStart(run) { scene.gotoStart() }
    function gotoEnd(run) { scene.gotoEnd() }

    KuesaUtils.View3DScene {
        id: scene

        screenSize: Qt.size(root.width, root.height)

        Binding {
            target: root.activeCamera
            property: "aspectRatio"
            value: root.aspectRatio
        }

        onLoadingDone: {
            // Need to make sure QNodes created/parented outside the Scene3D
            // are properly moved into the scene graph to make sure backend
            // nodes get created
            for(var c in data)
                adoptNode(data[c])
            for(c in components)
                adoptNode(components[c])

            root.loadingDone()
        }
    }
}