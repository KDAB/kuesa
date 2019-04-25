#version 100

/*
    skinned.vert

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

attribute vec3 vertexPosition;
attribute vec3 vertexNormal;
attribute vec4 vertexTangent;
attribute vec4 vertexColor;
attribute vec2 vertexTexCoord;
attribute vec2 vertexTexCoord1;

attribute uvec4 vertexJointIndices;
attribute vec4 vertexJointWeights;

varying vec3 worldPosition;
varying vec3 worldNormal;
varying vec4 worldTangent;
varying vec4 color;
varying vec2 texCoord;
varying vec2 texCoord1;

uniform mat4 modelMatrix;
uniform mat3 modelNormalMatrix;
uniform mat4 mvp;

const int maxJoints = 100;
uniform mat4 skinningPalette[maxJoints];

uniform mat3 texCoordTransform;

void main()
{
    // Pass through scaled texture coordinates
    vec3 transformedTexCoord = texCoordTransform * vec3(vertexTexCoord, 1.0);
    texCoord = transformedTexCoord.xy / transformedTexCoord.z;
    
    vec3 transformedTexCoord1 = texCoordTransform * vec3(vertexTexCoord1, 1.0);
    texCoord1 = transformedTexCoord1.xy / transformedTexCoord1.z;

    // Pass through vertex colors
    color = vertexColor;

    // Perform the skinning
    mat4 skinningMatrix = skinningPalette[vertexJointIndices[0]] * vertexJointWeights[0];
    skinningMatrix     += skinningPalette[vertexJointIndices[1]] * vertexJointWeights[1];
    skinningMatrix     += skinningPalette[vertexJointIndices[2]] * vertexJointWeights[2];
    skinningMatrix     += skinningPalette[vertexJointIndices[3]] * vertexJointWeights[3];

    vec4 skinnedPosition = skinningMatrix * vec4(vertexPosition, 1.0);
    vec3 skinnedNormal = vec3(skinningMatrix * vec4(vertexNormal, 0.0));
    vec3 skinnedTangent = vec3(skinningMatrix * vec4(vertexTangent.xyz, 0.0));

    // Transform position, normal, and tangent to world space
    worldPosition = vec3(modelMatrix * skinnedPosition);
    worldNormal = normalize(modelNormalMatrix * skinnedNormal);
    worldTangent.xyz = normalize(vec3(modelMatrix * vec4(skinnedTangent, 0.0)));
    worldTangent.w = vertexTangent.w;

    gl_Position = mvp * skinnedPosition;
}
