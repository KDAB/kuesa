/*
    View3D.qml

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

import QtQuick 2.15
import QtQml 2.15
import QtQuick.Scene3D 2.15
import Qt3D.Core 2.15
import Qt3D.Render 2.15
import Qt3D.Extras 2.15
import Kuesa 1.3 as Kuesa
import Kuesa.Utils 1.3 as KuesaUtils

/*!
    \qmltype View3D
    \inqmlmodule Kuesa
    \inherits Scene3D
    \brief Convenience wrapper around Scene3D and KuesaUtils::View3DScene.

    View3D forwards all properties defined on View3DScene. It makes it
    convenient to instantiate a Kuesa 3D view capable of being integrated in a
    QtQuick application.

    \qml
    import QtQuick 2.15
    import Kuesa 1.3 as Kuesa

    Item {
        id: root

        KuesaUtils.View3D {
            id: scene3D
            anchors.fill: parent
            source: "qrc:/car.gltf"
            camera: "CamSweep_Orientation"
            animations: [
                Kuesa.AnimationPlayer { name: "SomeAnimation"; loops: 2; running: true }
            ]
            transformTrackers: [
                Kuesa.TransformTracker { id: myTracker; name: "SomeNodeName" },
            ]
        }

        Text {
            id: innerText
            // Bind position and opacity to myTracker' properties
            readonly property point position: motorTracker.screenPosition
            x: position.x - implicitWidth * 0.5
            y: position.y - implicitHeight * 0.5
            opacity: 0.5 * motorTracker.translation.y
        }
    }

    \endqml

    By default it will draw to a single View if no SceneConfiguration
    referencing one or more ViewConfiguration instances is set on the
    activeScene.

    Therefore, keep in mind that resources that are camera / view dependent
    such as those set onto the transformTrackers, placeholderTrackers or
    layerNames properties of the View3D only work for that default single
    View.
 */

Scene3D {
    id: root
    property alias source: sceneConfiguration.source
    property var camera: ""
    property color backgroundColor: "white"
    readonly property real aspectRatio: width / Math.max(1, height)
    readonly property KuesaUtils.View3DScene scene: scene
    property alias frameGraph: scene.frameGraph
    property alias renderSettings: scene.renderSettings
    readonly property Entity activeCamera: scene.frameGraph.camera
    readonly property bool loaded: scene.importer.status === Kuesa.GLTF2Importer.Ready
    readonly property bool ready: scene.ready
    property alias activeScene: scene.activeScene
    property alias components: scene.components
    property alias showDebugOverlay: scene.showDebugOverlay
    property alias animations: sceneConfiguration.animations
    property alias transformTrackers: defaultViewConfiguration.transformTrackers
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
    property alias transforms: scene.transforms
    property alias asynchronous: scene.asynchronous
    property alias reflectionPlanes: scene.reflectionPlanes
    property alias placeholders: scene.placeholders
    property alias placeholderTrackers: defaultViewConfiguration.placeholderTrackers
    property alias views: sceneConfiguration.views
    property alias reflectionPlaneName: scene.reflectionPlaneName
    property alias layerNames: defaultViewConfiguration.layerNames
    default property alias children: scene.data

    signal loadingDone()

    aspects: ["input", "animation", "logic"]

    onCameraChanged: {
        if (typeof(camera) === 'string') {
            defaultViewConfiguration.cameraName = camera
        } else {
            scene.frameGraph.camera = camera
        }
    }

    function transformForEntity(entity) {
        return scene.transformForEntity(entity)
    }

    function start() { scene.start() }
    function restart() { scene.restart() }
    function stop() { scene.stop() }
    function gotoNormalizedTime(t) { scene.gotoNormalizedTime(t) }
    function gotoStart(run) { scene.gotoStart() }
    function gotoEnd(run) { scene.gotoEnd() }

    KuesaUtils.View3DScene {
        id: scene

        screenSize: Qt.size(root.width, root.height)
        activeScene: KuesaUtils.SceneConfiguration {
            id: sceneConfiguration
            views: KuesaUtils.ViewConfiguration {
                id: defaultViewConfiguration
            }
        }

        Binding {
            when: root.cameraAspectRatioMode == Scene3D.AutomaticAspectRatio
            target: root.activeCamera
            property: "aspectRatio"
            value: root.aspectRatio
            restoreMode: Binding.RestoreBindingOrValue
        }

        Binding {
            target: defaultViewConfiguration
            property: "clearColor"
            value: root.backgroundColor
            restoreMode: Binding.RestoreBindingOrValue
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

        // Force firing of signals attached to properties on the root element
        // when they are fired from View3DScene even if the property value
        // hasn't changed.
        // This can happen when switching activeScene using the same source
        // where ready remains true but it's still useful to be notified about
        // the fact that the loading is completed
        onLoadedChanged: root.loadedChanged()
        onReadyChanged: root.readyChanged()
    }
}
