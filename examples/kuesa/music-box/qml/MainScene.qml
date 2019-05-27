import Qt3D.Core 2.10
import Qt3D.Render 2.11
import Qt3D.Input 2.0
import Qt3D.Logic 2.0
import Qt3D.Extras 2.11
import Qt3D.Animation 2.10
import QtQml 2.2

import Kuesa 1.0 as Kuesa

Kuesa.SceneEntity {
    id: scene

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
            }
        },
        InputSettings { },
        DirectionalLight {
            worldDirection: frameGraph.camera ? frameGraph.camera.viewVector : Qt.vector3d(0, -1, 0)
            intensity: 0.05
        },
        EnvironmentLight {
            irradiance: TextureLoader {
                source: "qrc:/pink_sunrise_irradiance.dds"
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
                generateMipMaps: false
            }
            specular: TextureLoader {
                source: "qrc:/pink_sunrise_specular.dds"
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

    Kuesa.GLTF2Importer {
        id: importer
        sceneEntity: scene
        source: "qrc:/RobotArm.gltf"
        onStatusChanged: {
            if (status == Kuesa.GLTF2Importer.Ready) {
                console.log('Animation clips: ' + scene.animationClips.names);

                // start robot arm animation
                actionPlayers.model = d.robotArmAnimations;

                // get transforms for music barrel
                var musicBarrelTransform = scene.transformForEntity('musicBarrel');
                musicBarrelTransform.defaultPropertyTrackingMode = Node.TrackAllValues;

                d.animationRunning = true
            }
        }
    }

    Kuesa.Skybox {
        id: skybox
        baseName: "qrc:/pink_sunrise_skybox"
        extension: ".dds"
    }

    // robot arm animation
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
}
