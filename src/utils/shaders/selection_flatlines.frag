#version 150 core

out vec4 fragColor;

uniform vec4 lineColor = vec4(1, 1, 1, 1);

void main() {
    fragColor = lineColor;
}
