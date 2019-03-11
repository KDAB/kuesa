/*
    skybox.frag

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mike Krus <mike.krus@kdab.com>

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

precision highp float;

varying highp vec3 texCoord0;
uniform samplerCube skyboxTexture;

// Gamma correction
uniform float gamma;

uniform float gammaStrength;

vec3 gammaCorrect(const in vec3 color)
{
    return pow(color, vec3(1.0 / gamma));
}

vec3 toneMap(const in vec3 c)
{
    return c / (c + vec3(1.0));
}

void main()
{
    vec4 baseColor = textureCube(skyboxTexture, texCoord0);
    vec4 cToneMapped = vec4(toneMap(baseColor.rgb), 1.0);
    vec4 gammaColor = vec4(gammaCorrect(cToneMapped.rgb), 1.0);
    // This is an odd way to enable or not gamma correction,
    // but this is a way to avoid branching until we can generate shaders
    gl_FragColor = mix(cToneMapped, gammaColor, gammaStrength);
}

