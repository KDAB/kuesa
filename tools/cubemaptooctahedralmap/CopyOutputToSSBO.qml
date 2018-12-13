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
