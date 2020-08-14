/*
    dof_composite.frag

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#version 100
precision mediump float;
precision highp int;

const highp float sharpTextureCutoff = 0.2;
const highp float dofTextureCutoff = 1.0;

uniform highp float nearPlane;
uniform highp float farPlane;
uniform highp sampler2D depthTexture;
uniform highp float focusDistance;
uniform highp float focusRange;
uniform highp float bokehRadius;
uniform highp sampler2D textureSampler;
uniform highp sampler2D dofTexture;

varying highp vec2 texCoord;

highp float mapZ(highp float z)
{
    highp float a = ((-1.0) + (nearPlane / farPlane)) / nearPlane;
    highp float b = 1.0 / nearPlane;
    return 1.0 / ((a * z) + b);
}

highp float confusionCircle()
{
    highp float param = texture2D(depthTexture, texCoord).x;
    highp float depth = mapZ(param);

    // points in front: negative coc. points behind: positive coc
    highp float coc = (depth - focusDistance) / focusRange;
    coc = clamp(coc, -1.0, 1.0) * bokehRadius;
    return coc;
}

void main()
{
    highp float coc = confusionCircle();
    highp vec4 dofTextureValue = texture2D(dofTexture, texCoord);
    highp vec4 sharpTextureValue = texture2D(textureSampler, texCoord);

    // relative strength of sharp texture vs blurred textures based on CoC
    highp float dofTexStrength = smoothstep(sharpTextureCutoff, dofTextureCutoff, abs(coc));

    // now adjust blending of sharp and blurred tetures based on background-foreground value
    // stored in alpha channel. If blurred dofTexture contributes, the amount depends on
    // whether the texture value belongs to foreground or background:
    // Foreground--use fg. blurred objects in fg do obscure sharp things behind
    // Background--use sharp texture. blurred objects in bg do NOT blur into sharp objects in front.
    highp float mixValue = dofTexStrength + dofTextureValue.a  - dofTexStrength * dofTextureValue.a;

    highp vec4 finalColor = mix(sharpTextureValue, dofTextureValue, mixValue);
    gl_FragData[0] = vec4(finalColor.rgb, 1.0);
}

