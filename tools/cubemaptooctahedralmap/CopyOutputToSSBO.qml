/*
    CopyOutputToSSBO.qml

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
import Tools 1.0

Entity {
    id: root
    property Texture2D fboTexture
    readonly property bool isInitialized: ssboBuffer.isInitialized

    SSBOBuffer {
        id: ssboBuffer
        isInteractive: _isInteractive
        textureSize: Qt.size(fboTexture.width, fboTexture.height)
        outputFileName: _outputPath
    }

    Material {
        id: computeMaterial

        parameters: [
            Parameter { name: "fbo"; value: root.fboTexture },
            Parameter { name: "Pixels"; value: ssboBuffer }
        ]

        effect: Effect {
            techniques: Technique {
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    profile: GraphicsApiFilter.CoreProfile
                    majorVersion: 4
                    minorVersion: 3
                }
                renderPasses: RenderPass {
                    shaderProgram: ShaderProgram {
                        computeShaderCode: "
                            #version 430 core

                            layout (local_size_x = 32, local_size_y = 32) in;
                            layout (std430, binding = 0) coherent buffer Pixels
                            {
                                vec4 pixels[];
                            } data;

                            uniform sampler2D fbo;

                            shared uint ssboLength;
                            shared uint textureWidth;
                            void main(void) {
                                uint textureX = gl_GlobalInvocationID.x;
                                uint textureY = gl_GlobalInvocationID.y;

                                // Store ssboLength once per workgroup
                                if (gl_LocalInvocationID.xy == uvec2(0, 0)) {
                                    ssboLength = data.pixels.length();
                                    textureWidth = textureSize(fbo, 0).x;
                                }

                                // Wait for all invocations in this workGroup to have reached this point
                                barrier();

                                uint ssboIdx = textureY * textureWidth + textureX;
                                // Have we reached the length of our buffer yet?
                                if (ssboIdx > ssboLength || textureX > textureWidth)
                                    return;

                                data.pixels[ssboIdx] = texelFetch(fbo, ivec2(textureX, textureY), 0);
                            }
                        "
                    }
                }
            }
        }
    }

    ComputeCommand {
        id: computeCommand
        enabled: !ssboBuffer.isComplete
        workGroupX: Math.ceil(fboTexture.width / 32)
        workGroupY: Math.ceil(fboTexture.height / 32)
        workGroupZ: 1
    }

    components: [computeCommand, computeMaterial]
}
