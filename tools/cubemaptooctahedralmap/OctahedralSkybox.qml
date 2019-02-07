/*
    OctahedralSkybox.qml

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
import Qt3D.Extras 2.12

// Draw Skybox preview
Entity {
    property alias octahedralTexture: octahedralTextureParam.value
    property Layer layer

    components: [cubeMesh, material, layer]

    CuboidMesh {
        id: cubeMesh
        xyMeshResolution: Qt.size(2, 2)
        yzMeshResolution: Qt.size(2, 2)
        xzMeshResolution: Qt.size(2, 2)
    }

    Material {
        id: material
        parameters: [
            Parameter { id: octahedralTextureParam; name: "octahedralTexture" }
        ]
        effect: Effect {
            techniques: Technique {
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    profile: GraphicsApiFilter.CoreProfile
                    majorVersion: 3
                    minorVersion: 2
                }
                renderPasses: RenderPass {
                    shaderProgram: ShaderProgram {
                        vertexShaderCode: "
                            #version 150 core

                            uniform mat4 modelMatrix;
                            uniform mat4 viewMatrix;
                            uniform mat4 projectionMatrix;

                            in vec3 vertexPosition;
                            out vec3 texCoords;

                            void main(void) {
                                texCoords = vertexPosition;
                                // Converting the viewMatrix to a mat3, then back to a mat4
                                // removes the translation component from it
                                gl_Position = vec4(projectionMatrix * mat4(mat3(viewMatrix)) * modelMatrix * vec4(vertexPosition, 1.0)).xyww;
                            }
                        "
                        fragmentShaderCode: "
                            #version 150 core

                            uniform sampler2D octahedralTexture;
                            in vec3 texCoords;
                            out vec4 fragColor;

                            vec2 octohedralProjection(vec3 dir) {
                                dir /= dot(vec3(1.0), abs(dir));
                                vec2 rev = abs(dir.zx) - vec2(1.0);
                                vec2 neg = -sign(dir).xz * rev.xy;
                                vec2 uv = dir.y < 0.0 ? neg : dir.xz;
                                return 0.5 * uv + vec2(0.5);
                            }

                            vec4 sampleTextureAtLevel(vec2 texCoords, int level)
                            {
                                // X,Y coordinates are in the 0 - 1 range

                                // Level 0 goes from 0 - 0.66 on X and 0 - 1 on Y
                                // Level 1 goes from 0.66 - 1.0 on X and 0 - 0.5 on Y
                                // Level 2 goes from 0.66 - 0.83 on X and 0.5 - 0.75 on Y
                                // Level 3 goes from 0.66 - 0.745 on X and 0.75 - 0.875 on Y

                                float levelF = float(level);

                                // Bring coordinates into the proper range for level
                                vec2 offsetCoords = vec2(texCoords.x * 2.0 / 3.0, texCoords.y) / pow(2.0, levelF);

                                // Offset coordinates into proper area
                                offsetCoords += vec2(step(1.0, levelF) * 2.0 / 3.0,
                                                     step(2.0, levelF) * (1.0 - 1.0 / float(pow(2, level - 1))));

                                return texture(octahedralTexture, offsetCoords);
                            }

                            void main(void) {
                                fragColor = sampleTextureAtLevel(octohedralProjection(texCoords), 0);
                            }
                        "
                    }
                }
            }
        }
    }
}
