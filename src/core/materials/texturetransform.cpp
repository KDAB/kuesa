/*
    texturetransform.cpp

    This file is part of Kuesa.

    Copyright (C) 2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "texturetransform.h"
#include <cmath>

QT_BEGIN_NAMESPACE

namespace Kuesa {

/*!
    \class Kuesa::TextureTransform
    \inheaderfile Kuesa/TextureTransform
    \inmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::TextureTransform provides an easy to use texture transform
    implementation. Given an offset, a rotation and a scale, it produces a
    matrix that can be directly pushed to a shader and used to transform uv
    coordinates.

    On top of Kuesa::TextureTransform, Kuesa implements KHR_texture_transform

    \sa Kuesa::MetallicRoughnessProperties
    \sa https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_texture_transform
*/

/*!
    \qmltype TextureTransform
    \instantiates Kuesa::TextureTransform
    \inqmlmodule Kuesa
    \since Kuesa 1.3

    \brief Kuesa::TextureTransform provides an easy to use texture transform
    implementation. Given an offset, a rotation and a scale, it produces a
    matrix that can be directly pushed to a shader and used to transform uv
    coordinates.

    On top of Kuesa::TextureTransform, Kuesa implements KHR_texture_transform

    \sa Kuesa::MetallicRoughnessProperties
    \sa https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_texture_transform
 */

/*!
    \property Kuesa::TextureTransform::offset
    \since Kuesa 1.3
 */

/*!
    \property Kuesa::TextureTransform::rotation
    \since Kuesa 1.3
 */

/*!
    \property Kuesa::TextureTransform::scale
    \since Kuesa 1.3
 */

/*!
    \property Kuesa::TextureTransform::matrix
    \since Kuesa 1.3
 */

/*!
    \qmlproperty vector2d Kuesa::TextureTransform::offset
    \since Kuesa 1.3
 */

/*!
    \qmlproperty float Kuesa::TextureTransform::rotation
    \since Kuesa 1.3
 */

/*!
    \qmlproperty vector3d Kuesa::TextureTransform::scale
    \since Kuesa 1.3
 */

/*!
    \qmlproperty matrix3x3 Kuesa::TextureTransform::matrix
    \since Kuesa 1.3
 */

TextureTransform::TextureTransform(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(parent)
    , m_offset(QVector2D(0.0f, 0.0f))
    , m_scale(QVector2D(1.0f ,1.0f))
    , m_rotation(0.0f)
{
}


const QVector2D &TextureTransform::offset() const
{
    return m_offset;
}

const QVector2D &TextureTransform::scale() const
{
    return m_scale;
}

float TextureTransform::rotation() const
{
    return m_rotation;
}

QMatrix3x3 TextureTransform::matrix() const
{
    QMatrix3x3 mat;
    QMatrix3x3 translation;
    translation(0, 2) = m_offset[0];
    translation(1, 2) = m_offset[1];

    QMatrix3x3 scale;
    scale(0,0) = m_scale[0];
    scale(1,1) = m_scale[1];

    QMatrix3x3 rotation;
    rotation(0,0) = std::cos(m_rotation);
    rotation(1,0) = -std::sin(m_rotation);
    rotation(1,1) = std::cos(m_rotation);
    rotation(0,1) = std::sin(m_rotation);

    return translation * rotation * scale;
}

void TextureTransform::setOffset(const QVector2D &offset)
{
    if (m_offset != offset) {
        m_offset = offset;
        emit offsetChanged(offset);
        emit matrixChanged(matrix());
    }
}

void TextureTransform::setScale(const QVector2D &scale)
{
    if (m_scale != scale) {
        m_scale = scale;
        emit scaleChanged(scale);
        emit matrixChanged(matrix());
    }
}

void TextureTransform::setRotation(float rotation)
{
    if (m_rotation != rotation) {
        m_rotation = rotation;
        emit rotationChanged(rotation);
        emit matrixChanged(matrix());
    }
}

} // Kuesa

QT_END_NAMESPACE
