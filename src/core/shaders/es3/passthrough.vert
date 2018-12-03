#version 300 es

in vec3 vertexPosition;
in vec2 vertexTexCoord;

out vec2 texCoord;

uniform mat4 mvp;

void main()
{
    // Pass through the texture coords
    texCoord = vertexTexCoord;

    // Calculate the clip-space coordinates
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
