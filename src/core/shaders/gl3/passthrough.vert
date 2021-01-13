#version 150

/*
    passthrough.vert

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec2 vertexTexCoord1;

out vec2 texCoord;
out vec2 texCoord1;

uniform mat4 mvp;

void main()
{
    // Pass through the texture coords
    texCoord = vertexTexCoord;
    texCoord1 = vertexTexCoord1;

    // Calculate the clip-space coordinates
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
