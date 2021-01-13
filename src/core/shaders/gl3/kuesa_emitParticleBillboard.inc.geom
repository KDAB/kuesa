/*
    kuesa_emitParticleBillboard.inc.geom

    This file is part of Kuesa.

    Copyright (C) 2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

layout(triangle_strip, max_vertices=4) out;

out SpriteVertex {
    vec2 texCoord;
    vec4 color;
} gs_out;

uniform mat4 projectionMatrix;

bool emitBillboard(bool alive, vec3 position, mat2 rotation, vec2 size, vec4 color)
{
    if (!alive) {
        return true;
    }

    const vec2 xyOffsets[4] = vec2[](
        vec2(-1, -1),
        vec2(1, -1),
        vec2(-1, 1),
        vec2(1, 1)
    );

    for (int i = 0; i < 4; i++) {
        vec2 xyOffset = 0.5 * size * xyOffsets[i];
        vec2 texCoord = vec2(0.5) + 0.5 * xyOffsets[i];
        vec3 position = vec3(position.xy + rotation * xyOffset, position.z);

        gl_Position = projectionMatrix * vec4(position, 1.0);
        gs_out.texCoord = texCoord;
        gs_out.color = color;
        EmitVertex();
    }

    return true;
}
