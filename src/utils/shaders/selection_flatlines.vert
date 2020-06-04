#version 150 core

in vec3 vertexPosition;

uniform mat4 viewProjectionMatrix;
in mat4 selectionMatrix; // The model matrix of the current cube. Varies per instance

void main() {
    gl_Position = vec4(viewProjectionMatrix * selectionMatrix * vec4(vertexPosition, 1.0));
}
