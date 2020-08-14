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

#version 330

const float sharpTextureCutoff = .2;
const float dofTextureCutoff = 1;

uniform float nearPlane;
uniform float farPlane;
uniform sampler2D depthTexture;
uniform float focusDistance;
uniform float focusRange;
uniform float bokehRadius;
uniform sampler2D textureSampler;
uniform sampler2D dofTexture;
in vec2 texCoord;

layout(location = 0) out vec4 fragColor;

float mapZ(float z)
{
    float a = ((-1.0) + (nearPlane / farPlane)) / nearPlane;
    float b = 1.0 / nearPlane;
    return 1.0 / ((a * z) + b);
}

float confusionCircle()
{
    float param = texture(depthTexture, texCoord).x;
    float depth = mapZ(param);
    // points in front: negative coc. points behind: positive coc
    float coc = (depth - focusDistance) / focusRange;
    coc = clamp(coc, -1.0, 1.0) * bokehRadius;
    return coc;
}

void main()
{
    float coc = confusionCircle();
    vec4 dofTextureValue = texture(dofTexture, texCoord);
    vec4 sharpTextureValue = texture(textureSampler, texCoord);

    // relative strength of sharp texture vs blurred textures based on CoC
    float dofTexStrength = smoothstep(sharpTextureCutoff, dofTextureCutoff, abs(coc));

    // now adjust blending of sharp and blurred tetures based on background-foreground value
    // stored in alpha channel. If blurred dofTexture contributes, the amount depends on
    // whether the texture value belongs to foreground or background:
    // Foreground--use fg. blurred objects in fg do obscure sharp things behind
    // Background--use sharp texture. blurred objects in bg do NOT blur into sharp objects in front.
    float mixValue = dofTexStrength + dofTextureValue.a  - dofTexStrength * dofTextureValue.a;

    vec4 finalColor = mix(sharpTextureValue, dofTextureValue, mixValue);
    fragColor = vec4(finalColor.rgb, 1.0);
}

