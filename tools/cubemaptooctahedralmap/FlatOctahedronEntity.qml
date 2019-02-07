/*
    FlatOctahedronEntity.qml

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

Entity {

    property Layer layer
    property alias cubeMap: cubeMapParam.value

    property int firstLevelWidth: 2048
    readonly property int maxMipsLevel: 1 + Math.floor(Math.log2(firstLevelWidth))

    // See https://photos.app.goo.gl/FtEpv9VKZMtpxJaU9
    function generateFlatOctahedronVertexBufferData() {
        var t = Qt.vector3d(0.0, 1.0, 0.0)
        var b = Qt.vector3d(0.0, -1.0, 0.0)
        var w = Qt.vector3d(-1.0, 0.0, 0.0)
        var e = Qt.vector3d(1.0, 0.0, 0.0)
        var n = Qt.vector3d(0.0, 0.0, -1.0)
        var s = Qt.vector3d(0.0, 0.0, 1.0)

        var vertices = [
                    // Top Left quadrant
                    // Outer - Face 6
                    Qt.vector3d(0.0, 1.0, 0.0),
                    n,
                    Qt.vector3d(-1.0, 1.0, 0.0),
                    b,
                    Qt.vector3d(-1.0, 0.0, 0.0),
                    w,
                    // Inner - Face 2
                    Qt.vector3d(0.0, 1.0, 0.0),
                    n,
                    Qt.vector3d(-1.0, 0.0, 0.0),
                    w,
                    Qt.vector3d(0.0, 0.0, 0.0),
                    t,
                    // Top Right quadrant
                    // Outer - Face 5
                    Qt.vector3d(0.0, 1.0, 0.0),
                    n,
                    Qt.vector3d(1.0, 0.0, 0.0),
                    e,
                    Qt.vector3d(1.0, 1.0, 0.0),
                    b,
                    // Inner - Face 1
                    Qt.vector3d(0.0, 1.0, 0.0),
                    n,
                    Qt.vector3d(0.0, 0.0, 0.0),
                    t,
                    Qt.vector3d(1.0, 0.0, 0.0),
                    e,
                    // Bottom Left quadrant
                    // Outer - Face 7
                    Qt.vector3d(-1.0, 0.0, 0.0),
                    w,
                    Qt.vector3d(-1.0, -1.0, 0.0),
                    b,
                    Qt.vector3d(0.0, -1.0, 0.0),
                    s,
                    // Inner - Face 3
                    Qt.vector3d(-1.0, 0.0, 0.0),
                    w,
                    Qt.vector3d(0.0, -1.0, 0.0),
                    s,
                    Qt.vector3d(0.0, 0.0, 0.0),
                    t,
                    // Bottom Right quadrant
                    // Outer - Face 8
                    Qt.vector3d(0.0, -1.0, 0.0),
                    s,
                    Qt.vector3d(1.0, -1.0, 0.0),
                    b,
                    Qt.vector3d(1.0, 0.0, 0.0),
                    e,
                    // Inner - Face 4
                    Qt.vector3d(0.0, -1.0, 0.0),
                    s,
                    Qt.vector3d(1.0, 0.0, 0.0),
                    e,
                    Qt.vector3d(0.0, 0.0, 0.0),
                    t
                ]

        var rawData = new Float32Array(vertices.length * 3);
        var i = 0;
        vertices.forEach(function(vec3) {
            rawData[i++] = vec3.x;
            rawData[i++] = vec3.y;
            rawData[i++] = vec3.z;
        });
        return rawData
    }

    function generatePerInstanceMipMapMatrixAttribute() {
        var matrices = []
        var scale = 1.0
        var xOffset = -1/3
        var yOffset = 0.0

        var level = 0;
        while (level < maxMipsLevel) {
            var mat = Qt.matrix4x4()
            // Since our texture is 1.5 larger than its original width to accommodate mip maps
            // we need to scale the x axis differently than the y axis
            var xScale = scale * 2 / 3
            mat.translate(Qt.vector3d(xOffset, yOffset, 0))
            mat.scale(xScale, scale, 1)

            if (level == 0) {
                xOffset = 1
                yOffset = -0.5
            }
            xOffset -= xScale * 0.5
            if (level > 0)
                yOffset += scale * 1.5
            scale = scale * 0.5

            matrices.push(mat)
            level += 1
        }

        var rawData = new Float32Array(matrices.length * 16);
        var i = 0;
        matrices.forEach(function(mat4) {
            rawData[i++] = mat4.m11;
            rawData[i++] = mat4.m21;
            rawData[i++] = mat4.m31;
            rawData[i++] = mat4.m41;

            rawData[i++] = mat4.m12;
            rawData[i++] = mat4.m22;
            rawData[i++] = mat4.m32;
            rawData[i++] = mat4.m42;

            rawData[i++] = mat4.m13;
            rawData[i++] = mat4.m23;
            rawData[i++] = mat4.m33;
            rawData[i++] = mat4.m43;

            rawData[i++] = mat4.m14;
            rawData[i++] = mat4.m24;
            rawData[i++] = mat4.m34;
            rawData[i++] = mat4.m44;
        });
        return rawData
    }

    components: [flatOctaHedronGeometryRenderer, cubeMapToOctahedronMaterial, layer]

    Buffer {
        id: flatOctahedronBuffer
        type: Buffer.VertexBuffer
        data: generateFlatOctahedronVertexBufferData()
    }

    Buffer {
        id: flatOctahedronPerInstanceMatrixBuffer
        type: Buffer.VertexBuffer
        data: generatePerInstanceMipMapMatrixAttribute()
    }

    GeometryRenderer {
        id: flatOctaHedronGeometryRenderer
        instanceCount: maxMipsLevel
        primitiveType: GeometryRenderer.Triangles
        geometry:  Geometry {
            id: flatOctaHedronGeometry
            Attribute {
                attributeType: Attribute.VertexAttribute
                vertexBaseType: Attribute.Float
                vertexSize: 3
                byteOffset: 0
                byteStride: 6 * 4 // 6 floats
                count: 24
                name: defaultPositionAttributeName
                buffer: flatOctahedronBuffer
            }
            Attribute {
                attributeType: Attribute.VertexAttribute
                vertexBaseType: Attribute.Float
                vertexSize: 3
                byteOffset: 3 * 4 // 3 floats
                byteStride: 6 * 4 // 6 floats
                count: 24
                name: defaultTextureCoordinateAttributeName
                buffer: flatOctahedronBuffer
            }
            Attribute {
                attributeType: Attribute.VertexAttribute
                vertexBaseType: Attribute.Float
                vertexSize: 16
                name: "mipMapMatrix"
                buffer: flatOctahedronPerInstanceMatrixBuffer
                divisor: 1
            }
        }
    }

    Material {
        id: cubeMapToOctahedronMaterial
        parameters: [
            Parameter { id: cubeMapParam; name: "cubeMap"; }
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

                            in vec3 vertexPosition;
                            in vec3 vertexTexCoord;
                            in mat4 mipMapMatrix;

                            out VertexData {
                                vec3 texCoords;
                            } vertexOut;

                            void main(void) {
                                vertexOut.texCoords = vertexTexCoord;
                                gl_Position = mipMapMatrix * vec4(vertexPosition, 1.0);
                            }
                            "

                        fragmentShaderCode: "
                            #version 150 core

                            uniform samplerCube cubeMap;

                            in VertexData {
                                vec3 texCoords;
                            } fragIn;

                            out vec4 fragColor;

                            void main(void) {
                                fragColor = texture(cubeMap, fragIn.texCoords);
                            }
                            "
                    }
                }
            }
        }
    }
}
