/*
    kuesa_reflectedViewMatrix.inc

    This file is part of Kuesa.

    Copyright (C) 2019-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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


mat4 reflectionMatrix(vec4 n)
{
    vec4 n2 = n * n;
    // see https://www.euclideanspace.com/maths/geometry/affine/reflection/matrix/index.htm
    // http://khayyam.kaplinski.com/2011/09/reflective-water-with-glsl-part-i.html
    // https://developer.arm.com/documentation/100140/0302/advanced-graphics-techniques/combining-reflections/about-combining-reflections
    return mat4( 1.0 - 2.0 * n2.x,   -2.0 * n.x * n.y,     -2.0 * n.x * n.z,    0.0,
                -2.0 * n.x * n.y,     1.0 - 2.0 * n2.y,    -2.0 * n.y * n.z,    0.0,
                -2.0 * n.x * n.z,    -2.0 * n.y * n.z,      1.0 - 2.0 * n2.z,   0.0,
                -2.0 * n.x * n.w ,   -2.0 * n.y * n.w,     -2.0 * n.z * n.w,    1.0);
}

mat4 kuesa_reflectedViewMatrix(mat4 viewM, vec4 planeEquation, bool enabled)
{
    if (!enabled)
        return viewM;

    // planeEquation is (nx, ny, nz, w) where (x * nx + y * ny + z * nz + w = 0)
    return viewM * reflectionMatrix(planeEquation);
}
