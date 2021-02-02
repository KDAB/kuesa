/*
    particlemesh.cpp

    This file is part of Kuesa.

    Copyright (C) 2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "particlemesh_p.h"

#include "particlegeometry_p.h"

using namespace Qt3DRender;

QT_BEGIN_NAMESPACE

namespace Kuesa {

ParticleMesh::ParticleMesh(QNode *parent)
    : QGeometryRenderer(parent)
{
    auto *geometry = new ParticleGeometry(this);
    connect(geometry, &ParticleGeometry::particleCountChanged, this, &ParticleMesh::particleCountChanged);
    connect(geometry, &ParticleGeometry::vertexBufferChanged, this, &ParticleMesh::vertexBufferChanged);
    setGeometry(geometry);

    setPrimitiveType(Points);
}

void ParticleMesh::setParticleCount(int particleCount)
{
    static_cast<ParticleGeometry *>(geometry())->setParticleCount(particleCount);
}

int ParticleMesh::particleCount() const
{
    return static_cast<ParticleGeometry *>(geometry())->particleCount();
}

void ParticleMesh::setVertexBuffer(Qt3DGeometry::QBuffer *vertexBuffer)
{
    static_cast<ParticleGeometry *>(geometry())->setVertexBuffer(vertexBuffer);
}

Qt3DGeometry::QBuffer *ParticleMesh::vertexBuffer() const
{
    return static_cast<ParticleGeometry *>(geometry())->vertexBuffer();
}

} // namespace Kuesa

QT_END_NAMESPACE
