/*
    MainScene.qml

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mauro Persano <mauro.persano@kdab.com>

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

import Qt3D.Core 2.12
import Qt3D.Render 2.12
import Qt3D.Input 2.12
import Qt3D.Logic 2.12
import Qt3D.Extras 2.12
import Qt3D.Animation 2.12
import QtQml 2.2
import QtQuick 2.12

//![0]
import Kuesa 1.2 as Kuesa
import Kuesa.Effects 1.1 as KuesaFX
import MusicBox 1.0 as MusicBox

Kuesa.SceneEntity {
    id: scene
//![0]

//![2.1]
    QtObject {
        id: d

        readonly property var robotArmAnimations: [
            'ArmTopAction',
            'FingerBase3Action',
            'armBottomBottomAction',
            'armBottomBottomGauge.001Action',
            'armBottomBottomGaugeAction',
            'armBottomTopAction',
            'cogAction',
            'elbowPivotAction',
            'elbowPivotBaseAction',
            'fingerBase2Action',
            'fingerKnuckle1Action',
            'fingerPivot1.001Action',
            'fingerPivot2Action',
            'fingerTop1Action',
            'fingerTop2Action',
            'fingerTop3Action',
            'handleAction.001',
            'handleCoverAction',
            'musicBarrelAction',
            'shoulderPivotAction',
            'shoulderPivotBaseAction',
            'thumbBaseBottomAction',
            'thumbKnuckleAction',
            'thumbPivotAction',
            'thumbTopAction',
            'wristPivotAction'
        ]
//![2.1]

        readonly property var musicRows: [
            '........x.....................',
            '...x................x.........',
            '................x.............',
            '............x.................',
            '........x.....................',
            '....x.................x.......',
            '................x.............',
            '..............x...............',
            '........x.....................',
            '.....x....x............x......',
            '..................x...........',
            '......x........x..............',
            '.......x......................',
            '......x...x.............x.....',
            '..................x...........',
            '................x.............',
            '........x.....................',
            '.........x...............x....',
            '.......x.........x..x.........',
            '.....x......x..x..............',
            '...x..........................',
            '....x.................x.......',
            '......x.......x..x.x..........',
            '........x..x..x...............',
            '.........x....................',
            '........x.........x.......x...',
            '..................x...x.......',
            '................x.x...........',
            '..........x...................',
            '....x.x.x.......x.......x.....',
            '................x..x..x.......',
            '............x..x..............',
            '.......x......................',
            '..x....x.x.........x......x...',
            '...................x...x......',
            '...........x...x.x.x..........',
            '.......x......................',
            '...x...x..x.........x......x..',
            '....................x..x......',
            '...............x....x..x......',
            '.......x......................',
            '....x....x.x..........x.....x.',
            '.................x.x..x.......',
            '.....x...x.x..x..x.x..........',
            '......x..x.x.....x.x..x.......',
            '.....x...x...x.........x.....x',
            '.................x...x...x....',
            '...............x.x............',
            '.......x......................',
            '........x...............x.....',
            '......x.........x..x..........',
            '....x......x..x...............',
            '..x...........................',
            '.x................x..x........',
            'x.............................',
            '...x.................x.x......',
            '.....x........................',
            '....x..x...x..............x...',
            '...................x..........',
            '.......x...x...x.......x......',
            '.........................x....',
            '...........x.....x.x......x...',
            '.............................x',
            '..........................x...'
        ]

        readonly property var prongAnimations: [
            prong29Action,
            prong28Action,
            prong27Action,
            prong26Action,
            prong25Action,
            prong24Action,
            prong23Action,
            prong22Action,
            prong21Action,
            prong20Action,
            prong19Action,
            prong18Action,
            prong17Action,
            prong16Action,
            prong15Action,
            prong14Action,
            prong13Action,
            prong12Action,
            prong11Action,
            prong10Action,
            prong09Action,
            prong08Action,
            prong07Action,
            prong06Action,
            prong05Action,
            prong04Action,
            prong03Action,
            prong02Action,
            prong01Action,
            prong00Action,
        ]

        property bool animationRunning: false
    }

    components: [
        RenderSettings {
            activeFrameGraph: Kuesa.ForwardRenderer {
                id: frameGraph
                camera: mainCamera
                backToFrontSorting: true
                toneMappingAlgorithm: KuesaFX.ToneMappingAndGammaCorrectionEffect.Reinhard
            }
        },
        InputSettings { },
        Kuesa.DirectionalLight {
            direction: frameGraph.camera ? frameGraph.camera.viewVector : Qt.vector3d(0, -1, 0)
            intensity: 0.05
        },
        EnvironmentLight {
            irradiance: TextureLoader {
                source: "pink_sunrise_16f_irradiance" + ((!scene.es2) ? ".dds" : "_es2.dds")
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
                generateMipMaps: false
            }
            specular: TextureLoader {
                source: "pink_sunrise_16f_specular" + ((!scene.es2) ? ".dds" : "_es2.dds")
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
                generateMipMaps: false
            }
        },
        FrameAction {
            property var curRow: -1;
            readonly property var prongAngleOffset: 0.5 * Math.PI;
            onTriggered: {
                if (!d.animationRunning)
                    return;

                var matrix = scene.transformForEntity('musicBarrel').matrix;
                var c = matrix.m22; // cosine
                var s = matrix.m32; // sine
                var angle = -Math.atan2(s, c);
                if (angle < 0)
                    angle += 2 * Math.PI;

                var prongAngle = angle + prongAngleOffset;

                var row = Math.floor(prongAngle * d.musicRows.length / (2 * Math.PI)) % d.musicRows.length;
                if (row != curRow) {
                    curRow = row;
                    var rowNotes = d.musicRows[curRow];
                    for (var i = 0; i < d.prongAnimations.length; ++i) {
                        if (rowNotes.charAt(i) == 'x') {
                            var animation = d.prongAnimations[i];
                            animation.setNormalizedTime(0);
                            animation.start();
                            sampler.note(i, 0.5);
                        }
                    }
                }
            }
        }
    ]

    Camera {
        id: mainCamera
        fieldOfView: 35
        position: Qt.vector3d(-3, 2, -3)
        upVector: Qt.vector3d(0, 1, 0)
        viewCenter: Qt.vector3d(-1.37, -0.112, 1.964) // center of music box
    }

    CameraController {
        camera: mainCamera
    }

//!    [1]
    Kuesa.GLTF2Importer {
        id: importer
        sceneEntity: scene
        source: "RobotArm.gltf"
//!    [1]
//!    [2.3]
        onStatusChanged: {
            if (status == Kuesa.GLTF2Importer.Ready) {
                // start robot arm animation
                actionPlayers.model = d.robotArmAnimations;

                // get transforms for music barrel
                var musicBarrelTransform = scene.transformForEntity('musicBarrel');
                musicBarrelTransform.defaultPropertyTrackingMode = Node.TrackAllValues;

                d.animationRunning = true
            }
        }
//!    [2.3]
    }

    Kuesa.Skybox {
        id: skybox
        baseName: "qrc:/pink_sunrise_skybox"
        extension: ".dds"
    }

    // robot arm animation
//!    [2.2]
    NodeInstantiator {
        id: actionPlayers
        model: []
        delegate: Kuesa.AnimationPlayer {
            sceneEntity: scene
            clip: modelData
            loops: Kuesa.AnimationPlayer.Infinite
            running: d.animationRunning
        }
    }
//!    [2.2]

    // prong animations
    Kuesa.AnimationPlayer { id: prong00Action; sceneEntity: scene; clip: 'Prong00Action' }
    Kuesa.AnimationPlayer { id: prong01Action; sceneEntity: scene; clip: 'Prong01Action' }
    Kuesa.AnimationPlayer { id: prong02Action; sceneEntity: scene; clip: 'Prong02Action' }
    Kuesa.AnimationPlayer { id: prong03Action; sceneEntity: scene; clip: 'Prong03Action' }
    Kuesa.AnimationPlayer { id: prong04Action; sceneEntity: scene; clip: 'Prong04Action' }
    Kuesa.AnimationPlayer { id: prong05Action; sceneEntity: scene; clip: 'Prong05Action' }
    Kuesa.AnimationPlayer { id: prong06Action; sceneEntity: scene; clip: 'Prong06Action' }
    Kuesa.AnimationPlayer { id: prong07Action; sceneEntity: scene; clip: 'Prong07Action' }
    Kuesa.AnimationPlayer { id: prong08Action; sceneEntity: scene; clip: 'Prong08Action' }
    Kuesa.AnimationPlayer { id: prong09Action; sceneEntity: scene; clip: 'Prong09Action' }
    Kuesa.AnimationPlayer { id: prong10Action; sceneEntity: scene; clip: 'Prong10Action' }
    Kuesa.AnimationPlayer { id: prong11Action; sceneEntity: scene; clip: 'Prong11Action' }
    Kuesa.AnimationPlayer { id: prong12Action; sceneEntity: scene; clip: 'Prong12Action' }
    Kuesa.AnimationPlayer { id: prong13Action; sceneEntity: scene; clip: 'Prong13Action' }
    Kuesa.AnimationPlayer { id: prong14Action; sceneEntity: scene; clip: 'Prong14Action' }
    Kuesa.AnimationPlayer { id: prong15Action; sceneEntity: scene; clip: 'Prong15Action' }
    Kuesa.AnimationPlayer { id: prong16Action; sceneEntity: scene; clip: 'Prong16Action' }
    Kuesa.AnimationPlayer { id: prong17Action; sceneEntity: scene; clip: 'Prong17Action' }
    Kuesa.AnimationPlayer { id: prong18Action; sceneEntity: scene; clip: 'Prong18Action' }
    Kuesa.AnimationPlayer { id: prong19Action; sceneEntity: scene; clip: 'Prong19Action' }
    Kuesa.AnimationPlayer { id: prong20Action; sceneEntity: scene; clip: 'Prong20Action' }
    Kuesa.AnimationPlayer { id: prong21Action; sceneEntity: scene; clip: 'Prong21Action' }
    Kuesa.AnimationPlayer { id: prong22Action; sceneEntity: scene; clip: 'Prong22Action' }
    Kuesa.AnimationPlayer { id: prong23Action; sceneEntity: scene; clip: 'Prong23Action' }
    Kuesa.AnimationPlayer { id: prong24Action; sceneEntity: scene; clip: 'Prong24Action' }
    Kuesa.AnimationPlayer { id: prong25Action; sceneEntity: scene; clip: 'Prong25Action' }
    Kuesa.AnimationPlayer { id: prong26Action; sceneEntity: scene; clip: 'Prong26Action' }
    Kuesa.AnimationPlayer { id: prong27Action; sceneEntity: scene; clip: 'Prong27Action' }
    Kuesa.AnimationPlayer { id: prong28Action; sceneEntity: scene; clip: 'Prong28Action' }
    Kuesa.AnimationPlayer { id: prong29Action; sceneEntity: scene; clip: 'Prong29Action' }

    MusicBox.Sampler {
        id: sampler
        notes: [
            "samples/114.wav",
            "samples/112.wav",
            "samples/110.wav",
            "samples/109.wav",
            "samples/107.wav",
            "samples/105.wav",
            "samples/103.wav",
            "samples/102.wav",
            "samples/100.wav",
            "samples/98.wav",
            "samples/97.wav",
            "samples/95.wav",
            "samples/94.wav",
            "samples/93.wav",
            "samples/91.wav",
            "samples/90.wav",
            "samples/88.wav",
            "samples/86.wav",
            "samples/85.wav",
            "samples/83.wav",
            "samples/82.wav",
            "samples/81.wav",
            "samples/79.wav",
            "samples/78.wav",
            "samples/76.wav",
            "samples/74.wav",
            "samples/71.wav",
            "samples/70.wav",
            "samples/67.wav",
            "samples/66.wav"
        ]

        property real previousRms:0.;
        property real previousPeak: 0.;
        onAudioSignal: {
            // Minimal low-pass filtering
            var newRms = Math.log(1. + rms * previousRms) / 200.
            var newPeak = Math.log(1. + peak * previousPeak) / 200.
            previousRms = rms
            previousPeak = peak

            soundFxRadius = newRms
        }
    }

    property real soundFxRadius: 0.
    Kuesa.MetallicRoughnessMaterial {
        id: material
        effect: Kuesa.MetallicRoughnessEffect { }
        materialProperties: Kuesa.MetallicRoughnessProperties {
            baseColorFactor: "gray"
        }
    }


    Entity {
        Transform {
            id: torus1Transform
            translation: Qt.vector3d(-0.6, 0.35, 1.95)
            scale3D: Qt.vector3d(300. * soundFxRadius, 300. * soundFxRadius, 300. * soundFxRadius)

            Behavior on scale3D {
                Vector3dAnimation { }
            }

            rotationY: 100
        }
        TorusMesh {
            id: torus1Mesh
            radius: 0.1
            minorRadius: 0.01
            rings: 100
            slices: 20
        }
        components: [ torus1Mesh, material, torus1Transform ]
    }
    Entity {
        Transform {
            id: torus2Transform
            translation: Qt.vector3d(-0.7, 0.35, 1.95)
            scale3D: Qt.vector3d(200. * soundFxRadius, 200. * soundFxRadius, 200. * soundFxRadius)

            Behavior on scale3D {
                Vector3dAnimation { }
            }

            rotationY: 100
        }
        TorusMesh {
            id: torus2Mesh
            radius: 0.1
            minorRadius: 0.02
            rings: 100
            slices: 20
        }
        components: [ torus2Mesh, material, torus2Transform ]
    }
    Entity {
        Transform {
            id: torus3transform
            translation: Qt.vector3d(-.8, 0.35, 1.95)
            scale3D: Qt.vector3d(100. * soundFxRadius, 100. * soundFxRadius, 100. * soundFxRadius)

            Behavior on scale3D {
                Vector3dAnimation { }
            }

            rotationY: 100
        }
        TorusMesh {
            id: torus3Mesh
            radius: 0.1
            minorRadius: 0.05
            rings: 100
            slices: 20
        }
        components: [ torus3Mesh, material, torus3transform ]
    }
}
