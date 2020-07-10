/*
    kuesa_morphtargets_position.inc.vert

    This file is part of Kuesa.

    Copyright (C) 2019-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

layout(location = 9) in vec3 vertexPosition_1;
layout(location = 10) in vec3 vertexPosition_2;
// Note: it is assumed the morphWeights structure uniform has been defined
// elsewhere and included prior to this

vec3 kuesa_morphPosition(const in vec3 vPosition)
{
    vec3 pos = vPosition;
    pos += vertexPosition_1 * morphWeights.morphWeights[0];
    pos += vertexPosition_2 * morphWeights.morphWeights[1];
    return pos;
}
