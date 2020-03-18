/*
    kuesa_metallicRoughnessShaderData.inc.frag

    This file is part of Kuesa.

    Copyright (C) 2019-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Casafranca <juan.casafranca@kdab.com>

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

struct MetallicRoughness {
    vec4 baseColorFactor;
    vec4 emissiveFactor;

    sampler2D baseColorMap;
    sampler2D metalRoughMap;
    sampler2D normalMap;
    sampler2D ambientOcclusionMap;
    sampler2D emissiveMap;

    float metallicFactor;
    float roughnessFactor;
    float normalScale;
    float alphaCutoff;

    bool baseColorUsesTexCoord1;
    bool metallicRoughnessUsesTexCoord1;
    bool normalUsesTexCoord1;
    bool aoUsesTexCoord1;
    bool emissiveUsesTexCoord1;
};

uniform MetallicRoughness metallicRoughness;
