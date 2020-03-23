#version 430 core

/*
    particle.vert

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

#pragma include particle.inc

layout (std430, binding=0) buffer particleBuffer
{
    Particle particles[];
};

layout (location=0) in vec2 vertexPosition;

uniform vec2 initialSize;
uniform vec2 finalSize;
uniform vec4 initialColor;
uniform vec4 finalColor;

out WorldVertex
{
    bool alive;
    vec4 position;
    vec4 velocity;
    vec2 size;
    vec4 color;
    float rotation;
} vs_out;

void main()
{
    int particleIndex = int(vertexPosition.y);
    if (particleIndex < 0)
    {
        vs_out.alive = false;
        return;
    }

    Particle p = particles[particleIndex];

    vs_out.alive = true;
    vs_out.position = p.position;
    vs_out.velocity = p.velocity;
    vs_out.rotation = p.rotation;

    float normalizedAge = max(1.0 - (p.age / p.lifespan), 0.0);
    vs_out.size = mix(initialSize, finalSize, normalizedAge);
    vs_out.color = mix(initialColor, finalColor, normalizedAge);
}
