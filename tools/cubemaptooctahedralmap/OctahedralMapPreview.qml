/*
    OctahedralMapPreview.qml

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

Entity {
    property alias octahedralTexture: octahedralTextureParameter.value
    property Layer layer

    components: [plane, material, transform, layer]

    PlaneMesh {
        id: plane
        width: 2
        height: 2
    }

    Material {
        id: material
        parameters: [
            Parameter { id: octahedralTextureParameter; name: "octahedralTexture" }
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

                            uniform mat4 mvp;
                            in vec3 vertexPosition;
                            in vec2 vertexTexCoord;
                            out vec2 texCoords;

                            void main(void) {
                                texCoords = vertexTexCoord;
                                gl_Position = mvp * vec4(vertexPosition, 1.0);
                            }
                        "
                        fragmentShaderCode: "
                            #version 150 core

                            uniform sampler2D octahedralTexture;
                            in vec2 texCoords;
                            out vec4 fragColor;

                            void main(void) {
                                fragColor = texture(octahedralTexture, texCoords);
                            }
                        "
                    }
                }
            }
        }
    }

    Transform {
        id: transform
        rotationX: 90
    }
}
