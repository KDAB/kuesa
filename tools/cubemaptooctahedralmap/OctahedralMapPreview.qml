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
