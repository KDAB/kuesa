/*
    light.inc.frag

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Paul Lemire <paul.lemire@kdab.com>

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

#ifndef FP
#define FP highp
#endif

const int MAX_LIGHTS = 8;
const int TYPE_POINT = 0;
const int TYPE_DIRECTIONAL = 1;
const int TYPE_SPOT = 2;
struct Light {
    int type;
    FP vec3 position;
    FP vec3 color;
    FP float intensity;
    FP vec3 direction;
    FP float constantAttenuation;
    FP float linearAttenuation;
    FP float quadraticAttenuation;
    FP float cutOffAngle;
};
uniform Light lights[MAX_LIGHTS];
uniform int lightCount;

// Pre-convolved environment maps
struct EnvironmentLight {
    FP sampler2D irradiance; // For diffuse contribution
    FP sampler2D specular; // For specular contribution
    FP vec3 irradianceSize;
    FP vec3 specularSize;
};
uniform EnvironmentLight envLight;
uniform int envLightCount;
