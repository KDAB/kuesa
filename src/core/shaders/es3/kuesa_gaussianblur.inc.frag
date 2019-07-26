/*
    gaussianblur.frag

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

precision highp float;

// Use non-filtered texture fetches
const int samples = 5;
const float weight[samples] = float[]( 0.2270270270,
                                       0.1945945946,
                                       0.1216216216,
                                       0.0540540541,
                                       0.0162162162 );

vec4 kuesa_gaussianBlur(const in int pass,
                        const in float width,
                        const in float height,
                        const in vec2 texCoord)
{
    vec4 sum;

    if (pass == 1) {
        float dy = 1.0 / height;
        sum = texture(textureSampler, texCoord) * weight[0];
        for (int i = 1; i < samples; i++) {
            sum += texture(textureSampler, texCoord + vec2(0.0, float(i)) * dy) * weight[i];
            sum += texture(textureSampler, texCoord - vec2(0.0, float(i)) * dy) * weight[i];
        }
    } else {
        float dx = 1.0 / width;
        sum = texture(textureSampler, texCoord) * weight[0];
        for (int i = 1; i < samples; i++) {
            sum += texture(textureSampler, texCoord + vec2(float(i), 0.0) * dx) * weight[i];
            sum += texture(textureSampler, texCoord - vec2(float(i), 0.0) * dx) * weight[i];
        }
    }

    return sum;
}
