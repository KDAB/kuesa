/*
    main.qml

    This file is part of Kuesa.

    Copyright (C) 2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

import QtQuick.Scene3D 2.0
import QtQuick 2.11

import Qt3D.Core 2.10
import Qt3D.Render 2.11
import Qt3D.Input 2.0
import Qt3D.Extras 2.10

import Kuesa 1.3 as Kuesa
import Controllers 1.0 as Controllers

Scene3D {
    id: scene3D
    anchors.fill: parent
    aspects: ["input", "logic"]

    Kuesa.SceneEntity {
        Camera {
            id: mainCamera
            projectionType: CameraLens.PerspectiveProjection
            fieldOfView: 45
            aspectRatio: 1
            nearPlane : 0.1
            farPlane : 1000.0
            position: Qt.vector3d(0.0, 2.0, 3.0)
            upVector: Qt.vector3d(0.0, 1.0, 0.0)
            viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
        }

        Controllers.OrbitCameraController {
            camera: mainCamera
        }

        components: [
            RenderSettings {
                activeFrameGraph: Kuesa.ForwardRenderer {
                    id: frameGraph
                    camera: mainCamera
                    particlesEnabled: true
                }
            },
            InputSettings { }
        ]

        Entity {
            components: [
                Kuesa.DirectionalLight { intensity: 2 }
            ]
        }

        Entity {
            components: [
                PlaneMesh {
                    width: 4
                    height: 4
                },
                Transform {
                    translation: Qt.vector3d(0, -0.5, 0)
                },
                Kuesa.MetallicRoughnessMaterial {
                    effect: Kuesa.MetallicRoughnessEffect { }
                    materialProperties: Kuesa.MetallicRoughnessProperties {
                        baseColorFactor: Qt.rgba(1, 1, 1)
                        metallicFactor: .5
                        roughnessFactor: 0.25
                    }
                }
            ]
        }

        Kuesa.Particles {
            id: particles
            particleCount: _controller.particleCount
            maxParticlesEmittedPerFrame: _controller.maxParticlesEmittedPerFrame
            gravity: _controller.gravity
            emitterPosition: _controller.emitterPosition
            emitterPositionRandom: _controller.emitterPositionRandom
            emitterVelocity: _controller.emitterVelocity
            emitterVelocityRandom: _controller.emitterVelocityRandom
            particleLifespan: _controller.particleLifespan
            particleLifespanRandom: _controller.particleLifespanRandom
            initialSize: _controller.initialSize
            finalSize: _controller.finalSize
            initialColor: _controller.initialColor
            finalColor: _controller.finalColor
            initialAngle: _controller.initialAngle
            initialAngleRandom: _controller.initialAngleRandom
            rotationRate: _controller.rotationRate
            rotationRateRandom: _controller.rotationRateRandom
            spriteTexture: TextureLoader {
                source: "kuesa-logo.png"
            }
        }
    }
}
