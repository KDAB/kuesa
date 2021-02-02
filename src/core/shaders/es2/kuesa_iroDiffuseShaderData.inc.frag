/*
    kuesa_iroDiffuseShaderData.inc.frag

    This file is part of Kuesa.

    Copyright (C) 2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Nicolas Guichard <nicolas.guichard@kdab.com>

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

struct MaterialProperties {
    highp vec3 normalScaling;
    highp vec2 normalDisturb;

    highp sampler2D reflectionMap;
    highp float reflectionGain;
    highp vec3 reflectionInnerFilter;
    highp vec3 reflectionOuterFilter;
    bool usesReflectionMap;

    highp sampler2D diffuseMap;
    highp vec3 diffuseInnerFilter;
    highp vec3 diffuseOuterFilter;
    highp float diffuseGain;
    bool usesDiffuseMap;

    highp float postVertexColor;
    highp float postGain;
};

uniform MaterialProperties properties;
