#version 330 core

/*
    shadow_cube.geom

    This file is part of Kuesa.

    Copyright (C) 2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jim Albamont <jim.albamont@kdab.com>

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

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform int lightIndex;
uniform mat4 lightViewProjectionMatrices[6];

in vec3 worldPosition[];

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = 6 * lightIndex + face;
        for(int i = 0; i < 3; ++i)
        {
            gl_Position = lightViewProjectionMatrices[face] * vec4(worldPosition[i], 1.0);
            EmitVertex();
        }
        EndPrimitive();
    }
}
