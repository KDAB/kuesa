/*
    particlegeometry.cpp

    This file is part of Kuesa.

    Copyright (C) 2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mauro Persano <mauro.persano@kdab.com>

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

#include "particlegeometry_p.h"

namespace {
constexpr auto VertexElementSize = 2;
constexpr auto VertexStride = VertexElementSize * sizeof(float);
} // namespace

using namespace Qt3DGeometry;

QT_BEGIN_NAMESPACE

namespace Kuesa {

ParticleGeometry::ParticleGeometry(QNode *parent)
    : QGeometry(parent)
    , m_positionAttribute(new QAttribute(this))
{
    connect(m_positionAttribute, &QAttribute::countChanged, this, [this](uint count) {
        emit particleCountChanged(static_cast<int>(count));
    });
    connect(m_positionAttribute, &QAttribute::bufferChanged, this, &ParticleGeometry::vertexBufferChanged);

    m_positionAttribute->setName(QAttribute::defaultPositionAttributeName());
    m_positionAttribute->setVertexBaseType(QAttribute::Float);
    m_positionAttribute->setVertexSize(2);
    m_positionAttribute->setAttributeType(QAttribute::VertexAttribute);
    m_positionAttribute->setByteStride(VertexStride);
    m_positionAttribute->setByteOffset(0);
    addAttribute(m_positionAttribute);
}

void ParticleGeometry::setParticleCount(int particleCount)
{
    m_positionAttribute->setCount(particleCount);
}

int ParticleGeometry::particleCount() const
{
    return m_positionAttribute->count();
}

void ParticleGeometry::setVertexBuffer(Qt3DGeometry::QBuffer *vertexBuffer)
{
    m_positionAttribute->setBuffer(vertexBuffer);
}

Qt3DGeometry::QBuffer *ParticleGeometry::vertexBuffer() const
{
    return m_positionAttribute->buffer();
}

} // namespace Kuesa

QT_END_NAMESPACE
