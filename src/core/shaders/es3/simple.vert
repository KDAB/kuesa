/*
    simple.vert

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#version 300 es

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec4 vertexTangent;
in vec4 vertexColor;
in vec2 vertexTexCoord;

out vec3 worldPosition;
out vec3 worldNormal;
out vec4 worldTangent;
out vec4 color;
out vec2 texCoord;

uniform mat4 modelMatrix;
uniform mat3 modelNormalMatrix;
uniform mat4 modelViewProjection;

uniform mat3 texCoordTransform;

void main()
{
    // Pass through scaled texture coordinates
    vec3 transformedTexCoord = texCoordTransform * vec3(vertexTexCoord, 1.0);
    texCoord = transformedTexCoord.xy / transformedTexCoord.z;

    // Pass through vertex colors
    color = vertexColor;

    // Transform position, normal, and tangent to world space
    worldPosition = vec3(modelMatrix * vec4(vertexPosition, 1.0));
    worldNormal = normalize(modelNormalMatrix * vertexNormal);
    worldTangent.xyz = normalize(vec3(modelMatrix * vec4(vertexTangent.xyz, 0.0)));
    worldTangent.w = vertexTangent.w;

    // Calculate vertex position in clip coordinates
    gl_Position = modelViewProjection * vec4(vertexPosition, 1.0);
}
