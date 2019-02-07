/*
    TexturePreview.qml

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

import Qt3D.Core 2.12
import Qt3D.Render 2.12
import Qt3D.Input 2.12
import Qt3D.Extras 2.12
import QtQml 2.12
import QtQuick.Controls.Material 2.4 as QQ2Material

Entity {
    id: root3D
    property string filePath
    property int mode: _PLANE_MODE
    readonly property int _PLANE_MODE: 1
    readonly property int _SKYBOX_MODE: 2
    property real exposure: 2.5

    onModeChanged: {
        camera.position = Qt.vector3d(0, 0, -5)
        camera.upVector = Qt.vector3d(0, 1, 0)
        camera.viewCenter = Qt.vector3d(0, 0, 0)
    }

    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                camera: camera
                clearColor: QQ2Material.Material.color(QQ2Material.Material.Grey)
            }
        },
        InputSettings {}
    ]

    TextureLoader {
        id: ddsTexture
        source: filePath
    }

    Camera {
        id: camera
        fieldOfView: 40
        position: Qt.vector3d(0, 0, -5)
        upVector: Qt.vector3d(0, 1, 0)
        viewCenter: Qt.vector3d(0, 0, 0)
    }

    OrbitCameraController {
        camera: camera
    }

    Entity {
        enabled: mode === _PLANE_MODE
        readonly property PlaneMesh mesh: PlaneMesh {
            width: 2.0
            height: 2.0
            meshResolution: Qt.size(2, 2)
        }
        readonly property Material material: Material {
            parameters: [
                Parameter { name: "tex"; value: ddsTexture},
                Parameter { name: "exposure"; value: root3D.exposure}
            ]
            effect: Effect {
                techniques: Technique {
                    graphicsApiFilter {
                        api: GraphicsApiFilter.OpenGL
                        majorVersion: 3
                        minorVersion: 2
                        profile: GraphicsApiFilter.CoreProfile
                    }
                    filterKeys: FilterKey { name: "renderingStyle"; value: "forward" }
                    renderPasses: RenderPass {
                        shaderProgram: ShaderProgram {
                            vertexShaderCode: "#version 150
                                               in vec3 vertexPosition;
                                               in vec2 vertexTexCoord;
                                               out vec2 texCoord;
                                               uniform mat4 mvp;
                                               void main() {
                                                    texCoord = vertexTexCoord;
                                                    gl_Position = mvp * vec4(vertexPosition, 1.0);
                                               }
                                              "
                            fragmentShaderCode: "#version 150
                                                 in vec2 texCoord;
                                                 out vec4 fragColor;
                                                 uniform sampler2D tex;
                                                 uniform float exposure;
                                                 uniform float gamma = 2.2;
                                                 uniform float gammaStrength = 0.0;
                                                 void main() {
                                                    vec3 color = texture(tex, vec2(1.0) - texCoord).xyz;
                                                    // Exposure
                                                    color *= pow(2.0, exposure);
                                                    // Tone Mapping
                                                    vec4 toneMapped = vec4(color / (color + vec3(1.0)), 1.0);
                                                    // Gammara correct
                                                    vec4 gammaColor = vec4(pow(toneMapped.rgb, vec3(1.0 / gamma)), 1.0);
                                                    fragColor = mix(toneMapped, gammaColor, gammaStrength);
                                                 }
                                                 "
                        }
                    }
                }
            }
        }
        readonly property Transform transform: Transform {
            rotationX: -90
        }
        readonly property PointLight light: PointLight {}
        components: [mesh, material, transform, light]
    }

    Entity {
        enabled: mode === _SKYBOX_MODE

        readonly property CuboidMesh mesh: CuboidMesh {
            xyMeshResolution: Qt.size(2, 2)
            xzMeshResolution: Qt.size(2, 2)
            yzMeshResolution: Qt.size(2, 2)
        }

        readonly property Material material: Material {
            parameters: [
                Parameter { name: "tex"; value: ddsTexture},
                Parameter { name: "exposure"; value: root3D.exposure}
            ]
            effect: Effect {
                techniques: Technique {
                    graphicsApiFilter {
                        api: GraphicsApiFilter.OpenGL
                        majorVersion: 3
                        minorVersion: 2
                        profile: GraphicsApiFilter.CoreProfile
                    }
                    filterKeys: FilterKey { name: "renderingStyle"; value: "forward" }
                    renderPasses: RenderPass {
                        shaderProgram: ShaderProgram {
                            vertexShaderCode: "#version 150
                                               in vec3 vertexPosition;
                                               out vec3 texCoord;
                                               uniform mat4 viewMatrix;
                                               uniform mat4 projectionMatrix;
                                               uniform mat4 modelMatrix;
                                               void main() {
                                                    texCoord = vertexPosition;
                                                    gl_Position = vec4(projectionMatrix * mat4(mat3(viewMatrix)) * modelMatrix * vec4(vertexPosition, 1.0)).xyww;
                                               }
                                              "
                            fragmentShaderCode: "#version 150
                                                 in vec3 texCoord;
                                                 out vec4 fragColor;
                                                 uniform samplerCube tex;
                                                 uniform float exposure;
                                                 uniform float gamma = 2.2;
                                                 uniform float gammaStrength = 0.0;
                                                 void main() {
                                                    vec3 color = texture(tex, texCoord).xyz;
                                                    // Exposure
                                                    color *= pow(2.0, exposure);
                                                    // Tone Mapping
                                                    vec4 toneMapped = vec4(color / (color + vec3(1.0)), 1.0);
                                                    // Gammara correct
                                                    vec4 gammaColor = vec4(pow(toneMapped.rgb, vec3(1.0 / gamma)), 1.0);
                                                    fragColor = mix(toneMapped, gammaColor, gammaStrength);
                                                 }
                                                 "
                        }
                        renderStates: [
                            DepthTest { depthFunction: DepthTest.LessOrEqual },
                            CullFace { mode: CullFace.Front },
                            SeamlessCubemap {}
                        ]
                    }
                }
            }
        }
        components: [mesh, material]
    }

}
