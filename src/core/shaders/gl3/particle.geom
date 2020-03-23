#version 430 core

/*
    particle.geom

    This file is part of Kuesa.

    Copyright (C) 2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mauro Persano <mauro.persano@kdab.com>

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

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

uniform vec3 eyePosition;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

in WorldVertex {
    bool alive;
    vec4 position;
    vec4 velocity;
    vec2 size;
    vec4 color;
    float rotation;
} gs_in[];

out SpriteVertex {
    vec2 texCoord;
    vec4 color;
} gs_out;

void emitVertex(vec3 position, vec2 offs, vec2 uv, vec4 color)
{
    position.xy += offs;
    gl_Position = projectionMatrix * vec4(position, 1.0);
    gs_out.texCoord = uv;
    gs_out.color = color;
    EmitVertex();
}

void main(void)
{
    if (gs_in[0].alive == false)
        return;

    vec3 worldPosition = gs_in[0].position.xyz;
    vec3 viewSpacePosition = (viewMatrix * vec4(worldPosition, 1.0)).xyz;

    vec4 color = gs_in[0].color;

    float w = gs_in[0].size.x;
    float h = gs_in[0].size.y;

    float c = cos(gs_in[0].rotation);
    float s = sin(gs_in[0].rotation);
    mat2 r = mat2(c, s, -s, c);

    emitVertex(viewSpacePosition, r * vec2(-0.5 * w, -0.5 * h), vec2(0, 0), color);
    emitVertex(viewSpacePosition, r * vec2(0.5 * w, -0.5 * h), vec2(1, 0), color);
    emitVertex(viewSpacePosition, r * vec2(-0.5 * w, 0.5 * h), vec2(0, 1), color);
    emitVertex(viewSpacePosition, r * vec2(0.5 * w, 0.5 * h), vec2(1, 1), color);
}
